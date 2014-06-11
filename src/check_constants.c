/**
 * \file
 * This file contains checks whether all constants satisfy well-formedness
 * conditions for a given Scribble protocol.
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/check.h"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#include <sesstype/st_expr.h>
#include "scribble/check.h"


static char *inf_const = NULL;
static st_tree *_tree = NULL;

static int tree_has_inf(st_tree *tree)
{
  for (int c=0; c<tree->info->nconst; c++) {
    if (tree->info->consts[c]->type == ST_CONST_INF)
      return 1;
  }
  return 0;
}

static int get_inf_count(st_tree *tree)
{
  int count = 0;
  for (int i=0; i<tree->info->nconst; i++) {
    if (tree->info->consts[i]->type == ST_CONST_INF) {
      if (inf_const != NULL) free(inf_const);
      inf_const = strdup(tree->info->consts[i]->name);
      count++;
    }
  }
  return count;
}

static int has_inf(st_expr *e)
{
  switch (e->type) {
    case ST_EXPR_TYPE_VAR:
      if (strcmp(e->var, inf_const) == 0) {
        // This branch of expr has inf
        return 1;
      }
    case ST_EXPR_TYPE_CONST:
    case ST_EXPR_TYPE_SEQ: // Just values
      return 0;
    case ST_EXPR_TYPE_RNG:
      return has_inf(e->rng->from) || has_inf(e->rng->to);
    case ST_EXPR_TYPE_ADD:
    case ST_EXPR_TYPE_SUB:
    case ST_EXPR_TYPE_MUL:
    case ST_EXPR_TYPE_DIV:
    case ST_EXPR_TYPE_MOD:
    case ST_EXPR_TYPE_SHL:
    case ST_EXPR_TYPE_SHR:
      return has_inf(e->bin->left) || has_inf(e->bin->right);
    default:
      fprintf(stderr, "%s:%d Unknown expr type %d\n",
          __FILE__, __LINE__, e->type);
      return 0;
  }
}

static int expr_is_valid(st_expr *e)
{
  switch (e->type) {
    case ST_EXPR_TYPE_VAR:
    case ST_EXPR_TYPE_CONST:
    case ST_EXPR_TYPE_SEQ:
      return 1;
    case ST_EXPR_TYPE_RNG:
      return expr_is_valid(e->rng->from) && expr_is_valid(e->rng->to);
    case ST_EXPR_TYPE_ADD:
    case ST_EXPR_TYPE_SUB:
      return expr_is_valid(e->bin->left) && expr_is_valid(e->bin->right);
    case ST_EXPR_TYPE_MUL:
    case ST_EXPR_TYPE_DIV:
    case ST_EXPR_TYPE_MOD:
    case ST_EXPR_TYPE_SHL:
    case ST_EXPR_TYPE_SHR:
      return !has_inf(e) && !tree_has_inf(_tree); // Expressions with INF must not use these operators
    default:
      fprintf(stderr, "%s:%d Unknown expr type %d\n",
          __FILE__, __LINE__, e->type);
      return 0;
  }
}

static int node_has_only_valid_exprs(st_node *node)
{
  int valid = 1;

  switch (node->type) {
    case ST_NODE_SEND:
      for (int i=0; i<node->interaction->nto; i++) {
        if (node->interaction->to[i]->dimen > 0) {
          for (int j=0; j<node->interaction->to[i]->dimen; j++) {
            valid &= expr_is_valid(node->interaction->to[i]->param[j]);
          }
        }
      }
      break;
    case ST_NODE_RECV:
      if (node->interaction->from->dimen > 0) {
        for (int j=0; j<node->interaction->from->dimen; j++) {
          valid &= expr_is_valid(node->interaction->from->param[j]);
        }
      }
      break;
    case ST_NODE_SENDRECV:
      for (int i=0; i<node->interaction->nto; i++) {
        if (node->interaction->to[i]->dimen > 0) {
          for (int j=0; j<node->interaction->to[i]->dimen; j++) {
            valid &= expr_is_valid(node->interaction->to[i]->param[j]);
          }
        }
      }
      if (node->interaction->from->dimen > 0) {
        for (int j=0; j<node->interaction->from->dimen; j++) {
          valid &= expr_is_valid(node->interaction->from->param[j]);
        }
      }
      break;
    case ST_NODE_CONTINUE:
    case ST_NODE_ALLREDUCE:
    case ST_NODE_CHOICE:
    case ST_NODE_RECUR:
    case ST_NODE_FOR:
    case ST_NODE_PARALLEL:
    case ST_NODE_ROOT:
      break;
    default:
      fprintf(stderr, "%s:%d Unknown node type %d\n",
          __FILE__, __LINE__, node->type);
  }
  for (int i=0; i<node->nchild; i++) {
    valid &= node_has_only_valid_exprs(node->children[i]);
  }

  return valid;
}

static int has_add(st_expr *e)
{
  switch (e->type) {
    case ST_EXPR_TYPE_VAR:
    case ST_EXPR_TYPE_CONST:
    case ST_EXPR_TYPE_SEQ: // Just values
      return 0;
    case ST_EXPR_TYPE_RNG:
      return has_add(e->rng->from) || has_add(e->rng->to);
    case ST_EXPR_TYPE_ADD:
      return 1;
    case ST_EXPR_TYPE_SUB:
    case ST_EXPR_TYPE_MUL:
    case ST_EXPR_TYPE_DIV:
    case ST_EXPR_TYPE_MOD:
    case ST_EXPR_TYPE_SHL:
    case ST_EXPR_TYPE_SHR:
      return has_add(e->bin->left) || has_add(e->bin->right);
    default:
      fprintf(stderr, "%s:%d Unknown expr type %d\n",
          __FILE__, __LINE__, e->type);
      return 0;
  }
}

static int param_is_valid(char *name, unsigned int index, st_expr *e)
{
  if (has_inf(e)) {
    // Strategy 1 (inf): Look for minus (possible for FP)
    return !has_add(e);
  } else {
    // Strategy 2 (no inf): just evaluate the expression
    st_expr_eval(e);
    switch (e->type) {
      case ST_EXPR_TYPE_CONST:
        for (int i=0; i<_tree->info->nrole; i++) {
          if (strcmp(_tree->info->roles[i]->name, name) == 0) {
            return (_tree->info->roles[i]->param[index]->rng->from->num <= e->num
                    && _tree->info->roles[i]->param[index]->rng->to->type == ST_EXPR_TYPE_CONST
                    && e->num <= _tree->info->roles[i]->param[index]->rng->to->num);
          }
        }
        break;
    }
  }
  return 0;
}

int scribble_check_constants(st_tree *tree)
{
  assert(tree != NULL);
  assert(tree->root != NULL);

  _tree = tree;

  if (get_inf_count(tree) > 1) {
    fprintf(stderr, "%s:%d %s Error: Given protocol has more than one unbounded constants\n",
        __FILE__, __LINE__, __FUNCTION__);
    return 1;
  }

  if (!node_has_only_valid_exprs(tree->root)) {
    fprintf(stderr, "%s:%d %s Error: Expressions found containing non +/- operations\n",
        __FILE__, __LINE__, __FUNCTION__);
    return 1;
  }

  fprintf(stderr, "Constants check complete\n");
  return 0;
}
