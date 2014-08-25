/**
 * \file
 * This file contains checks whether all indices are bound for a given Scribble
 * protocol.
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/check.h"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#ifdef __DEBUG__
#include <sesstype/st_node_print.h>
#endif
#include "scribble/check.h"
#include "scribble/print_utils.h"

#define NO_INDEX -1

// This represents bound variables at one level
typedef struct _env_t {
  struct env_entry {
    int paramidx; // Parameter index, 0..nparams, NO_INDEX = bound by for-loop
    char *name; // bound variable
  } *vars;
  unsigned int count;
  struct _env_t *parent;
} env_t;


/**
 * Check whether all variables used in expression e is defined.
 *
 * @param[in] e        Expression to check.
 * @param[in] paramidx The current parameter index in the parameterised role.
 * @param[in] syms     Currently defined symbols.
 * @param[in] level    Current nesting level of the symbols.
 */
static int check_expr(st_expr *e, unsigned int paramidx, env_t *env, unsigned int level)
{
  env_t *cur_env = NULL;
#ifdef __DEBUG__
  fprintf(stderr, "\n%s:%d:%s(", __FILE__, __LINE__, __FUNCTION__);
  st_expr_fprint(stderr, e);
  fprintf(stderr, ", syms[], level=%d)\n", level);

  fprintf(stderr, "------ Vars ------ @ %u\n", level);
  fprintf(stderr, "name\tidx\tLvl\n");
  cur_env = env;
  while (cur_env != NULL) {
    for (int j=0; j<cur_env->count; j++) {
      fprintf(stderr, "%s\t%d\t%d %3d\n", cur_env->vars[j].name, cur_env->vars[j].paramidx, level, j);
    }
    cur_env = cur_env->parent;
  }
  fprintf(stderr, "------------------\n");
#endif

  switch (e->type) {
    case ST_EXPR_TYPE_VAR:
      cur_env = env;
      while (cur_env != NULL) {
        for (int j=0; j<cur_env->count; j++) {
          if (strcmp(e->var, cur_env->vars[j].name) == 0
              && paramidx != NO_INDEX
              && (cur_env->vars[j].paramidx == NO_INDEX || cur_env->vars[j].paramidx == paramidx)) {
            return 0;
          } else if (strcmp(e->var, cur_env->vars[j].name) == 0 && paramidx == NO_INDEX) {
            return 0;
          }
        }
        cur_env = cur_env->parent;
      }
      fprintf_error(stderr, "Error: binding variable not found for %s.\n", e->var);
      return 1;

    case ST_EXPR_TYPE_ADD:
    case ST_EXPR_TYPE_SUB:
    case ST_EXPR_TYPE_MUL:
    case ST_EXPR_TYPE_DIV:
    case ST_EXPR_TYPE_MOD:
    case ST_EXPR_TYPE_SHL:
    case ST_EXPR_TYPE_SHR:
      return check_expr(e->bin->left, paramidx, env, level) || check_expr(e->bin->right, paramidx, env, level);

    case ST_EXPR_TYPE_SEQ:
    case ST_EXPR_TYPE_CONST:
      return 0;

    default:
      fprintf_error(stderr,
          "%s:%d %s Unknown expr type: %d\n",
          __FILE__, __LINE__, __FUNCTION__, e->type);
  }
  return 1;
}


/**
 * Recursive function to check index bindings for a node.
 *
 */
