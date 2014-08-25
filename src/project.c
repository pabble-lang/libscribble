/**
 * \file
 * This file contains an implementation of projection algorithm
 * from global Scribble into endpoint Scribble.
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/project.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sesstype/st_node.h>
#include <sesstype/st_node_print.h>
#include "scribble/project.h"
#include "scribble/print_utils.h"

#ifdef SCRIBBLE_TOOL
extern int scribble_codegen_mode;
#endif

st_node *scribble_project_root(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_ROOT);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);
#ifdef __DEBUG__
  fprintf_info(stderr, "%s:%d %s entry\n", __FILE__, __LINE__, __FUNCTION__);
#endif

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(tree, node->children[i], projectrole, env);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_message(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_SENDRECV);
  assert(node->interaction->nto > 0 && node->interaction->to[0] != NULL && node->interaction->from != NULL);
  assert(strcmp(projectrole, tree->info->myrole->name) == 0);
#ifdef __DEBUG__
  fprintf_info(stderr, "%s:%d %s {projectrole: %s} entry\n", __FILE__, __LINE__, __FUNCTION__, projectrole);
  st_node_fprint(stderr, node, 0);
#endif

  st_node *local;
  st_node *root = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  st_role *to   = node->interaction->to[0]; // XXX We are considering only to[0]
  st_role *from = node->interaction->from;

  int to_is_group = 0; // Group declared in protocol parameters eg. group X={...}
  int to_is_anon_group = 0; // Group declared inline (no name) eg. W[1..N]
  int from_is_group = 0; // Group declared in protocol parameters eg. group X={...}
  int from_is_anon_group = 0; // Group declared inline (no name) eg. W[1..N]
  int is_relative = 0;

  // Check if the names are groups names
  for (int group=0; group<tree->info->ngroup; group++) {
    to_is_group   |= (strcmp(to->name, tree->info->groups[group]->name)   == 0);
    from_is_group |= (strcmp(from->name, tree->info->groups[group]->name) == 0);
  }
  is_relative = (from->dimen > 0 && from->param[0]->type == ST_EXPR_TYPE_RNG && strcmp(from->param[0]->rng->bindvar, "_") != 0);

  // Check if the names are (anonymous) group
  to_is_group   |= to_is_anon_group   = to->dimen == 1 && to->param[0]->type == ST_EXPR_TYPE_RNG && !strcmp(to->param[0]->rng->bindvar, "_");
  from_is_group |= from_is_anon_group = from->dimen == 1 && from->param[0]->type == ST_EXPR_TYPE_RNG && !strcmp(from->param[0]->rng->bindvar, "_");


  if (tree->info->myrole->dimen == 0
#ifdef SCRIBBLE_TOOL
      || (scribble_codegen_mode && strncmp(from->name, "__", 2) == 0 && strncmp(to->name, "__", 2) == 0)
#endif
      ) { // Rule 1-2: Non-parametric roles
#ifdef __DEBUG__
    fprintf_info(stderr, "Rule 1-2: Non-parametric rules\n");
#endif

    if ((strcmp(to->name, projectrole) == 0)
#ifdef SCRIBBLE_TOOL
        || (scribble_codegen_mode && strncmp(to->name, "__", 2) == 0)
#endif
        ) { // Rule 1
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 1: Non-parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_role_copy(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    }

    if (strcmp(from->name, projectrole) == 0
#ifdef SCRIBBLE_TOOL
        || (scribble_codegen_mode && strncmp(from->name, "__", 2) == 0)
#endif
        ) { // Rule 2
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 2: Non-parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_role_copy(to);
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    }

  } else { // Rule 3-8: Parametric roles
#ifdef __DEBUG__
    fprintf_info(stderr, "Rule 3-8: parametric rules\n");
#endif

    if (!is_relative && !to_is_group && strcmp(to->name, projectrole) == 0) { // Rule 3
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 3: Parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_role_copy(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig= node->interaction->msgsig;
      local->interaction->msg_cond = st_role_copy(to);
      st_node_append(root, local);
    }

    if (!is_relative && !from_is_group && strcmp(from->name, projectrole) == 0) { // Rule 4
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 4: Parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_role_copy(to);
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = st_role_copy(from);
      st_node_append(root, local);
    }

    if (strcmp(to->name, ST_ROLE_ALL) == 0 && strcmp(from->name, ST_ROLE_ALL) == 0) { // Rule 5 (regardless of projectrole)
#ifdef __DEBUG__
      fprintf_info(stderr, "Trigges Rule 5: All-to-all\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_role_copy(to);
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);

      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_role_copy(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    } else {

      if (to_is_group) { // Rule 6 Group
#ifdef __DEBUG__
        fprintf_info(stderr, "Rule 6: Group\n");
#endif
        // Match receiver
        if (to_is_anon_group) {
          if (strcmp(to->name, projectrole) == 0) {
#ifdef __DEBUG__
            fprintf_info(stderr, "Triggers Rule 6.1: Group (inline)\n");
#endif
            local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
            local->interaction->from = st_role_copy(from);
            local->interaction->nto = 0;
            local->interaction->to = NULL;
            local->interaction->msgsig = node->interaction->msgsig;
            local->interaction->msg_cond = st_role_copy(to);
            st_node_append(root, local);
          }
        } else {
          // Search for the group that this matches
          for (int group=0; group<tree->info->ngroup; group++) {
            if (strcmp(to->name, tree->info->groups[group]->name) == 0) {
              for (int member=0; member<tree->info->groups[group]->nmemb; member++) {
                if (strcmp(ST_ROLE_ALL, projectrole) == 0 // Always match if it's ALL
                    || strcmp(tree->info->groups[group]->membs[member]->name, projectrole) == 0) {
#ifdef __DEBUG__
                  fprintf_info(stderr, "Triggers Rule 6.2: Group (declared)\n");
#endif
                  local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
                  local->interaction->from = st_role_copy(from);
                  local->interaction->nto = 0;
                  local->interaction->to = NULL;
                  local->interaction->msgsig = node->interaction->msgsig;
                  local->interaction->msg_cond = st_role_copy(to);
                  st_node_append(root, local);
                  break;
                }
              }
            }
          }
        }
      }

      if (from_is_group) { // Rule 7 Group
#ifdef __DEBUG__
        fprintf_info(stderr, "Rule 7: Group\n");
#endif
        if (from_is_anon_group) {
          if (strcmp(from->name, projectrole) == 0) {
#ifdef __DEBUG__
            fprintf_info(stderr, "Triggers Rule 7.1: Group (inline)\n");
#endif
            local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
            local->interaction->from = NULL;
            local->interaction->nto = 1;
            local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
            local->interaction->to[0] = st_role_copy(to);
            local->interaction->msgsig = node->interaction->msgsig;
            local->interaction->msg_cond = st_role_copy(from);
            st_node_append(root, local);
          }
        } else {
          // Search for the group that this matches
          for (int group=0; group<tree->info->ngroup; group++) {
            if (strcmp(from->name, tree->info->groups[group]->name) == 0) {
              for (int member=0; member<tree->info->groups[group]->nmemb; member++) {
                if (strcmp(ST_ROLE_ALL, projectrole) == 0 // Always match if it's ALL
                    || strcmp(tree->info->groups[group]->membs[member]->name, projectrole) == 0) {
#ifdef __DEBUG__
                  fprintf_info(stderr, "Triggers Rule 7.2: Group (declared)\n");
#endif
                  local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
                  local->interaction->from = NULL;
                  local->interaction->nto = 1;
                  local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
                  local->interaction->to[0] = st_role_copy(to);
                  local->interaction->msgsig = node->interaction->msgsig;
                  local->interaction->msg_cond = st_role_copy(from);
                  st_node_append(root, local);
                  break;
                }
              }
            }
          }
        }
      }

    }

    if (is_relative && strcmp(to->name, projectrole) == 0) { // Rule 8 Relative role
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 8: Relative rule\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_role_copy(from);
      for (int param=0; param<to->dimen; param++) {
        if ((from->param[param]->type == ST_EXPR_TYPE_CONST)
            ||(from->param[param]->type == ST_EXPR_TYPE_VAR && st_tree_has_constant(tree, from->param[param]->var))) { // Is it an unbounded constant?
          // Do nothing
        } else {
          // inv(e)
          st_expr *inv_to = st_expr_inv(to->param[param]);

          if (inv_to) {
            local->interaction->from->param[param] = inv_to;
          } else {
            fprintf_error(stderr, "No inverse for: ");
            st_expr_fprint(stderr, to->param[param]);
            fprintf(stderr, "\n");
            local->interaction->from->param[param] = st_expr_copy(to->param[param]);
            // TODO Signal expand statements
            assert(0);
          }
        }
      }
      local->interaction->msg_cond = st_role_copy(to); // e

      // --- hack ---
      int role_index = -1;
      for (int x=0; x<tree->info->nrole; x++) {
        if (strcmp(tree->info->roles[x]->name, local->interaction->msg_cond->name) == 0) {
          role_index = x;
        }
      }
      if (local->interaction->msg_cond != NULL) {
        if (local->interaction->msg_cond->dimen == 1
            && local->interaction->msg_cond->param[0]->type == ST_EXPR_TYPE_RNG
            && local->interaction->msg_cond->param[0]->rng->from->type == ST_EXPR_TYPE_CONST
            && local->interaction->msg_cond->param[0]->rng->to->type == ST_EXPR_TYPE_CONST
            && local->interaction->msg_cond->param[0]->rng->from->num <= tree->info->roles[role_index]->param[0]->rng->from->num
            && local->interaction->msg_cond->param[0]->rng->to->num >= tree->info->roles[role_index]->param[0]->rng->to->num) {
          fprintf_error(stderr, "Out of bounds!\n");
          local->marked = 1;
        }
      }
      // --- hack ---

      for (int param=0; param<local->interaction->msg_cond->dimen; param++) {
        // apply(b, e);
        local->interaction->msg_cond->param[param] = st_expr_apply(from->param[param], local->interaction->msg_cond->param[param]);
      }
      local->interaction->msgsig = node->interaction->msgsig;
      st_node_append(root, local);
    }

    if (is_relative && strcmp(from->name, projectrole) == 0) { // Rule 9 Relative role
#ifdef __DEBUG__
      fprintf_info(stderr, "Triggers Rule 9: Relative role\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->nto = node->interaction->nto;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_role_copy(to);
      local->interaction->msg_cond = st_role_copy(from);
      local->interaction->msgsig = node->interaction->msgsig;

      // --- hack ---
      int role_index = -1;
      for (int x=0; x<tree->info->nrole; x++) {
        if (strcmp(tree->info->roles[x]->name, local->interaction->msg_cond->name) == 0) {
          role_index = x;
        }
      }
      if (local->interaction->msg_cond != NULL) {
        if (local->interaction->msg_cond->dimen == 1
            && local->interaction->msg_cond->param[0]->type == ST_EXPR_TYPE_RNG
            && local->interaction->msg_cond->param[0]->rng->from->type == ST_EXPR_TYPE_CONST
            && local->interaction->msg_cond->param[0]->rng->to->type == ST_EXPR_TYPE_CONST
            && local->interaction->msg_cond->param[0]->rng->from->num <= tree->info->roles[role_index]->param[0]->rng->from->num
            && local->interaction->msg_cond->param[0]->rng->to->num >= tree->info->roles[role_index]->param[0]->rng->to->num) {
          fprintf_error(stderr, "Out of bounds!\n");
          local->marked = 1;
        }
      }
      // --- hack ---

      st_node_append(root, local);
    }

  } /* end Parametric roles */

