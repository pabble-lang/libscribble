/**
 * \file
 * This file contains a pretty printer of (multiparty) session
 * in form of a Scribble protocol.
 *
 * \headerfile "st_node.h"
 * \headerfile "scribble/print.h"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <st_node.h>
#include "print.h"


int scribble_colour_mode(int colour_mode)
{
  static int mode = 0;
  if (colour_mode == -1) return mode;
  mode = colour_mode;
  return mode;
}

void scribble_fprintf(FILE *stream, const char *format, ...)
{
  int i = 0;
  char orig_str[2560];
  int found, found2;
  static int in_comment = 0;
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(orig_str, 2560, format, argptr);
  va_end(argptr);

  const char *scribble_keywords[] = {
    "and", "as", "at", "by",
    "catch", "choice", "continue", "create", "do",
    "enter", "for", "from",
    "import", "instantiates",
    "interruptible",
    "or", "par",
    "rec",
    "spawns", "throw", "to",
    "with", "if",
    "for", NULL
  };

  const char *scribble_keywords2[] = {
    "global", "local", "protocol", "role", "const", "range", NULL
  };

  // Colour mode
  //
  if (!scribble_colour_mode(-1)) {
    fprintf(stream, "%s", orig_str);
    return;
  }

  // Not colour mode
  //
  for (i=0; i<strlen(orig_str); ++i) {
    if (orig_str[i] == ' ') {
      fprintf(stream, " ");
    } else {
      break;
    }
  }
  char *token = strtok(orig_str, " ");
  if (token == NULL) return;
  do {
    // Comment detection
    if (!in_comment && NULL != strstr(token, "//")) {
      in_comment = 1;
    }
    if (in_comment) {
      if (NULL != strchr(token, '\n')) {
        fprintf(stream, "\033[1;34m%s\033[0m", token);
        in_comment = 0;
      } else {
        fprintf(stream, "\033[1;34m%s\033[0m ", token);
      }
      continue;
    }

    // Keywords detection
    found = 0;
    i = 0;
    while (scribble_keywords[i] != NULL) {
      if (0 == strcmp(token, scribble_keywords[i])) {
        found = 1;
      }
      i++;
    }
    found2 = 0;
    i = 0;
    while (scribble_keywords2[i] != NULL) {
      if (0 == strcmp(token, scribble_keywords2[i])) {
        found2 = 1;
      }
      i++;
    }
    if (found) {
      fprintf(stream, "\033[1;33m%s\033[0m ", token);
    } else if (found2) {
      fprintf(stream, "\033[1;32m%s\033[0m ", token);
    } else {
      if (NULL != strchr(token, '\n') || NULL != strchr(token, '}')) {
        // w/o trailing space
        fprintf(stream, "%s", token);
      } else {
        fprintf(stream, "%s ", token);
      }
    }
  } while ((token = strtok(NULL, " ")) != NULL);
}

void scribble_fprint_node(FILE *stream, st_node *node, int indent)
{
  switch (node->type) {
    case ST_NODE_ROOT:
      scribble_fprint_root(stream, node, indent);
      break;
    case ST_NODE_SENDRECV:
      scribble_fprint_message(stream, node, indent);
      break;
    case ST_NODE_SEND:
      scribble_fprint_send(stream, node, indent);
      break;
    case ST_NODE_RECV:
      scribble_fprint_recv(stream, node, indent);
      break;
    case ST_NODE_CHOICE:
      scribble_fprint_choice(stream, node, indent);
      break;
    case ST_NODE_PARALLEL:
      scribble_fprint_parallel(stream, node, indent);
      break;
    case ST_NODE_RECUR:
      scribble_fprint_recur(stream, node, indent);
      break;
    case ST_NODE_CONTINUE:
      scribble_fprint_continue(stream, node, indent);
      break;
    case ST_NODE_FOR:
      scribble_fprint_for(stream, node, indent);
      break;
    case ST_NODE_ALLREDUCE:
      scribble_fprint_allreduce(stream, node, indent);
      break;
    default:
      fprintf(stderr, "%s:%d %s Unknown node type: %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
  }
}


void scribble_fprint_role(FILE *stream, st_role *role)
{
  scribble_fprintf(stream, "%s", role->name);
  if (role->dimen > 0) {
    assert(role->param != NULL);
    for (int i=0; i<role->dimen; ++i) {
      scribble_fprintf(stream, "[");
      scribble_fprint_expr(stream, role->param[i]);
      scribble_fprintf(stream, "]");
    }
  }
}

void scribble_fprint_expr(FILE *stream, st_expr *expr)
{
  assert(expr != NULL);

  st_expr_eval(expr);

  switch (expr->type) {
    case ST_EXPR_TYPE_RNG:
      if (strcmp(expr->rng->bindvar, "_")) {
        scribble_fprintf(stream, "%s:", expr->rng->bindvar);
      }
      scribble_fprint_expr(stream, expr->rng->from);
      scribble_fprintf(stream, "..");
      scribble_fprint_expr(stream, expr->rng->to);
      break;
    case ST_EXPR_TYPE_ADD:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "+");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SUB:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "-");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_MUL:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "*");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_MOD:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "%%");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_DIV:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "/");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SHL:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, "<<");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SHR:
      scribble_fprintf(stream, "(");
      scribble_fprint_expr(stream, expr->bin->left);
      scribble_fprintf(stream, ">>");
      scribble_fprint_expr(stream, expr->bin->right);
      scribble_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_CONST:
      scribble_fprintf(stream, "%d", expr->num);
      break;
    case ST_EXPR_TYPE_VAR:
      scribble_fprintf(stream, "%s", expr->var);
      break;
    default:
      fprintf(stderr, "%s:%d %s Unknown expr type: %d\n", __FILE__, __LINE__, __FUNCTION__, expr->type);
  };
}


void scribble_fprint_message(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_SENDRECV);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  // From
  scribble_fprintf(stream, "%s(%s) from ",
      node->interaction->msgsig.op == NULL? "" : node->interaction->msgsig.op,
      node->interaction->msgsig.payload == NULL? "" : node->interaction->msgsig.payload);
  scribble_fprint_role(stream, node->interaction->from);

  // To
  scribble_fprintf(stream, " to ");
  for (int i=0; i<node->interaction->nto; ++i) {
    scribble_fprint_role(stream, node->interaction->to[i]);
    if (i!=node->interaction->nto-1)
      scribble_fprintf(stream, ", ");
  }

  scribble_fprintf(stream, ";%s\n", node->marked ? " // <- HERE" : "");
}


void scribble_fprint_send(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_SEND);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  // If
  if (node->interaction->msg_cond != NULL) {
    scribble_fprintf(stream, "if ");
    scribble_fprint_role(stream, node->interaction->msg_cond);
    scribble_fprintf(stream, " ");
  }

  scribble_fprintf(stream, "%s(%s) to ",
      node->interaction->msgsig.op == NULL? "" : node->interaction->msgsig.op,
      node->interaction->msgsig.payload == NULL? "" : node->interaction->msgsig.payload);

  // To
  for (int r=0; r<node->interaction->nto; ++r) {
    scribble_fprint_role(stream, node->interaction->to[r]);
    if (r!=node->interaction->nto-1)
      scribble_fprintf(stream, ", ");
  }

  scribble_fprintf(stream, ";%s\n", node->marked ? " // <- HERE" : "");
}


void scribble_fprint_recv(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_RECV);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  // If
  if (node->interaction->msg_cond != NULL) {
    scribble_fprintf(stream, "if ");
    scribble_fprint_role(stream, node->interaction->msg_cond);
    scribble_fprintf(stream, " ");
  }

  // From
  scribble_fprintf(stream, "%s(%s) from ",
      node->interaction->msgsig.op == NULL? "" : node->interaction->msgsig.op,
      node->interaction->msgsig.payload == NULL? "" : node->interaction->msgsig.payload);
  scribble_fprint_role(stream, node->interaction->from);

  scribble_fprintf(stream, ";%s\n", node->marked ? " // <- HERE" : "");
}


void scribble_fprint_choice(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_CHOICE);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  scribble_fprintf(stream, "choice at ");
  scribble_fprint_role(stream, node->choice->at);
  scribble_fprintf(stream, " %s", node->marked ? "/* HERE */ " : "");
  for (int child=0; child<node->nchild; ++child) {
    scribble_fprint_node(stream, node->children[child], indent);
    if (child != node->nchild-1) {
      scribble_fprintf(stream, " or ");
    }
  }
  scribble_fprintf(stream, "\n");
}


