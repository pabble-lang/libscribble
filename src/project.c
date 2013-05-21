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
#include "scribble/project.h"


st_node *scribble_project_root(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env)
{
  assert(tree != NULL && node != NULL && node->type == ST_NODE_ROOT);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

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

  st_node *local;
  st_node *root = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  st_role *to   = node->interaction->to[0];
  st_role *from = node->interaction->from;

  int to_is_group = 0;
  int from_is_group = 0;
  int is_relative = 0;

  // Check if the names are groups names
  for (int group=0; group<tree->info->ngroup; group++) {
    to_is_group   |= (strcmp(to->name, tree->info->groups[group]->name)   == 0);
    from_is_group |= (strcmp(from->name, tree->info->groups[group]->name) == 0);
  }
  is_relative = (from->dimen > 0 && from->param[0]->type == ST_EXPR_TYPE_RNG);


  if (tree->info->myrole->dimen == 0) { // Rule 1-2: Non-parametric roles

    if (strcmp(to->name, projectrole) == 0) { // Rule 1
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 1: Non-parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_node_copy_role(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    }

    if (strcmp(from->name, projectrole) == 0) { // Rule 2
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 2: Non-parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_node_copy_role(to);
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    }

  } else { // Rule 3-8: Parametric roles

    if (!is_relative && strcmp(to->name, projectrole) == 0) { // Rule 3
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 3: Parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_node_copy_role(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = st_node_copy_role(to);
      st_node_append(root, local);
    }

    if (!is_relative && strcmp(from->name, projectrole) == 0) { // Rule 4
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 4: Parametric participant\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_node_copy_role(to);
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = st_node_copy_role(from);
      st_node_append(root, local);
    }

    if (strcmp(to->name, ST_ROLE_ALL) == 0 && strcmp(from->name, ST_ROLE_ALL) == 0) { // Rule 5 (regardless of projectrole)
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Trigges Rule 5: All-to-all\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;
      local->interaction->nto = 1;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_node_copy_role(to);
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);

      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_node_copy_role(from);
      local->interaction->nto = 0;
      local->interaction->to = NULL;
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      local->interaction->msg_cond = NULL;
      st_node_append(root, local);
    } else {

      if (to_is_group) { // Rule 6 Group
#ifdef __DEBUG__
        fprintf(stderr, "INFO/Triggers Rule 6: Group\n");
#endif
        for (int group=0; group<tree->info->ngroup; group++) {
          if (strcmp(to->name, tree->info->groups[group]->name) == 0) {
            for (int member=0; member<tree->info->groups[group]->nmemb; member++) {
              if (strcmp(tree->info->groups[group]->membs[member]->name, projectrole) == 0) {
                local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
                local->interaction->from = st_node_copy_role(from);
                local->interaction->nto = 0;
                local->interaction->to = NULL;
                local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
                local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
                local->interaction->msg_cond = st_node_copy_role(to);
                st_node_append(root, local);
                break;
              }
            }
          }
        }
      }

      if (from_is_group) { // Rule 7 Group
#ifdef __DEBUG__
        fprintf(stderr, "INFO/Triggers Rule 7: Group\n");
#endif
        for (int group=0; group<tree->info->ngroup; group++) {
          if (strcmp(from->name, tree->info->groups[group]->name) == 0) {
            for (int member=0; member<tree->info->groups[group]->nmemb; member++) {
              if (strcmp(ST_ROLE_ALL, projectrole) == 0 || strcmp(tree->info->groups[group]->membs[member]->name, projectrole) == 0) {
                local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
                local->interaction->from = NULL;
                local->interaction->nto = 1;
                local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
                local->interaction->to[0] = st_node_copy_role(to);
                local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
                local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
                local->interaction->msg_cond = st_node_copy_role(from);
                st_node_append(root, local);
                break;
              }
            }
          }
        }
      }

    }

    if (is_relative && strcmp(to->name, projectrole) == 0) { // Rule 8 Relative role
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 8: Relative rule\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);
      local->interaction->from = st_node_copy_role(from);
      for (int param=0; param<to->dimen; param++) {
        if ((from->param[param]->type == ST_EXPR_TYPE_CONST)
            ||(from->param[param]->type == ST_EXPR_TYPE_VAR && st_tree_is_constant(tree, from->param[param]->var))) { // Is it an unbounded constant?
          // Do nothing
        } else {
          // inv(e)
          st_expr *inv_to = st_expr_inv(to->param[param]);

          if (inv_to) {
            local->interaction->from->param[param] = inv_to;
          } else {
            fprintf(stderr, "ERROR: No inverse for: ");
            st_expr_print(to->param[param]); fflush(stdout);
            fprintf(stderr, "\n");
            local->interaction->from->param[param] = st_expr_copy(to->param[param]);
            // TODO Signal expand statements
            assert(0);
          }
        }
      }
      local->interaction->msg_cond = st_node_copy_role(to); // e
      for (int param=0; param<local->interaction->msg_cond->dimen; param++) {
        // apply(b, e);
        local->interaction->msg_cond->param[param] = st_expr_apply(from->param[param], local->interaction->msg_cond->param[param]);
      }
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      st_node_append(root, local);
    }

    if (is_relative && strcmp(from->name, projectrole) == 0) { // Rule 9 Relative role
#ifdef __DEBUG__
      fprintf(stderr, "INFO/Triggers Rule 9: Relative role\n");
#endif
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->nto = node->interaction->nto;
      local->interaction->to = (st_role **)calloc(local->interaction->nto, sizeof(st_role *));
      local->interaction->to[0] = st_node_copy_role(to);
      local->interaction->msg_cond = st_node_copy_role(from);
      local->interaction->msgsig.op = strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = strdup(node->interaction->msgsig.payload);
      st_node_append(root, local);
    }

  } /* end Parametric roles */

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
  local->choice->at = st_node_copy_role(node->choice->at);

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
  local->forloop->range = (st_rng_expr_t *)malloc(sizeof(st_rng_expr_t));
  local->forloop->range->bindvar = strdup(node->forloop->range->bindvar);
  local->forloop->range->from = st_expr_copy(node->forloop->range->from);
  local->forloop->range->to = st_expr_copy(node->forloop->range->to);

  if (env->count == 0) {
    env->exprs = (st_expr **)malloc(sizeof(st_expr *) * (env->count+1));
  } else {
    env->exprs = (st_expr **)realloc(env->exprs, sizeof(st_expr *) * (env->count+1));
  }
  env->exprs[env->count] = (st_expr *)malloc(sizeof(st_expr));
  env->exprs[env->count]->type = ST_EXPR_TYPE_RNG;
  env->exprs[env->count]->rng = (st_rng_expr_t *)malloc(sizeof(st_rng_expr_t));
  env->exprs[env->count]->rng->bindvar = strdup(node->forloop->range->bindvar);
  env->exprs[env->count]->rng->from = st_expr_copy(node->forloop->range->from);
  env->exprs[env->count]->rng->to = st_expr_copy(node->forloop->range->to);
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
  st_expr_list *env = (st_expr_list *)malloc(sizeof(st_expr_list));

  if (ST_TYPE_GLOBAL != global->info->type) {
    fprintf(stderr, "Warn: Not projecting for endpoint protocol.\n");
    return global;
  }

  st_tree *local = st_tree_init((st_tree *)malloc(sizeof(st_tree)));

  st_tree_set_name(local, global->info->name);
  local->info->type = ST_TYPE_LOCAL;

  // Lookup and copy whole role over.
  for (int role=0; role<global->info->nrole; role++) {
    if (strcmp(global->info->roles[role]->name, projectrole) == 0) {
      local->info->myrole = st_node_copy_role(global->info->roles[role]);
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
  g->name = "All";
  g->nmemb = global->info->nrole;
  g->membs = (st_role **)calloc(g->nmemb, sizeof(st_role *));
  for (int role=0; role<global->info->nrole; role++) {
    g->membs[role] = st_node_copy_role(global->info->roles[role]);
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