#ifdef __DEBUG__
  fprintf_info(stderr, "%s:%d %s exit\n", __FILE__, __LINE__, __FUNCTION__);
#endif

  if (root->nchild == 0) {

    return NULL;

  } else if (root->nchild == 1) {

    local = root->children[0];
    root->nchild = 0;
    free(root->children);
    return local;

  } else { // More than one children

    assert(root->nchild == 2);
    return root;

  }
}


st_node *scribble_project_choice(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_CHOICE);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CHOICE);

  local->choice->at = (st_role *)malloc(sizeof(st_role));
  local->choice->at = st_role_copy(node->choice->at);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(tree, node->children[i], projectrole, env);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_parallel(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_PARALLEL);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_PARALLEL);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(tree, node->children[i], projectrole, env);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_recur(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_RECUR);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECUR);

  local->recur->label = (char *)calloc(sizeof(char), strlen(node->recur->label)+1);
  local->recur->label = strdup(node->recur->label);

  st_node *child_node = NULL;
  for (int child=0; child<node->nchild; child++) {
    child_node = scribble_project_node(tree, node->children[child], projectrole, env);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_continue(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_CONTINUE);
  return node;
}


st_node *scribble_project_foreach(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_FOR);

  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_FOR);
  local->forloop->range = st_expr_init_rng(strdup(node->forloop->range->bindvar), st_expr_copy(node->forloop->range->from), st_expr_copy(node->forloop->range->to));
  local->forloop->except = node->forloop->except == NULL ? NULL : strdup(node->forloop->except);

  env->exprs = (st_expr **)realloc(env->exprs, sizeof(st_expr *) * (env->count+1));
  env->exprs[env->count] = (st_expr *)malloc(sizeof(st_expr));
  env->exprs[env->count]->type = ST_EXPR_TYPE_RNG;
  env->exprs[env->count]->rng = st_expr_init_rng(strdup(node->forloop->range->bindvar), st_expr_copy(node->forloop->range->from), st_expr_copy(node->forloop->range->to));
  env->count++;

  st_node *child_node = NULL;
  for (int child=0; child<node->nchild; child++) {
    child_node = scribble_project_node(tree, node->children[child], projectrole, env);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  st_expr_free(env->exprs[env->count-1]);
  env->count--;

  return local;
}

