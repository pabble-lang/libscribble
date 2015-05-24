#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#include <sesstype/st_node_print.h>
#include <sesstype/st_expr.h>
#include "scribble/check.h"
#include "scribble/print_utils.h"

#include <z3.h>


typedef struct __scope_var {
  char const *forall_var;
  Z3_ast ast;
  struct __scope_var *parent;
} scope_var;

static scope_var *cur_scope;
static const st_info *_info;
static Z3_context _ctx;

/** Convenient macros **/

#define INT_VAR(X) (Z3_mk_const(_ctx, Z3_mk_string_symbol(_ctx, (X)), Z3_mk_int_sort(_ctx)))
#define INT_VAL(X) (Z3_mk_int(_ctx, (X), Z3_mk_int_sort(_ctx)))

/** Forward declarations **/

static Z3_ast expr2ast(const st_expr *expr);
static bool scope_push_rng(st_rng_expr_t *range_expr);
static bool scope_push_assign(const char *var, const st_expr *range_expr);
static bool z3prove(Z3_ast f);


static bool role_is_defined(st_role *role)
{
  bool is_defined = false;

  // Check that the role name is defined.
  for (int r=0; r<_info->nrole; r++) {
    if (strcmp(role->name, _info->roles[r]->name) == 0) { // Matching role.
      if (role->dimen == _info->roles[r]->dimen) {
        is_defined = true;
#ifdef __DEBUG__
        fprintf(stderr, "Found matching role [%s/%u].\n", role->name, role->dimen);
#endif
        break;
      }
    }
  }

  return is_defined;
}

static bool role_group_is_defined(st_role *role)
{
  bool is_defined = false;

  // Check that role matches defined group name.
  for (unsigned int g=0; g<_info->ngroup; g++) {
    if (strcmp(role->name, _info->groups[g]->name) == 0) { // Matching group.
      assert(role->dimen == 0 /* Groups should not have dimensions */ );
      is_defined = true;
#ifdef __DEBUG__
      fprintf(stderr, "Found matching group [%s].\n", role->name);
#endif
      break;
    }
  }

  return is_defined;
}

static Z3_ast check_bounds(char const *rolename, unsigned int dimen, Z3_ast ast)
{
  for (int r=0; r<_info->nrole; r++) {
    if (strcmp(_info->roles[r]->name, rolename) == 0) {
      assert(_info->roles[r]->param[dimen]->type == ST_EXPR_TYPE_RNG);
      return Z3_mk_and(_ctx, 2, (Z3_ast[]){
          Z3_mk_le(_ctx, expr2ast(_info->roles[r]->param[dimen]->rng->from), ast),
          Z3_mk_le(_ctx, ast, expr2ast(_info->roles[r]->param[dimen]->rng->to))
        });
    }
  }
  fprintf(stderr, "Cannot find role %s\n", rolename);
  return Z3_mk_false(_ctx);
}

static bool inline role_is_valid(st_role *role)
{
  return (role_is_defined(role) || role_group_is_defined(role));
}

static Z3_ast expr2ast(const st_expr *e)
{
  switch (e->type) {
    case ST_EXPR_TYPE_CONST:
      return INT_VAL(e->num);

    case ST_EXPR_TYPE_VAR:
      return INT_VAR(e->var);

    case ST_EXPR_TYPE_ADD:
      return Z3_mk_add(_ctx, 2, (Z3_ast[]){ expr2ast(e->bin->left), expr2ast(e->bin->right) });

    case ST_EXPR_TYPE_SUB:
      return Z3_mk_sub(_ctx, 2, (Z3_ast[]){ expr2ast(e->bin->left), expr2ast(e->bin->right) });

    case ST_EXPR_TYPE_MUL:
      return Z3_mk_mul(_ctx, 2, (Z3_ast[]){ expr2ast(e->bin->left), expr2ast(e->bin->right) });

    case ST_EXPR_TYPE_DIV:
      return Z3_mk_div(_ctx, expr2ast(e->bin->left), expr2ast(e->bin->right));

    case ST_EXPR_TYPE_MOD:
      return Z3_mk_rem(_ctx, expr2ast(e->bin->left), expr2ast(e->bin->right));

    case ST_EXPR_TYPE_SHL: // x << y
      return Z3_mk_mul(_ctx, 2, (Z3_ast[]){
          expr2ast(e->bin->left),
          Z3_mk_power(_ctx, INT_VAL(2), expr2ast(e->bin->right))
        });

    case ST_EXPR_TYPE_SHR: // x >> y
      return Z3_mk_mul(_ctx, 2, (Z3_ast[]){ expr2ast(e->bin->left), expr2ast(e->bin->right) });

    case ST_EXPR_TYPE_RNG:
      fprintf_error(stderr, "%s:%d %s ERROR: Cannot convert range expression.\n",
          __FILE__, __LINE__, __FUNCTION__);
      return Z3_mk_true(_ctx);

    case ST_EXPR_TYPE_SEQ:
      fprintf_error(stderr, "%s:%d %s ERROR: Cannot convert sequence expression.\n",
          __FILE__, __LINE__, __FUNCTION__);
      return Z3_mk_true(_ctx);

    default:
      fprintf_error(stderr, "%s:%d %s ERROR: Cannot convert Z3_ast for expression type %u.\n",
          __FILE__, __LINE__, __FUNCTION__, e->type);
      return Z3_mk_true(_ctx);
  }
}

