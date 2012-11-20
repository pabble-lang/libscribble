/**
 * \file
 * This file contains an implementation of projection algorithm
 * from global Scribble into endpoint Scribble.
 *
 * \headerfile "st_node.h"
 * \headerfile "scribble/project.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <st_node.h>
#include "project.h"


st_node *scribble_project_root(st_node *node, char *projectrole)
{
  assert(node != NULL && node->type == ST_NODE_ROOT);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(node->children[i], projectrole);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_message(st_node *node, char *projectrole)
{
  int i;
  assert(node != NULL && node->type == ST_NODE_SENDRECV);
  st_node *local;
  st_node *root = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT);

  assert(node->interaction->to[0] != NULL && node->interaction->from != NULL);

  // Parametrised to
  if (node->interaction->to[0]->param != NULL) {
    for (i=0; i<node->interaction->nto; ++i) {
      if (0 == strcmp(node->interaction->to[i]->name, projectrole)) { // Rule 1, 2
        local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);

        if (node->interaction->from->param != NULL) { // Rule 1, parametrised -> parametrised

          local->interaction->from = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->from->name = strdup(node->interaction->from->name);
          local->interaction->from->param = st_expr_copy(node->interaction->from->param);

        } else if (node->interaction->from->param == NULL) { // Rule 2, non-parametrised -> parametrised

          local->interaction->from = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->from->name = strdup(node->interaction->from->name);
          local->interaction->from->param = NULL;

        }

        // Message condition (XXX: only copy to[0])
        local->interaction->msg_cond = (msg_cond_t *)malloc(sizeof(msg_cond_t));
        local->interaction->msg_cond->name = strdup(node->interaction->to[0]->name);
        local->interaction->msg_cond->param = st_expr_copy(node->interaction->to[0]->param);


        // Message signature
        local->interaction->msgsig.op = node->interaction->msgsig.op == NULL ? NULL : strdup(node->interaction->msgsig.op);
        local->interaction->msgsig.payload = node->interaction->msgsig.payload == NULL ? NULL : strdup(node->interaction->msgsig.payload);

        st_node_append(root, local); // L(T) from R;
      }
    }
  }

  // Parametrised from
  if (node->interaction->from->param != NULL) {
    if (0 == strcmp(node->interaction->from->name, projectrole)) { // Rule 3, 4
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);

      local->interaction->from = NULL;

      local->interaction->nto = node->interaction->nto;
      if (node->interaction->to[0]->param != NULL) { // Rule 3, parametrised -> parametrised

        local->interaction->to = (st_role_t **)calloc(sizeof(st_role_t *), local->interaction->nto);
        for (i=0; i<local->interaction->nto; ++i) {
          local->interaction->to[i] = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->to[i]->name = strdup(node->interaction->to[i]->name);
          local->interaction->to[i]->param = st_expr_copy(node->interaction->to[i]->param);
        }

      } else if (node->interaction->to[0]->param == NULL) { // Rule 4, parametrised -> non-parametirsed

        local->interaction->to = (st_role_t **)calloc(sizeof(st_role_t *), local->interaction->nto);
        for (i=0; i<local->interaction->nto; ++i) {
          local->interaction->to[i] = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->to[i]->name = strdup(node->interaction->to[i]->name);
          local->interaction->to[i]->param = NULL;
        }
      }

      // Message condition
      local->interaction->msg_cond = (msg_cond_t *)malloc(sizeof(msg_cond_t));
      local->interaction->msg_cond->name = strdup(node->interaction->from->name);
      local->interaction->msg_cond->param = st_expr_copy(node->interaction->msg_cond->param);

      // Message signature
      local->interaction->msgsig.op = node->interaction->msgsig.op == NULL ? NULL : strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = node->interaction->msgsig.payload == NULL ? NULL : strdup(node->interaction->msgsig.payload);

      st_node_append(root, local); // L(T) to R;
    }
  }

  // Non-parametrised to
  if (node->interaction->to[0]->param == NULL) { // Rule 5, 7 (normal projection)
    for (i=0; i<node->interaction->nto; ++i) {
      if (0 == strcmp(node->interaction->to[i]->name, projectrole)) {
        local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECV);

        if (node->interaction->from->param != NULL) { // Rule 5 non-parametrised -> parametrised

          local->interaction->from = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->from->name = strdup(node->interaction->from->name);
          local->interaction->from->param = st_expr_copy(node->interaction->from->param);

        } else if (node->interaction->from->param == NULL) {  // Rule 7 non-parametrised -> non-parametrised

          local->interaction->from = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->from->name = strdup(node->interaction->from->name);
          local->interaction->from->param = NULL;

        }

        local->interaction->nto = 0;
        local->interaction->to = NULL;

        // Note: No msg_cond
        local->interaction->msg_cond = NULL;

        // Message signature
        local->interaction->msgsig.op = node->interaction->msgsig.op == NULL ? NULL : strdup(node->interaction->msgsig.op);
        local->interaction->msgsig.payload = node->interaction->msgsig.payload == NULL ? NULL : strdup(node->interaction->msgsig.payload);

        st_node_append(root, local);
      }
    }
  }

  // Non-parametrised from
  if (node->interaction->from->param == NULL) { // Rule 6, 8 (normal projection)
    if (0 == strcmp(node->interaction->from->name, projectrole)) {
      local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_SEND);
      local->interaction->from = NULL;

      local->interaction->nto = node->interaction->nto;
      if (node->interaction->to[0]->param != NULL) { // Rule 6, parametrised -> non-parametrised

        local->interaction->to = (st_role_t **)calloc(sizeof(st_role_t *), local->interaction->nto);
        for (i=0; i<local->interaction->nto; ++i) {
          local->interaction->to[i] = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->to[i]->name = strdup(node->interaction->to[i]->name);
          local->interaction->to[i]->param = st_expr_copy(node->interaction->to[i]->param);
        }

      } else if (node->interaction->to[0]->param == NULL) { // Rule 8, non-parametrised -> non-parametirsed

        local->interaction->to = (st_role_t **)calloc(sizeof(st_role_t *), local->interaction->nto);
        for (i=0; i<local->interaction->nto; ++i) {
          local->interaction->to[i] = (st_role_t *)malloc(sizeof(st_role_t));
          local->interaction->to[i]->name = strdup(node->interaction->to[i]->name);
          local->interaction->to[i]->param = NULL;
        }

      }

      // Note: No msg_cond
      local->interaction->msg_cond = NULL;

      // Message signature
      local->interaction->msgsig.op = node->interaction->msgsig.op == NULL ? NULL : strdup(node->interaction->msgsig.op);
      local->interaction->msgsig.payload = node->interaction->msgsig.payload == NULL ? NULL : strdup(node->interaction->msgsig.payload);

      st_node_append(root, local);
    }
  }


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


st_node *scribble_project_choice(st_node *node, char *projectrole)
{
  assert(node != NULL && node->type == ST_NODE_CHOICE);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CHOICE);

  local->choice->at = (char *)calloc(sizeof(char), strlen(node->choice->at)+1);
  local->choice->at = strdup(node->choice->at);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(node->children[i], projectrole);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_parallel(st_node *node, char *projectrole)
{
  assert(node != NULL && node->type == ST_NODE_PARALLEL);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_PARALLEL);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(node->children[i], projectrole);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_recur(st_node *node, char *projectrole)
{
  assert(node != NULL && node->type == ST_NODE_RECUR);
  st_node *local = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_RECUR);

  local->recur->label = (char *)calloc(sizeof(char), strlen(node->recur->label)+1);
  local->recur->label = strdup(node->recur->label);

  int i = 0;
  st_node *child_node = NULL;
  for (i=0; i<node->nchild; ++i) {
    child_node = scribble_project_node(node->children[i], projectrole);
    if (child_node != NULL) {
      st_node_append(local, child_node);
    }
  }

  return local;
}


st_node *scribble_project_continue(st_node *node, char *projectrole)
{
  return node;
}


st_node *scribble_project_node(st_node *node, char *projectrole)
{
  switch (node->type) {
    case ST_NODE_ROOT:
      return scribble_project_root(node, projectrole);
      break;
    case ST_NODE_SENDRECV:
      return scribble_project_message(node, projectrole);
      break;
    case ST_NODE_CHOICE:
      return scribble_project_choice(node, projectrole);
      break;
    case ST_NODE_PARALLEL:
      return scribble_project_parallel(node, projectrole);
      break;
    case ST_NODE_RECUR:
      return scribble_project_recur(node, projectrole);
      break;
    case ST_NODE_CONTINUE:
      return scribble_project_continue(node, projectrole);
      break;
    case ST_NODE_SEND:
    case ST_NODE_RECV:
    default:
      fprintf(stderr, "%s:%d %s Unknown node type: %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
      return NULL;
      break;
  }
}


st_tree *scribble_project(st_tree *global, char *projectrole)
{
  int i;

  if (ST_TYPE_GLOBAL != global->info->type) {
    fprintf(stderr, "Warn: Not projecting for endpoint protocol.\n");
    return global;
  }


  st_tree *local = st_tree_init((st_tree *)malloc(sizeof(st_tree)));
  local->info->myrole = (char *)calloc(sizeof(char), strlen(projectrole)+1);
  strcpy(local->info->myrole, projectrole);

  st_tree_set_name(local, global->info->name);
  local->info->type = ST_TYPE_LOCAL;
  // Copy imports over.
  for (i=0; i<global->info->nimport; ++i) {
    st_tree_add_import(local, *(global->info->imports[i]));
  }
  // Copy roles over.
  for (i=0; i<global->info->nrole; ++i) {
    if (strcmp(global->info->roles[i]->name, projectrole) == 0) {
      if (global->info->roles[i]->param != NULL) {
        local->info->type = ST_TYPE_PARAMETRISED;
        local->info->myrole = (char *)realloc(local->info->myrole, sizeof(char) * (strlen(projectrole)+4+6+1));
        sprintf(local->info->myrole,"%s[%d..%d]", projectrole, global->info->roles[i]->param->binexpr->left->constant, global->info->roles[i]->param->binexpr->right->constant);
      }
      continue;
    }
    if (global->info->roles[i]->param == NULL) {
      st_tree_add_role(local, global->info->roles[i]->name);
    } else if (global->info->roles[i]->param != NULL) {
      st_tree_add_role_param(local, global->info->roles[i]->name, global->info->roles[i]->param);
    } else assert(1/*Not a valid role*/);
  }
  assert(global->info->nrole == local->info->nrole+1);

  if (global->root != NULL) {
    assert(global->root->type == ST_NODE_ROOT);
    local->root = scribble_project_root(global->root, projectrole);
  }
  return local;
}
