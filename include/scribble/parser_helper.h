#ifndef SCRIBBLE__PARSER_HELPER__H__
#define SCRIBBLE__PARSER_HELPER__H__

#include <assert.h>
#include <string.h>
#include <sesstype/st_node.h>

typedef struct {
  unsigned int count;
  st_node **nodes;
} st_nodes;

typedef struct {
  char *name;
  st_expr *expr;
} symbol_t;

typedef struct {
  int count;
  symbol_t **symbols;
} symbol_table_t;

static symbol_table_t symbol_table;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
static void register_constant_bounds(st_tree *tree, const char *name, unsigned int lbound, unsigned int ubound)
{
  st_tree_add_const(tree, (st_const_t){.name=strdup(name), .type=ST_CONST_BOUND, .bounds.lbound=lbound, .bounds.ubound=ubound});
}

static void register_constant_bounds_inf(st_tree *tree, const char *name, unsigned int lbound)
{
  st_tree_add_const(tree, (st_const_t){.name=strdup(name), .type=ST_CONST_INF, .inf.lbound=lbound});
}

static void register_constant(st_tree *tree, const char *name, unsigned int value)
{
  st_tree_add_const(tree, (st_const_t){.name=strdup(name), .type=ST_CONST_VALUE, .value=value});
}

static void register_range(const char *name, unsigned int from, unsigned int to)
{
  symbol_table.count++;
  symbol_table.symbols = (symbol_t **)realloc(symbol_table.symbols, sizeof(symbol_t *)*symbol_table.count);
  symbol_table.symbols[symbol_table.count-1] = (symbol_t *)malloc(sizeof(symbol_t));
  symbol_table.symbols[symbol_table.count-1]->name = strdup(name);
  symbol_table.symbols[symbol_table.count-1]->expr = st_expr_range(st_expr_constant(from), st_expr_constant(to));
}

static st_role *role_empty()
{
  st_role *r = (st_role *)malloc(sizeof(st_role));
  r->dimen = 0;
  return r;
}

static st_role *role_set_name(st_role *role, char *name)
{
  role->name = strdup(name);
  return role;
}

static st_role *role_add_param(st_role *role, st_expr *param)
{
  if (role->dimen <= 0) {
    role->param = (st_expr **)malloc(sizeof(st_expr *));
    role->param[0] = param;
    role->dimen = 1;
  } else {
    role->param = (st_expr **)realloc(role->param, sizeof(st_expr *) * (role->dimen+1));
    role->param[role->dimen] = param;
    role->dimen++;
  }
  return role;
}

static st_role_group *role_group_set_name(st_role_group *group, char *name)
{
  group->name = strdup(name);
  return group;
}

static st_role_group *role_group_add_role(st_role_group *group, st_role *role)
{
  if (group->nmemb <= 0) {
    group->membs = (st_role **)malloc(sizeof(st_role *));
    group->membs[0] = st_node_copy_role(role);
    group->nmemb = 1;
  } else {
    group->membs = (st_role **)realloc(group->membs, sizeof(st_role *) * (group->nmemb+1));
    group->membs[0] = st_node_copy_role(role);
    group->nmemb++;
  }
  return group;
}

static char *join_str(const char *s0, const char *s1)
{
  int str_len = strlen(s0) + strlen(s1);
  char *s = (char *)calloc(str_len+1, sizeof(char));
  sprintf(s, "%s%s", s0, s1);
  return s;
}

static st_node *par_node(st_node *first_node, st_node *other_nodes)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_PARALLEL);

  node->nchild = other_nodes->nchild + 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = first_node;
  for (int i=0; i<other_nodes->nchild; ++i) {
    node->children[1+i] = other_nodes->children[i];
  }
  return node;
}

static st_node *rec_node(char *label, st_node *child_nodes)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECUR);
  node->recur->label = strdup(label);

  node->nchild = child_nodes->nchild;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  memcpy(node->children, child_nodes->children, sizeof(st_node *) * node->nchild);

  return node;
}

static st_node *choice_node(st_role *role, st_node *first_node, st_node *other_nodes)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CHOICE);
  node->choice->at = st_node_copy_role(role);

  node->nchild = other_nodes->nchild + 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = first_node; // First or-block
  for (int i=0; i<other_nodes->nchild; ++i) {
    node->children[1+i] = other_nodes->children[i];
  }
  return node;
}