static bool scope_push_rng(st_rng_expr_t *rng_def)
{
  // Check if variable already defined
  const scope_var *scope = cur_scope;
  while (scope != NULL) {
    if (scope->forall_var != NULL && strcmp(scope->forall_var, rng_def->bindvar) == 0) {
      fprintf_error(stderr, "%s:%d(%s): Ambiguous index variable %s\n",
          __FILE__, __LINE__, __FUNCTION__, rng_def->bindvar);
      return false;
    }
    scope = scope->parent;
  }

  // Allocate a new scope
  scope_var *new_scope = (scope_var *)malloc(sizeof(scope_var));
  new_scope->forall_var = rng_def->bindvar; // ref.
  new_scope->ast = Z3_mk_and(_ctx, 2, (Z3_ast[]){
      Z3_mk_le(_ctx, expr2ast(rng_def->from), INT_VAR(rng_def->bindvar)),
      Z3_mk_le(_ctx, INT_VAR(rng_def->bindvar), expr2ast(rng_def->to))
    });
  new_scope->parent = cur_scope;
  cur_scope = new_scope;

  return true;
}

static bool scope_push_assign(const char *var, const st_expr *assign_expr)
{
  // Check if variable already defined
  scope_var const *scope = cur_scope;
  while (scope != NULL) {
    if (scope->forall_var != NULL && strcmp(scope->forall_var, var) == 0) {
      fprintf_error(stderr, "%s:%d %s: Ambiguous index variable __\n",
          __FILE__, __LINE__, __FUNCTION__);
      return false;
    }
    scope = scope->parent;
  }


  // Allocate a new scope
  scope_var *new_scope = (scope_var *)malloc(sizeof(scope_var));
  new_scope->forall_var = NULL; // No \forall
  new_scope->ast = Z3_mk_eq(_ctx, INT_VAR(var), expr2ast(assign_expr));
  new_scope->parent = cur_scope;
  cur_scope = new_scope;

  return true;
}

static void scope_pop()
{
  scope_var *scope = cur_scope;
  if (cur_scope == NULL) {
    fprintf_error(stderr, "%s:%d %s Cannot pop empty scope\n",
        __FILE__, __LINE__, __FUNCTION__);
  } else {
    cur_scope = cur_scope->parent;
    scope->parent = NULL;
    // free scope->ast;
    free(scope);
  }
}