static int check_node(st_node *node, env_t *env, unsigned int level)
{
  int error = 0;

#ifdef __DEBUG__
  fprintf(stderr, "%s:%d:%s(node, vars[], level=%d)\n", __FILE__, __LINE__, __FUNCTION__, level);
  st_node_fprint(stderr, node, 0);
#endif

  env_t *this_env = NULL;
  this_env = (env_t *)malloc(sizeof(env_t));
  this_env->count = 0;
  this_env->vars = NULL;
  this_env->parent = env;

#ifdef __DEBUG__
  fprintf(stderr, "------ Vars at each node ------ @ %u\n", level);
  fprintf(stderr, "name\tidx\tlvl\n");
  env_t *cur_env = this_env;
  while (cur_env != NULL) {
    for (int j=0; j<cur_env->count; j++) {
      fprintf(stderr, "%s\t%d\t%d %3d\n", cur_env->vars[j].name, cur_env->vars[j].paramidx, level, j);
    }
    cur_env = cur_env->parent;
  }
  fprintf(stderr, "------------------------------\n");
#endif

  switch (node->type) {

    case ST_NODE_SEND:
      fprintf_error(stderr,
          "%s:%d %s Global protocol should not contain SEND statements\n",
          __FILE__, __LINE__, __FUNCTION__);
      break;

    case ST_NODE_RECV:
      fprintf_error(stderr,
          "%s:%d %s Global protocol should not contain RECV statements\n",
          __FILE__, __LINE__, __FUNCTION__);
      break;

    case ST_NODE_SENDRECV:
#ifdef __DEBUG__
      fprintf(stderr,
          "%s:%d:%s Checking SENDRECV node, dimen = %d\n",
          __FILE__, __LINE__, __FUNCTION__, node->interaction->from->dimen);
#endif
      for (int i=0; i<node->interaction->msgsig.npayload; i++) {
        if (node->interaction->msgsig.payloads[i].expr!=NULL) { // Check parametric payload type
          error |= check_expr(node->interaction->msgsig.payloads[i].expr, NO_INDEX, this_env, level);
        }
      }
      for (int i=0; i<node->interaction->from->dimen; i++) {
        if (node->interaction->from->param[i]->type == ST_EXPR_TYPE_RNG) {
          this_env->vars = (struct env_entry *)realloc(this_env->vars, sizeof(struct env_entry) * (this_env->count+1));
          this_env->vars[this_env->count].paramidx = i;
          this_env->vars[this_env->count].name = strdup(node->interaction->from->param[i]->rng->bindvar);
          this_env->count++;
        }
      }
#ifdef __DEBUG__
      fprintf(stderr, "To check RECV code\n");
#endif
      for (int i=0; i<node->interaction->to[0]->dimen; i++) {
        error |= check_expr(node->interaction->to[0]->param[i], i, this_env, level);
      }
      break;

    case ST_NODE_CHOICE:
#ifdef __DEBUG__
      fprintf(stderr,
          "%s:%d:%s Checking CHOICE node\n",
          __FILE__, __LINE__, __FUNCTION__);
#endif
      for (int i=0; i<node->choice->at->dimen; i++) {
        error |= check_expr(node->choice->at->param[i], i, this_env, level);
      }
      break;

    case ST_NODE_CONTINUE:
      assert(node->nchild == 0);
      break;

    case ST_NODE_FOR:
#ifdef __DEBUG__
      fprintf(stderr,
          "%s:%d:%s Checking FOR node\n",
          __FILE__, __LINE__, __FUNCTION__);
#endif
      this_env->vars = (struct env_entry *)realloc(this_env->vars, sizeof(struct env_entry) * (this_env->count+1));
      this_env->vars[this_env->count].paramidx = NO_INDEX;
      this_env->vars[this_env->count].name = strdup(node->forloop->range->bindvar);
      this_env->count++;
      // nobreak
    // ----- Child nodes -----
    case ST_NODE_ROOT:
    case ST_NODE_PARALLEL:
    case ST_NODE_RECUR:
#ifdef __DEBUG__
      fprintf(stderr, "To check child node %d\n", node->type);
#endif
      for (int i=0; i<node->nchild; ++i) {
        error |= check_node(node->children[i], this_env, level+1);
      }
      break;

    default:
      fprintf_error(stderr,
          "%s:%d %s Unknown node type: %d\n",
          __FILE__, __LINE__, __FUNCTION__, node->type);
      break;
  }

  //
  // Free this level.
  //
  if (this_env->count > 0) {
    for (unsigned int i=0; i<this_env->count; i++) {
      free(this_env->vars[i].name);
    }
    free(this_env->vars);
    this_env->count = 0;
  }
  free(this_env);

  return error;
}


/**
 * Checks whether all indices are bound (by variable).
 */
int scribble_check_bound_indices(st_tree *tree)
{
  assert(tree != NULL);
  assert(tree->root != NULL);

  int error = 0;

#ifdef __DEBUG__
  fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__);
#endif

  if (tree->info->type != ST_TREE_GLOBAL) {
    fprintf_error(stderr, "%s:%d %s Given protocol is not global protocol\n", __FILE__, __LINE__, __FUNCTION__);
    return 1; // Cannot check so always return error
  }

  env_t *env = (env_t *)malloc(sizeof(env_t));
  env->count = tree->info->nconst;
  env->vars = (struct env_entry *)malloc(sizeof(struct env_entry) * (env->count+1));;
  for (unsigned int i=0; i<env->count; i++) {
    env->vars[i].paramidx = NO_INDEX;
    env->vars[i].name = strdup(tree->info->consts[i]->name);
  }
  env->parent = NULL;

  error = check_node(tree->root, env, 0);

  for (unsigned int i=0; i<env->count; i++) free(env->vars[i].name);
  free(env->vars);
  free(env);

  if (!error) fprintf_info(stderr, "Index binding check complete\n");
  return error;
}

#undef NO_INDEX