void scribble_fprint_parallel(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_PARALLEL);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  scribble_fprintf(stream, "par %s ", node->marked ? "/* HERE */" : "");
  for (int child=0; child<node->nchild; ++child) {
    scribble_fprint_node(stream, node->children[child], indent);
    if (child != node->nchild-1) {
      scribble_fprintf(stream, " and ");
    }
  }
  scribble_fprintf(stream, "\n");
}


void scribble_fprint_recur(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_RECUR);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  scribble_fprintf(stream, "rec %s {%s\n", node->recur->label, node->marked ? " // <- HERE" : "");
  for (int child=0; child<node->nchild; ++child) {
    scribble_fprint_node(stream, node->children[child], indent+1);
  }

  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");
  scribble_fprintf(stream, "}\n");
}


void scribble_fprint_continue(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_CONTINUE);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  scribble_fprintf(stream, "continue %s;%s\n", node->cont->label, node->marked ? " // <- HERE" : "");
}

void scribble_fprint_for(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_FOR);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");

  scribble_fprintf(stream, "for (%s:", node->forloop->range->bindvar);
  scribble_fprint_expr(stream, node->forloop->range->from);
  scribble_fprintf(stream, "..");
  scribble_fprint_expr(stream, node->forloop->range->to);
  scribble_fprintf(stream, ") { %s\n", node->marked ? " /* HERE */" : "");
  for (int child=0; child<node->nchild; ++child) {
    scribble_fprint_node(stream, node->children[child], indent+1);
  }
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");
  scribble_fprintf(stream, "}\n");
}