static bool node_is_valid(st_node *node)
{
  bool is_valid = true;
  switch (node->type) {
    case ST_NODE_SENDRECV:
      {
        // Prove sender/receiver defined.
        is_valid &= role_is_valid(node->interaction->from);
        for (int t=0; t<node->interaction->nto && is_valid; t++) {
          is_valid &= role_is_valid(node->interaction->to[t]);
        }

        if (is_valid) { // Then prove indices within bounds.
          if (node->interaction->from->dimen > 0) {
            for (unsigned int i=0; i<node->interaction->from->dimen; i++) {
              switch (node->interaction->from->param[i]->type) {
                case ST_EXPR_TYPE_RNG:
                  // Add \forall x.(A<=x<=B)
                  is_valid &= scope_push_rng(node->interaction->from->param[i]->rng);

                  // T from R[x:A..B] to R[y], R[z] --> check_bounds(R, x)
                  is_valid &= z3prove(check_bounds(node->interaction->from->name, i, INT_VAR(node->interaction->from->param[i]->rng->bindvar)));

                  for (int t=0; t<node->interaction->nto; t++) {
                    if (node->interaction->to[t]->dimen == node->interaction->from->dimen) {

                      // Add x'=y
                      char *tmp_name = (char *)calloc(strlen(node->interaction->from->param[i]->rng->bindvar)+2, sizeof(char));
                      sprintf(tmp_name, "%s'", node->interaction->from->param[i]->rng->bindvar);
                      is_valid &= scope_push_assign(tmp_name, node->interaction->to[t]->param[i]);

                      // T from R[x:A..B] to R[y], R[z] --> check_bounds(R, x'=y)
                      //                                    check_bounds(R, x'=z)
                      // To prove:
                      is_valid &= z3prove(check_bounds(node->interaction->to[t]->name, i, INT_VAR(tmp_name)));
                      free(tmp_name);

                      scope_pop(); // Pop to-parameter scope.

                    } else { // Reject if to-roles do not have same dimension
                      is_valid = false;
                    }
                  }
                  scope_pop();
                  break;
                case ST_EXPR_TYPE_CONST:
                case ST_EXPR_TYPE_VAR:
                case ST_EXPR_TYPE_ADD:
                case ST_EXPR_TYPE_SUB:
                case ST_EXPR_TYPE_MUL:
                case ST_EXPR_TYPE_DIV:
                case ST_EXPR_TYPE_MOD:
                case ST_EXPR_TYPE_SHL:
                case ST_EXPR_TYPE_SHR:
                  // T from R[x] to R[y], R[z]; --> check_bounds(R, x)
                  is_valid &= z3prove(check_bounds(node->interaction->from->name, i, expr2ast(node->interaction->from->param[i])));

                  for (int t=0; t<node->interaction->nto; t++) {
                    // T from R[x] to R[y], R[z]; --> check_bounds(R, y), check_bounds(R, z);
                    is_valid &= z3prove(check_bounds(node->interaction->to[t]->name, i, expr2ast(node->interaction->to[t]->param[i])));
                  }
                  break;
                default:
                  assert(0 /* Unknown expression type */);
              }
            }
          }
        }
      }
      break;
    case ST_NODE_FOR:
      is_valid |= scope_push_rng(node->forloop->range);
      if (is_valid) {
        for (int child=0; child<node->nchild && is_valid; child++) {
          is_valid &= node_is_valid(node->children[child]);
        }
        scope_pop();
      }
      break;
    default:
      for (int child=0; child<node->nchild && is_valid; child++) {
        is_valid &= node_is_valid(node->children[child]);
      }
  }
  return is_valid;
}

static void z3_error_handler(Z3_context c, Z3_error_code e)
{
  fprintf_error(stderr, "Z3 Error code: %d\n", e);
  exit(1);
}

static bool prove(Z3_context ctx, Z3_ast f, Z3_bool expect_valid)
{
  bool is_valid = true;
  Z3_model m = 0;
  Z3_ast not_f;

  Z3_push(ctx); // Save current context.

  not_f = Z3_mk_not(ctx, f);
  Z3_assert_cnstr(ctx, not_f);

  switch (Z3_check_and_get_model(ctx, &m)) {
    case Z3_L_FALSE: // Proved.
      if (expect_valid == Z3_L_FALSE) {
        is_valid = true;
      } else {
        is_valid = false;
        fprintf_error(stderr, "Unexpected results for:\n%s\n", Z3_ast_to_string(_ctx, f));
      }
      break;

    case Z3_L_UNDEF: // Cannot prove or disprove.
      is_valid = false; // Either way this is not what we are supposed to expect
      fprintf_error(stderr, "Unexpected results for:\n%s\n", Z3_ast_to_string(_ctx, f));
      fprintf_error(stderr, "Cannot determine if constraints can be satisfied for:\n%s\n",
          Z3_ast_to_string(_ctx, f));
      if (m != 0) {
        /* m should be viewed as a potential counterexample. */
        fprintf_error(stderr, "Potential counterexample:\n%s\n", Z3_model_to_string(ctx, m));
      }
      break;

    case Z3_L_TRUE: // Disproved.
      if (expect_valid == Z3_L_TRUE) {
        is_valid = true;
      } else {
        is_valid = false;
        fprintf_error(stderr, "Index constraints cannot be satisfied for:\n%s\n", Z3_ast_to_string(_ctx, f));
        if (m) {
          /* the model returned by Z3 is a counterexample */
          fprintf_error(stderr, "Counterexample:\n%s\n", Z3_model_to_string(ctx, m));
        }
      }
      break;
  }

  if (m) Z3_del_model(ctx, m);

  Z3_pop(ctx, 1); // Restore context.

  return is_valid;
}