st_node *scribble_project_allreduce(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_ALLREDUCE);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ALLREDUCE);
  local->allreduce->msgsig = node->allreduce->msgsig;
  return local;
}

st_node *scribble_project_ifblk_simplify(st_tree *tree, st_node *node, char *projectrole, st_expr *expr, st_expr_list *env)
{
  assert(tree != NULL && node != NULL);

  st_node *local = NULL;
  st_node *local_child = NULL;
  if ((node->type == ST_NODE_SEND || node->type == ST_NODE_RECV) && node->interaction->msg_cond != NULL) {
    if (node->interaction->msg_cond->param[0]->type == expr->type) {
      local = st_node_init((st_node *)malloc(sizeof(st_node)), node->type);
      if (node->interaction->from != NULL) {
        local->interaction->from = st_role_copy(node->interaction->from);
      }
      local->interaction->nto = node->interaction->nto;
      if (node->interaction->to != NULL) {
        local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
        local->interaction->to[0] = st_role_copy(node->interaction->to[0]);
      }
      local->interaction->msgsig = node->interaction->msgsig;
      local->interaction->msg_cond = NULL;
      return local; // Return statement without msg_cond
    } else {
      return NULL; // Return NULL (ie. ignore me)
    }
  } else { // Other types of statements
    local = st_node_init((st_node *)malloc(sizeof(st_node)), node->type);
    switch (local->type) {
      case ST_NODE_ROOT:
        break;
      case ST_NODE_CHOICE:
        local->choice->at = (st_role *)malloc(sizeof(st_role));
        local->choice->at = st_role_copy(node->choice->at);
        break;
      case ST_NODE_PARALLEL:
        break;
      case ST_NODE_RECUR:
        local->recur->label = (char *)calloc(sizeof(char), strlen(node->recur->label)+1);
        local->recur->label = strdup(node->recur->label);
        break;
      case ST_NODE_CONTINUE:
        local->cont->label = strdup(node->cont->label);
        break;
      case ST_NODE_FOR:
        if (expr->type == ST_EXPR_TYPE_VAR && node->forloop->except != NULL && strcmp(node->forloop->except, expr->var) == 0) return NULL; // We are projecting for [i]
        local->forloop->range = st_expr_init_rng(strdup(node->forloop->range->bindvar), st_expr_copy(node->forloop->range->from), st_expr_copy(node->forloop->range->to));
        local->forloop->except = strdup(node->forloop->except);
        break;
      case ST_NODE_ALLREDUCE:
        local->allreduce->msgsig = node->allreduce->msgsig;
        break;
#ifdef PABBLE_DYNAMIC
      case ST_NODE_ONEOF:
        local->oneof->role = strdup(node->oneof->role);
        local->oneof->range = st_expr_init_rng(strdup(node->oneof->range->bindvar), st_expr_copy(node->oneof->range->from), st_expr_copy(node->oneof->range->to));
        break;
#endif
      case ST_NODE_IFBLK:
        local->ifblk->cond = st_role_copy(node->ifblk->cond);
        break;
      default:
        fprintf(stderr, "%s: %d %s Cannot handle node type %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
        assert(0);
    }
    for (int i=0; i<node->nchild; i++) {
      local_child = scribble_project_ifblk_simplify(tree, node->children[i], projectrole, expr, env);
      if (local_child != NULL) st_node_append(local, local_child);
    }
  }
  return local;
}

#ifdef PABBLE_DYNAMIC
st_node *scribble_project_oneof(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_ONEOF);

  if (strcmp(projectrole, node->oneof->role) != 0) return NULL;

  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);
  st_node *temp = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  // if-block for the coordinator
  st_node *coord = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_IFBLK);
  coord->ifblk->cond = (st_role *)malloc(sizeof(st_role));
  coord->ifblk->cond->name = strdup(node->oneof->role);
  coord->ifblk->cond->dimen = 1;
  coord->ifblk->cond->param = (st_expr **)malloc(sizeof(st_expr *));
  coord->ifblk->cond->param[0] = (st_expr *)malloc(sizeof(st_expr));
  coord->ifblk->cond->param[0]->type = ST_EXPR_TYPE_CONST;
  coord->ifblk->cond->param[0]->num = 0; // Infer it later

  // Try to infer it..
  if (node->children[0]->type == ST_NODE_SENDRECV) {
    coord->ifblk->cond->param[0] = st_expr_copy(node->children[0]->interaction->to[0]->param[0]);
  } else if (node->children[0]->type == ST_NODE_ROOT && node->children[0]->nchild > 1 && node->children[0]->children[0]->type == ST_NODE_SENDRECV) {
    coord->ifblk->cond->param[0] = st_expr_copy(node->children[0]->children[0]->interaction->to[0]->param[0]);
  } else {
    // XXX Should also go into ROOT/REC and infer from there.
    fprintf(stderr, "Cannot find coordinator role, using [0] as default\n");
  }

  // Copy over the oneof statement as child if-block
  st_node *coord_oneof = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ONEOF);
  coord_oneof->oneof->role = strdup(node->oneof->role);
  coord_oneof->oneof->range = st_expr_init_rng(strdup(node->oneof->range->bindvar), st_expr_copy(node->oneof->range->from), st_expr_copy(node->oneof->range->to));
  coord_oneof->oneof->unordered = node->oneof->unordered;
  if (coord_oneof != NULL) st_node_append(coord, st_node_append(st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT), coord_oneof));
  if (coord != NULL) st_node_append(local, coord);

  // if-block for the workers
  st_node *worker = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_IFBLK);
  worker->ifblk->cond = (st_role *)malloc(sizeof(st_role));
  worker->ifblk->cond->name = strdup(node->oneof->role);
  worker->ifblk->cond->dimen = 1;
  worker->ifblk->cond->param = (st_expr **)malloc(sizeof(st_expr));
  worker->ifblk->cond->param[0] = st_expr_range(st_expr_copy(node->oneof->range->from), st_expr_copy(node->oneof->range->to));
  worker->ifblk->cond->param[0]->rng->bindvar = strdup(node->oneof->range->bindvar);
  if (worker != NULL) st_node_append(local, worker);

  st_expr *worker_expr = NULL;
  switch (worker->ifblk->cond->param[0]->type) {
    case ST_EXPR_TYPE_RNG:
      worker_expr = (st_expr *)malloc(sizeof(st_expr));
      worker_expr->type = ST_EXPR_TYPE_VAR;
      worker_expr->var = strdup(worker->ifblk->cond->param[0]->rng->bindvar);
      break;
    default:
      worker_expr = worker->ifblk->cond->param[0];
  }

  // Go into children
  for (int i=0; i<node->nchild; i++) {
    temp = scribble_project_node(tree, node->children[i], projectrole, env);
    st_node_append(coord_oneof, scribble_project_ifblk_simplify(tree, temp, projectrole, coord->ifblk->cond->param[0], env));
    st_node_append(worker, scribble_project_ifblk_simplify(tree, temp, projectrole, worker_expr, env));
  }

  return local;
}
#endif


