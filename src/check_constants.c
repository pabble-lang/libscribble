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

static int has_only_one_inf(st_tree *tree)
{
  int count = 0;
  for (int i=0; i<tree->info->nconst; i++) {
    if (tree->info->consts[i]->type == ST_CONST_INF) count++;
  }
  return count<=1;
}

static int expr_is_valid(st_expr *expr)
{
  int valid = 1;

  switch (expr->type) {
    case ST_EXPR_TYPE_CONST:
    case ST_EXPR_TYPE_VAR:
    case ST_EXPR_TYPE_SEQ:
    case ST_EXPR_TYPE_RNG:
      valid = 1;
      break;
    case ST_EXPR_TYPE_ADD:
    case ST_EXPR_TYPE_SUB:
      valid &= expr_is_valid(expr->bin->left) && expr_is_valid(expr->bin->right);
      break;
    case ST_EXPR_TYPE_MUL:
    case ST_EXPR_TYPE_DIV:
    case ST_EXPR_TYPE_MOD:
    case ST_EXPR_TYPE_SHL:
    case ST_EXPR_TYPE_SHR:
      valid = 0;
      break;
  }

  return valid;
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
    case ST_NODE_CHOICE:
    case ST_NODE_RECUR:
    case ST_NODE_FOR:
    case ST_NODE_PARALLEL:
    case ST_NODE_ALLREDUCE:
      /* Nothing to do */
      break;
    case ST_NODE_ROOT:
      for (int i=0; i<node->nchild; i++) {
        valid &= node_has_only_valid_exprs(node->children[i]);
      }
      break;
    default:
      fprintf(stderr, "%s:%d Unknown node type %d\n",
          __FILE__, __LINE__, node->type);
  }

  return valid;
}

int scribble_check_constants(st_tree *tree)
{
  assert(tree != NULL);
  assert(tree->root != NULL);

  if (!has_only_one_inf(tree)) {
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
