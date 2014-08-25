/**
 * \file
 * This file contains checks whether all roles are defined for a given Scribble protocol.
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/check.h"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#include "scribble/check.h"
#include "scribble/print_utils.h"

static st_tree *t;

static inline int role_is_defined(st_role *role, st_role *decl_roles[], unsigned int ndecl_role)
{
  int found = 0;
  // Roles with special semantics (__rolename)
  if (0 == strncmp(role->name, "__", 2)) {
    if (role->dimen > 0) {
      found = 0;
      fprintf_error(
          stderr, "Built-in role (%s/%u) with parameters not supported\n",
          role->name, role->dimen);
    } else {
      found = 1;
      fprintf_info(stderr, "Built-in role %s\n", role->name);
    }
  }
  for (unsigned int i=0; i<ndecl_role; i++) {
    found |= (strcmp(decl_roles[i]->name, role->name) == 0
              && decl_roles[i]->dimen == role->dimen);
  }
  for (unsigned int i=0; i<t->info->ngroup; i++) {
    found |= (strcmp(t->info->groups[i]->name, role->name) == 0);
  }
  found |= (strcmp(role->name, ST_ROLE_ALL) == 0);
  return found;
}

static int check_node(st_node *node, st_role *decl_roles[], unsigned int ndecl_role)
{
  assert(node != NULL);

  int found = 0;
  int error = 0;

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
      found |= role_is_defined(node->interaction->from, decl_roles, ndecl_role);
      if (!found) {
        fprintf_error(stderr,
            "%s: From (%s/%d) not declared\n",
            __FUNCTION__, node->interaction->from->name, node->interaction->from->dimen);
        node->marked = 1;
        error = 1;
      }
      for (int i=0; i<node->interaction->nto; ++i) {
        found &= role_is_defined(node->interaction->to[i], decl_roles, ndecl_role);
        if (!found) {
          fprintf_error(stderr,
              "%s: To #%d (%s/%d) not declared\n",
              __FUNCTION__, i, node->interaction->to[i]->name, node->interaction->to[i]->dimen);
          node->marked = 1;
          error = 1;
        }
      }
      break;

    case ST_NODE_CONTINUE:
      assert(node->nchild == 0);
      break;

    case ST_NODE_CHOICE:
      found |= role_is_defined(node->choice->at, decl_roles, ndecl_role);
      if (!found) {
        fprintf_error(stderr,
            "%s: Choice role (%s/%d) not declared\n",
            __FUNCTION__, node->choice->at->name, node->choice->at->dimen);
        node->marked = 1;
        error = 1;
      }
      // nobreak
    case ST_NODE_FOR:
    case ST_NODE_ROOT:
    case ST_NODE_PARALLEL:
    case ST_NODE_RECUR:
      for (int i=0; i<node->nchild; ++i) {
        error |= check_node(node->children[i], decl_roles, ndecl_role);
      }
      break;

    default:
      fprintf_error(stderr,
          "%s:%d %s Unknown node type: %d\n",
          __FILE__, __LINE__, __FUNCTION__, node->type);
      break;
  }

  return error;
}


int scribble_check_defined_roles(st_tree *tree)
{
  assert(tree != NULL);
  assert(tree->root != NULL);

  if (tree->info->type != ST_TREE_GLOBAL) {
    fprintf_error(stderr, "%s:%d %s Given protocol is not global protocol\n", __FILE__, __LINE__, __FUNCTION__);
    return 1; // Cannot check so always return error
  }

  t = tree;

  int err = check_node(tree->root, tree->info->roles, tree->info->nrole);

  if (!err) fprintf_info(stderr, "Defined roles check complete\n");
  return err;
}