static bool z3prove(Z3_ast f)
{
  bool is_valid = false;

  unsigned int forall_count = 0;
  unsigned int def_count = 0;

  Z3_sort *forall_sorts = NULL;
  Z3_symbol *forall_symbols = NULL;
  Z3_ast *defs = NULL;

  // Setup implication conditions.
  const scope_var *scope = cur_scope;
  while (scope != NULL) {
    if (scope->forall_var != NULL) {
      forall_sorts = (Z3_sort *)realloc(forall_sorts, (forall_count+1)*sizeof(Z3_sort));
      forall_symbols = (Z3_symbol *)realloc(forall_symbols, (forall_count+1)*sizeof(Z3_symbol));
      forall_sorts[forall_count] = Z3_mk_int_sort(_ctx);
      forall_symbols[forall_count] = Z3_mk_string_symbol(_ctx, scope->forall_var);
      forall_count++;
    }
    defs = (Z3_ast *)realloc(defs, (def_count+1)*sizeof(Z3_ast));
    defs[def_count] = scope->ast;
    def_count++;
    scope = scope->parent;
  }

  Z3_ast to_prove = Z3_mk_forall(_ctx,
       0, // weight
       0, NULL, // patterns
       forall_count, forall_sorts, forall_symbols,
       Z3_mk_implies(_ctx, Z3_mk_and(_ctx, def_count, defs), f));

  if (!(is_valid = prove(_ctx, to_prove, Z3_L_FALSE))) {
    fprintf_error(stderr, "%s:%d %s: Unable to prove constraints valid (see above).\n",
        __FILE__, __LINE__, __FUNCTION__);
    fprintf_error(stderr, "AST: %s\n", Z3_ast_to_string(_ctx, to_prove));
    return false;
  }

  return true;
}

bool pabble_check_indices(st_tree *tree)
{
  bool is_valid = false;

  // Setup Z3 variables.
  Z3_config cfg = Z3_mk_config();
  Z3_set_param_value(cfg, "model", "true");
  _ctx = Z3_mk_context(cfg);
  Z3_set_error_handler(_ctx, z3_error_handler);
  Z3_del_config(cfg);

  // Setup variables.
  _info = tree->info;
  cur_scope = NULL;
  unsigned int push_count = 0;

  // Add constants to scope.
  for (unsigned int c=0; c<tree->info->nconst; c++) {
    switch (tree->info->consts[c]->type) {
      case ST_CONST_VALUE:
        scope_push_assign(tree->info->consts[c]->name, st_expr_constant(tree->info->consts[c]->value));
        push_count++;
        break;
      case ST_CONST_BOUND:
        scope_push_rng(
            st_expr_init_rng(
              strdup(tree->info->consts[c]->name),
              st_expr_constant(tree->info->consts[c]->bounds.lbound),
              st_expr_constant(tree->info->consts[c]->bounds.ubound)));
        push_count++;
        break;
      case ST_CONST_INF:
        assert(0);
    }
  }

  is_valid = node_is_valid(tree->root);

  if (is_valid) {
    fprintf_info(stderr, "Indices check complete.\n");
  } else {
    fprintf_error(stderr, "Indices check failed.\n");
  }

  while (push_count-->0) {
    scope_pop();
  }
  assert(cur_scope == NULL);

  Z3_del_context(_ctx);

  return is_valid;
}

#undef INT_VAL
#undef INT_VAR