static st_node *send_node(st_role* role, st_node_msgsig_t msgsig, msg_cond_t *msg_cond)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
  node->interaction->from = NULL;
  // At the moment, we only accept a single to-role
  node->interaction->nto = 1;
  node->interaction->to = (st_role **)calloc(sizeof(st_role), node->interaction->nto);
  node->interaction->to[0] = st_node_copy_role(role);
  node->interaction->msgsig = msgsig;
  if (msg_cond != NULL) {
    node->interaction->msg_cond = st_node_copy_role(msg_cond);
  }

  return node;
}

static st_node *message_node(st_node_msgsig_t msgsig, st_role *from_role, st_role *to_role)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SENDRECV);
  node->interaction->from = st_node_copy_role(from_role);
  // At the moment, we only accept a single to-role
  node->interaction->nto = 1;
  node->interaction->to = (st_role **)calloc(sizeof(st_role), node->interaction->nto);
  node->interaction->to[0] = st_node_copy_role(to_role);
  node->interaction->msgsig = msgsig;

  return node;
}

static st_node *recv_node(st_role* role, st_node_msgsig_t msgsig, msg_cond_t *msg_cond)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
  node->interaction->from = st_node_copy_role(role);
  node->interaction->nto = 0;
  node->interaction->msgsig = msgsig;
  if (msg_cond != NULL) {
    node->interaction->msg_cond = st_node_copy_role(msg_cond);
  }

  return node;
}

static st_node *foreach_except_node(st_expr *bind_expr, char *except, st_node* body)
{
  assert(bind_expr->type == ST_EXPR_TYPE_RNG);
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_FOR);
  node->forloop->range = st_expr_init_rng(strdup(bind_expr->rng->bindvar), st_expr_copy(bind_expr->rng->from), st_expr_copy(bind_expr->rng->to));
  node->forloop->except = strdup(except);

  node->nchild = 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = body;

  return node;
}

static st_node *foreach_node(st_expr *bind_expr, st_node *body)
{
  assert(bind_expr->type == ST_EXPR_TYPE_RNG);
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_FOR);
  node->forloop->range = st_expr_init_rng(strdup(bind_expr->rng->bindvar), st_expr_copy(bind_expr->rng->from), st_expr_copy(bind_expr->rng->to));

  node->nchild = 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = body;

  return node;
}

static st_node *allreduce_node(st_node_msgsig_t msgsig)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ALLREDUCE);
  node->allreduce->msgsig = msgsig;

  return node;
}

static st_node *ifblk_node(st_role *role, st_node *body)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_IFBLK);
  node->ifblk->cond = st_node_copy_role(role);

  node->nchild = 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = body;

  return node;
}

static st_node *oneof_node(char *role, char *bindvar, st_expr *rngexpr, st_node *body)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ONEOF);
  node->oneof->role = strdup(role);
  node->oneof->range = st_expr_init_rng(strdup(bindvar), st_expr_copy(rngexpr->rng->from), st_expr_copy(rngexpr->rng->to));
  node->oneof->unordered = 0;

  node->nchild = 1;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  node->children[0] = body;

  return node;
}

static st_node *repeat_oneof_node(char *role, char *bindvar, st_expr *rngexpr, st_node *body)
{
  st_node *node = oneof_node(role, bindvar, rngexpr, body);
  node->oneof->unordered = 1;

  return node;
}

static st_node *interaction_block(st_nodes *node_list)
{
  st_node *node = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  node->nchild = node_list->count;
  node->children = (st_node **)calloc(sizeof(st_node *), node->nchild);
  for (int i=0; i<node->nchild; ++i) {
    node->children[i] = node_list->nodes[node_list->count-1-i];
  }

  return node;
}

static st_nodes *interaction_sequence(st_nodes *nodes, st_node *node)
{
  if (nodes == NULL) {
    nodes = memset((st_nodes *)malloc(sizeof(st_nodes)), 0, sizeof(st_nodes));
    nodes->count = 0;
  } else {
    nodes->nodes = (st_node **)realloc(nodes->nodes, sizeof(st_node *) * (nodes->count+1));
    nodes->nodes[nodes->count++] = node;
  }

  return nodes;
}
#pragma clang diagnostic pop

#endif // SCRIBBLE__PARSER_HELPER__H__