void scribble_fprint_allreduce(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_ALLREDUCE);
  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");
  scribble_fprintf(stream, "allreduce %s(%s);%s\n",
      node->interaction->msgsig.op == NULL? "" : node->interaction->msgsig.op,
      node->interaction->msgsig.payload == NULL? "" : node->interaction->msgsig.payload,
      node->marked ? " /* HERE */" : "");
}

void scribble_fprint_root(FILE *stream, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_ROOT);
  scribble_fprintf(stream, "{%s\n", node->marked ? " // <- HERE" : "");

  for (int child=0; child<node->nchild; child++) {
    scribble_fprint_node(stream, node->children[child], indent+1);
  }

  for (int i=0; i<indent; ++i) scribble_fprintf(stream, "  ");
  scribble_fprintf(stream, "}");
}


void scribble_fprint(FILE *stream, st_tree *tree)
{
  for (int i=0; i<tree->info->nimport; ++i) {
    scribble_fprintf(stream, "import %s", tree->info->imports[i]->name);
    if (tree->info->imports[i]->from != NULL) {
      scribble_fprintf(stream, " from %s", tree->info->imports[i]->from);
    }
    if (tree->info->imports[i]->as != NULL) {
      scribble_fprintf(stream, " as %s", tree->info->imports[i]->as);
    }
    scribble_fprintf(stream, ";\n");
  }

  for (int k=0; k<tree->info->nconst; ++k) {
    scribble_fprintf(stream, "const %s", tree->info->consts[k]->name);
    switch (tree->info->consts[k]->type) {
      case ST_CONST_VALUE:
        scribble_fprintf(stream, " = %u\n",
            tree->info->consts[k]->value);
        break;
      case ST_CONST_BOUND:
        scribble_fprintf(stream, " is between %u and %u;\n", tree->info->consts[k]->bounds.lbound, tree->info->consts[k]->bounds.ubound);
        break;
      default:
        scribble_fprintf(stream, "\n");
        fprintf(stderr, "Undefined constant type consts[%d] (%d)\n", k, tree->info->consts[k]->type);
    }
  }

  if (ST_TYPE_GLOBAL == tree->info->type) {
    scribble_fprintf(stream, "global protocol %s ", tree->info->name);
  } else if (ST_TYPE_LOCAL == tree->info->type) {
    scribble_fprintf(stream, "local protocol %s at ", tree->info->name);
    scribble_fprint_role(stream, tree->info->myrole);
  } else assert(0/* unrecognised type */);
  scribble_fprintf(stream, "(");
  for (int role=0; role<tree->info->nrole; ++role) {
    scribble_fprintf(stream, "role ");
    scribble_fprint_role(stream, tree->info->roles[role]);
    if (role != tree->info->nrole-1) {
      scribble_fprintf(stream, ", ");
    }
  }
  for (int group=0; group<tree->info->ngroup; group++) {
    scribble_fprintf(stream, ", group %s={", tree->info->groups[group]->name);
    for (int role=0; role<tree->info->groups[group]->nmemb; role++) {
      scribble_fprint_role(stream, tree->info->groups[group]->membs[role]);
      if (role != tree->info->groups[group]->nmemb-1) {
        scribble_fprintf(stream, ", ");
      }
    }
    scribble_fprintf(stream, "}");
  }


  scribble_fprintf(stream, ") ");

  scribble_fprint_root(stream, tree->root, 0);

  scribble_fprintf(stream, "\n");
}


void scribble_print(st_tree *tree)
{
  scribble_fprint(stdout, tree);
}