st_node *scribble_project_node(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL);

  switch (node->type) {
    case ST_NODE_ROOT:      return scribble_project_root(tree, node, projectrole, env);
    case ST_NODE_SENDRECV:  return scribble_project_message(tree, node, projectrole, env);
    case ST_NODE_CHOICE:    return scribble_project_choice(tree, node, projectrole, env);
    case ST_NODE_PARALLEL:  return scribble_project_parallel(tree, node, projectrole, env);
    case ST_NODE_RECUR:     return scribble_project_recur(tree, node, projectrole, env);
    case ST_NODE_CONTINUE:  return scribble_project_continue(tree, node, projectrole, env);
    case ST_NODE_FOR:       return scribble_project_foreach(tree, node, projectrole, env);
    case ST_NODE_ALLREDUCE: return scribble_project_allreduce(tree, node, projectrole, env);
#ifdef PABBLE_DYNAMIC
    case ST_NODE_ONEOF:     return scribble_project_oneof(tree, node, projectrole, env);
#endif
    case ST_NODE_IFBLK:
    case ST_NODE_SEND:
    case ST_NODE_RECV:
      fprintf(stderr, "%s:%d %s Invalid node type: %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
      return NULL;
    default:
      fprintf(stderr, "%s:%d %s Unknown node type: %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
      return NULL;
  }
}


st_tree *scribble_project(st_tree *global, char *projectrole)
{
  st_expr_list *env = memset(malloc(sizeof(st_expr_list)), 0, sizeof(st_expr_list));

  if (ST_TREE_GLOBAL != global->info->type) {
    fprintf(stderr, "Warn: Not projecting for endpoint protocol.\n");
    return global;
  }

  st_tree *local = st_tree_init((st_tree *)malloc(sizeof(st_tree)));

  st_tree_set_module(local, global->info->module);
  st_tree_set_name(local, global->info->name);
  local->info->type = ST_TREE_LOCAL;

  // Lookup and copy whole role over.
  for (int role=0; role<global->info->nrole; role++) {
    if (strcmp(global->info->roles[role]->name, projectrole) == 0) {
      local->info->myrole = st_role_copy(global->info->roles[role]);
    }
  }
  if (local->info->myrole == NULL) {
    return local;
  }

  for (int constant=0; constant<global->info->nconst; constant++) {
    st_tree_add_const(local, *(global->info->consts[constant]));
  }

  // Copy imports over.
  for (int import=0; import<global->info->nimport; import++) {
    st_tree_add_import(local, *(global->info->imports[import]));
  }

  // Copy roles over.
  for (int role=0; role<global->info->nrole; role++) {
    if (local->info->myrole->dimen == 0 && strcmp(global->info->roles[role]->name, local->info->myrole->name) == 0 ) {
      /* Ordinary local protocol */
      continue;
    } else {
      st_tree_add_role(local, global->info->roles[role]);
    }
  }

  st_role_group *g = malloc(sizeof(st_role_group));
  g->name = ST_ROLE_ALL;
  g->nmemb = global->info->nrole;
  g->membs = (st_role **)calloc(g->nmemb, sizeof(st_role *));
  for (int role=0; role<global->info->nrole; role++) {
    g->membs[role] = st_role_copy(global->info->roles[role]);
  }
  st_tree_add_role_group(global, g);
  // Copy groups over.
  for (int group=0; group<global->info->ngroup; group++) {
    st_tree_add_role_group(local, global->info->groups[group]);
  }

  // Copy protocol body over.
  if (global->root != NULL) {
    assert(global->root->type == ST_NODE_ROOT);
    local->root = scribble_project_root(local, global->root, projectrole, env);
  }

  local->info->ngroup--;
  free(local->info->groups[local->info->ngroup]);

  free(g->membs);
  free(g);
  free(env);

  return local;
}
