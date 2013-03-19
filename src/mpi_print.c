#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <st_node.h>
#include "mpi_print.h"
#include "print.h"

#define RANK_VARIABLE "rank"
#define SIZE_VARIABLE "size"

#define INDENT \
  do {\
    for (int i=0; i<indent; ++i)\
      mpi_fprintf(stream, "  ");\
  } while (0)

typedef struct {
  char **strings;
  int count;
} string_list;

string_list labels;

// Helper function
void mpi_fprint_const_or_var(FILE *stream, st_tree *tree, st_expr *e)
{
  int printed = 0;
  if (e->type == ST_EXPR_TYPE_CONST) {
      mpi_fprintf(stream, "%d", e->num);
  } else if (e->type == ST_EXPR_TYPE_VAR) {
    for (int constant=0; constant<tree->info->nconst; constant++) {
      // If matches constant name and has defined value
      if (strcmp(tree->info->consts[constant]->name, e->var) == 0 && tree->info->consts[constant]->type == ST_CONST_VALUE) {
        mpi_fprintf(stream, "%u", tree->info->consts[constant]->value);
        printed = 1;
      }
    }
    if (!printed) mpi_fprintf(stream, "%s", e->var);
  }
}

// Helper function
void mpi_find_labels(st_node *node)
{
  int found = 0;
  if (node->type == ST_NODE_SEND || node->type == ST_NODE_RECV) {
    if (strlen(node->interaction->msgsig.op) > 0 && strncmp(node->interaction->msgsig.op, "MPI_", 4) != 0) {
      for (int label=0; label<labels.count && !found; label++) {
        if (strcmp(labels.strings[label], node->interaction->msgsig.op) == 0) {
          found = 1;
        }
      }
      if (!found) {
        if (labels.count == 0) {
          labels.strings = (char **)malloc(sizeof(char *));
        } else {
          labels.strings = (char **)realloc(labels.strings, sizeof(char *) * labels.count);
        }

        labels.strings[labels.count++] = strdup(node->interaction->msgsig.op);
      }
    }
  }

  for (int child=0; child<node->nchild; child++) {
    mpi_find_labels(node->children[child]);
  }
}

// Helper function
void _mpi_fprint_children(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  int skip_node = 0;
  for (int child=0; child<node->nchild; child++) {

    skip_node = 0;
    if (child+1 < node->nchild) {
      // Do Allgather check.
      if (node->children[child]->type == ST_NODE_SEND && node->children[child+1]->type == ST_NODE_RECV) {
        if (strcmp(node->children[child]->interaction->to[0]->name, ST_ROLE_ALL) == 0
            && strcmp(node->children[child+1]->interaction->from->name, ST_ROLE_ALL) == 0) {
          {
            indent++;
            INDENT;
            mpi_fprintf(stream, "/* Allgather/Alltoall\n");
            scribble_fprint_send(stream, node->children[child], indent+1);
            scribble_fprint_recv(stream, node->children[child+1], indent+1);
            INDENT;
            mpi_fprintf(stream, "*/\n");
            mpi_fprint_allgather(stream, tree, node->children[child], indent);
            indent--;
          }
          skip_node = 1;
          child += 1;
        }
      }

      // Do Group-to-group check.
      if (!skip_node && node->children[child]->type == ST_NODE_RECV && node->children[child+1]->type == ST_NODE_SEND
          && node->children[child]->interaction->msg_cond == NULL && node->children[child+1]->interaction->msg_cond == NULL) {
        for (int group=0; group<tree->info->ngroup; group++) {
          if (strcmp(tree->info->groups[group]->name, node->children[child]->interaction->from->name) == 0
              && strcmp(tree->info->groups[group]->name, node->children[child+1]->interaction->to[0]->name) == 0) {
            {
              indent++;
              INDENT;
              mpi_fprintf(stream, "/* Group-to-group\n");
              scribble_fprint_recv(stream, node->children[child], indent+1);
              scribble_fprint_send(stream, node->children[child+1], indent+1);
              INDENT;
              mpi_fprintf(stream, "*/\n");
              mpi_fprint_allgather(stream, tree, node->children[child], indent);
              indent--;
            }
            skip_node = 1;
            child += 1;
          }
        }
      }

      // Do Scatter check.
      if (!skip_node && node->children[child]->type == ST_NODE_SEND && node->children[child+1]->type == ST_NODE_RECV
          && node->children[child]->interaction->msg_cond != NULL && node->children[child+1]->interaction->msg_cond != NULL) {
        if (strcmp(node->children[child]->interaction->to[0]->name, ST_ROLE_ALL) == 0
            && strcmp(node->children[child+1]->interaction->msg_cond->name, ST_ROLE_ALL) == 0) {
          {
            indent++;
            INDENT;
            mpi_fprintf(stream, "/* Scatter\n");
            scribble_fprint_send(stream, node->children[child], indent+1);
            scribble_fprint_recv(stream, node->children[child+1], indent+1);
            INDENT;
            mpi_fprintf(stream, "*/\n");
            mpi_fprint_scatter(stream, tree, node->children[child+1], indent); // root role is in receive line.
            indent--;
          }
          skip_node = 1;
          child += 1;
        }
      }

      // Do Gather check.
      if (!skip_node && node->children[child]->type == ST_NODE_RECV && node->children[child+1]->type == ST_NODE_SEND
          && node->children[child]->interaction->msg_cond != NULL && node->children[child+1]->interaction->msg_cond != NULL) {
        if (strcmp(node->children[child]->interaction->from->name, ST_ROLE_ALL) == 0
            && strcmp(node->children[child+1]->interaction->msg_cond->name, ST_ROLE_ALL) == 0) {
          {
            indent++;
            INDENT;
            mpi_fprintf(stream, "/* Gather\n");
            scribble_fprint_recv(stream, node->children[child], indent+1);
            scribble_fprint_send(stream, node->children[child+1], indent+1);
            INDENT;
            mpi_fprintf(stream, "*/\n");
            mpi_fprint_gather(stream, tree, node->children[child+1], indent); // root role is in send line.
            indent--;
          }
          skip_node = 1;
          child += 1;
        }
      }

    }

    if (!skip_node) {
      mpi_fprint_node(stream, tree, node->children[child], indent+1);
    }

  }
}

void mpi_fprint_allgather(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  INDENT;
  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  // Variable declarations
  mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

  INDENT;
  mpi_fprintf(stream, "%s *sbuf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "%s *rbuf = malloc(sizeof(%s) * count * size);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "MPI_Allgather(sbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", ");
  mpi_fprintf(stream, "rbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", ");
  if ((node->type == ST_NODE_SEND && strcmp(node->interaction->to[0]->name, ST_ROLE_ALL) == 0)
      ||(node->type == ST_NODE_RECV && strcmp(node->interaction->from->name, ST_ROLE_ALL) == 0)) {
    mpi_fprintf(stream, "MPI_COMM_WORLD);\n");
  } else {
    mpi_fprintf(stream, "%s_comm);\n", node->interaction->from == NULL? node->interaction->to[0]->name : node->interaction->from->name);
  }

  INDENT;
  mpi_fprintf(stream, "free(sbuf);\n");
  INDENT;
  mpi_fprintf(stream, "free(rbuf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}

void mpi_fprint_msg_cond(FILE *stream, st_tree *tree, const msg_cond_t *msg_cond, int indent)
{
  static int condition_count = 0;
  assert(msg_cond != NULL);

  int in_group = -1;
  int group_memb_idx = 0;
  if (msg_cond->dimen == 0) {
    for (int group=0; group<tree->info->ngroup; group++) {
      if (strcmp(tree->info->groups[group]->name, msg_cond->name) == 0) {
        in_group = group;
      }
    }
  }

  // No dimension, ie. non-parameterised role.
  if (msg_cond->dimen == 0 && in_group == -1) {
    // if ( rank == Worker_RANK )
    mpi_fprintf(stream, "if ( %s == %s_RANK ) ", RANK_VARIABLE, msg_cond->name);
  }

  // 1D parameterised role.
  if (msg_cond->dimen == 1) {
    switch (msg_cond->param[0]->type) {
      case ST_EXPR_TYPE_CONST:
      case ST_EXPR_TYPE_VAR:
      case ST_EXPR_TYPE_ADD:
      case ST_EXPR_TYPE_SUB:
      case ST_EXPR_TYPE_MUL:
      case ST_EXPR_TYPE_DIV:
      case ST_EXPR_TYPE_MOD:
      case ST_EXPR_TYPE_SHL:
      case ST_EXPR_TYPE_SHR:
        // if ( rank == Worker_RANK(N-1) )
        mpi_fprintf(stream, "if ( %s == %s_RANK(", RANK_VARIABLE, msg_cond->name);
        mpi_fprint_expr(stream, msg_cond->param[0]);
        mpi_fprintf(stream, ") ) ");
        break;

      case ST_EXPR_TYPE_RNG:
        // if ( Worker_RANK(1) <= rank && rank <= Worker_RANK(N) )
        mpi_fprintf(stream, "if ( %s_RANK(", RANK_VARIABLE, msg_cond->name);
        mpi_fprint_expr(stream, msg_cond->param[0]->rng->from);
        mpi_fprintf(stream, ") <= %s && %s <= %s_RANK(", RANK_VARIABLE, RANK_VARIABLE, msg_cond->name);
        mpi_fprint_expr(stream, msg_cond->param[0]->rng->from);
        mpi_fprintf(stream, ") ) ");
        break;

      default:
        assert(0 /* No other forms of conditions are allowed */);
    }
  }

  if (msg_cond->dimen >= 2 || in_group != -1) {
    mpi_fprintf(stream, "int cond%d = 0;\n", condition_count);

    while (1) {
      if (in_group != -1) {
        INDENT;
        mpi_fprintf(stream, "// Group %s #%d\n", tree->info->groups[in_group]->name, group_memb_idx);
        msg_cond = tree->info->groups[in_group]->membs[group_memb_idx];
      }

      for (int param=0; param<msg_cond->dimen-1; param++) {
        // for (int x=1; x<=N; x++)
        // for (int y=1; y<=N-1; y++)
        // for (int z=2; z<=N; z++)
        if (msg_cond->param[param]->type == ST_EXPR_TYPE_RNG) {
          // 1..N
          indent += param;
          INDENT;
          mpi_fprintf(stream, "for (int %c=", 'x'+param);
          mpi_fprint_const_or_var(stream, tree, msg_cond->param[param]->rng->from);
          mpi_fprintf(stream, "; %c<=", 'x'+param);
          mpi_fprint_const_or_var(stream, tree, msg_cond->param[param]->rng->to);
          mpi_fprintf(stream, "; %c++)\n", 'x'+param);
          indent -= param;
        } else {
          // 1 -> 1..1
          indent += param;
          INDENT;
          mpi_fprintf(stream, "for (int %c=", 'x'+param);
          mpi_fprint_const_or_var(stream, tree, msg_cond->param[param]);
          mpi_fprintf(stream, "; %c<=", 'x'+param);
          mpi_fprint_const_or_var(stream, tree, msg_cond->param[param]);
          mpi_fprintf(stream, "; %c++)\n", 'x'+param);
          indent -= param;
        }
      }
      if (msg_cond->param[msg_cond->dimen-1]->type == ST_EXPR_TYPE_RNG) {
        indent += (msg_cond->dimen - 1);
        INDENT;
        // cond0 |= ( Worker_RANK(x,y,1) <= rank && rank <= Worker_RANK(x,y,N) );
        mpi_fprintf(stream, "cond%d |= ( %s_RANK(", condition_count, msg_cond->name);
        for (int param=0; param<msg_cond->dimen-1; param++) mpi_fprintf(stream, "%c,", 'x'+param);
        mpi_fprint_const_or_var(stream, tree, msg_cond->param[msg_cond->dimen-1]->rng->from);
        mpi_fprintf(stream, ") <= %s && %s <= %s_RANK(", RANK_VARIABLE, RANK_VARIABLE, msg_cond->name);
        for (int param=0; param<msg_cond->dimen-1; param++) mpi_fprintf(stream, "%c", 'x'+param);
        mpi_fprint_const_or_var(stream, tree, msg_cond->param[msg_cond->dimen-1]->rng->to);
        mpi_fprintf(stream, ") <= %s && %s <= %s_RANK(", RANK_VARIABLE, RANK_VARIABLE, msg_cond->name);
        mpi_fprintf(stream, ") );\n");
        indent -= (msg_cond->dimen - 1);
      } else {
        indent += (msg_cond->dimen - 1);
        INDENT;
        // cond0 |= ( rank == Worker_RANK(x,y,1) );
        mpi_fprintf(stream, "cond%d |= ( %s == %s_RANK(", condition_count, RANK_VARIABLE,  msg_cond->name);
        for (int param=0; param<msg_cond->dimen-1; param++) mpi_fprintf(stream, "%c,", 'x'+param);
        mpi_fprint_const_or_var(stream, tree, msg_cond->param[msg_cond->dimen-1]);
        mpi_fprintf(stream, ") );\n", condition_count, RANK_VARIABLE,  msg_cond->name);
        indent -= (msg_cond->dimen - 1);
      }

      if (in_group != -1 && group_memb_idx < tree->info->groups[in_group]->nmemb-1) {
        group_memb_idx++;
        continue;
      }

      break; // For normal case (ie. in_group == -1) this is a one-off loop
    }

    INDENT;
    mpi_fprintf(stream, "if (cond%d) ", condition_count);

    condition_count++;
  }

}

void mpi_fprintf(FILE *stream, const char *format, ...)
{
  char orig_str[2560];
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(orig_str, 2560, format, argptr);
  va_end(argptr);

  fprintf(stream, "%s", orig_str);
}

void mpi_fprint_choice(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_CHOICE);
  INDENT;
  mpi_fprintf(stream, "/* Choice \n");
  scribble_fprint_node(stream, node, indent+1);
  INDENT;
  mpi_fprintf(stream, "*/\n");


  // This is choice role check, not conditional
  mpi_fprint_msg_cond(stream, tree, node->choice->at, indent);

    indent++;

    INDENT;
    mpi_fprintf(stream, "if (0/* condition */) { // branch decision\n");

      indent++;

      // Representing all branches
      for (int child=0; node->nchild; child++) {

        indent++;

        mpi_fprint_node(stream, tree, node->children[child], indent);

        indent--;

        if (child != node->nchild-1) {
          INDENT;
          mpi_fprintf(stream, "} else if (0/* condition */) { // branch decision\n");
        }
      }

      indent--;

    INDENT;
    mpi_fprintf(stream, "} // branch decision\n");

    indent--;

  INDENT;
  mpi_fprintf(stream, "} else {\n");

    indent++;

    INDENT;
    mpi_fprintf(stream, "MPI_Status status;\n");

    INDENT;
    mpi_fprintf(stream, "MPI_Probe(%d, MPI_ANY_TAG, MPI_COMM_WORLD, &status);\n",
        node->choice->at->param[0]->num);

    INDENT;
    mpi_fprintf(stream, "switch (status.MPI_TAG) {");

      for (int child=0; child<node->nchild; child++) {
        if (node->children[child]->type == ST_NODE_ROOT) {

          // choice at X { U from X; U to Y; /* Multiple lines */ }
          indent++;
          INDENT;
          mpi_fprintf(stream, "case %s:\n", node->children[child]->children[0]->interaction->msgsig.op);
          mpi_fprint_node(stream, tree, node->children[child], indent);
          INDENT;
          mpi_fprintf(stream, "break;\n");
          indent--;

        } else if (node->children[child]->type == ST_NODE_SEND || node->children[child]->type == ST_NODE_RECV) {

          // choice at X { U from X; }
          indent++;
          INDENT;
          mpi_fprintf(stream, "case %s:\n", node->children[child]->interaction->msgsig.op);
          mpi_fprint_node(stream, tree, node->children[child], indent);
          INDENT;
          mpi_fprintf(stream, "break;\n");
          indent--;

        }
      }

    INDENT;
    mpi_fprintf(stream, "} // switch\n");

  indent--;

  mpi_fprintf(stream, "} // rank selection\n");
}

void mpi_fprint_node(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  switch (node->type) {
    case ST_NODE_ROOT:      mpi_fprint_root(stream, tree, node, indent); break;
    case ST_NODE_SENDRECV:  assert(0/* Global protocol not possible */); break;
    case ST_NODE_SEND:      mpi_fprint_send(stream, tree, node, indent); break;
    case ST_NODE_RECV:      mpi_fprint_recv(stream, tree, node, indent); break;
    case ST_NODE_CHOICE:    mpi_fprint_choice(stream, tree, node, indent); break;
    case ST_NODE_PARALLEL:  mpi_fprint_parallel(stream, tree, node, indent); break;
    case ST_NODE_RECUR:     mpi_fprint_recur(stream, tree, node, indent); break;
    case ST_NODE_CONTINUE:  mpi_fprint_continue(stream, tree, node, indent); break;
    case ST_NODE_FOR:       mpi_fprint_for(stream, tree, node, indent); break;
    case ST_NODE_ALLREDUCE: mpi_fprint_allreduce(stream, tree, node, indent); break;
    default:
      fprintf(stderr, "%s:%d %s Unknown node type: %d\n", __FILE__, __LINE__, __FUNCTION__, node->type);
  }
}

void mpi_fprint_expr(FILE *stream, st_expr *expr)
{
  assert(expr != NULL);

  st_expr_eval(expr);

  switch (expr->type) {
    case ST_EXPR_TYPE_ADD:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "+");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SUB:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "-");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_MUL:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "*");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_MOD:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "%%");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_DIV:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "/");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SHL:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, "<<");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_SHR:
      mpi_fprintf(stream, "(");
      mpi_fprint_expr(stream, expr->bin->left);
      mpi_fprintf(stream, ">>");
      mpi_fprint_expr(stream, expr->bin->right);
      mpi_fprintf(stream, ")");
      break;
    case ST_EXPR_TYPE_CONST:
      mpi_fprintf(stream, "%d", expr->num);
      break;
    case ST_EXPR_TYPE_VAR:
      mpi_fprintf(stream, "%s", expr->var);
      break;
    case ST_EXPR_TYPE_RNG:
      assert(0 /* Only support printing pure expression */);
      break;
    default:
      fprintf(stderr, "%s:%d %s Unknown expr type: %d\n", __FILE__, __LINE__, __FUNCTION__, expr->type);
  };
}

void mpi_fprint_datatype(FILE *stream, st_node_msgsig_t msgsig)
{
  if (strcmp(msgsig.payload, "int") == 0) {
    mpi_fprintf(stream, "MPI_INT");
  } else if (strcmp(msgsig.payload, "float") == 0) {
    mpi_fprintf(stream, "MPI_FLOAT");
  } else if (strcmp(msgsig.payload, "double") == 0) {
    mpi_fprintf(stream, "MPI_DOUBLE");
  } else if (strcmp(msgsig.payload, "char") == 0) {
    mpi_fprintf(stream, "MPI_CHAR");
  }
}




void mpi_fprint_rank(FILE *stream, st_expr *param, const char *replace, const char *with)
{
  switch (param->type) {
    case ST_EXPR_TYPE_VAR: // Constant
      if (replace != NULL && strcmp(replace, param->var) == 0) {
        mpi_fprintf(stream, "%s", with);
      } else {
        mpi_fprintf(stream, "%s", param->var);
      }
      break;

    case ST_EXPR_TYPE_CONST:
      mpi_fprintf(stream, "%d", param->num);
      break;

    case ST_EXPR_TYPE_ADD:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "+");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_SUB:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "-");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_MUL:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "*");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_DIV:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "/");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_MOD:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "%%");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_SHL:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, "<<");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    case ST_EXPR_TYPE_SHR:
      mpi_fprintf(stream, "(");
      mpi_fprint_rank(stream, param->bin->left, replace, with);
      mpi_fprintf(stream, ">>");
      mpi_fprint_rank(stream, param->bin->right, replace, with);
      mpi_fprintf(stream, ")");
      break;

    default:
      assert(0 /*Not supported*/);
  }
}

void mpi_fprint_send(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_SEND);

  INDENT;
  // If
  if (node->interaction->msg_cond != NULL) {
    mpi_fprint_msg_cond(stream, tree, node->interaction->msg_cond, indent);
  }

  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  // Variable declarations
  mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

  INDENT;
  mpi_fprintf(stream, "%s *buf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "MPI_Send(buf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", ");
  char *rankbindvar = NULL;
  if (node->interaction->msg_cond != NULL) { // There is msg_cond, so need to do role->rank conversion
    if (node->interaction->msg_cond->dimen == 0) { // Ordinary roles
      mpi_fprintf(stream, "%s_RANK", node->interaction->msg_cond->name);
    } else if (node->interaction->msg_cond->dimen == 1) { // Linear Parameterised roles
      mpi_fprintf(stream, "%s_RANK(", node->interaction->msg_cond->name);
      for (int param=0; param<node->interaction->msg_cond->dimen; param++) {
        if (param!=0) mpi_fprintf(stream, ",");
        if (node->interaction->msg_cond->param[param]->type == ST_EXPR_TYPE_RNG
            && node->interaction->msg_cond->param[param]->rng->bindvar != NULL) {
          rankbindvar = strdup(node->interaction->msg_cond->param[param]->rng->bindvar);
        }
        mpi_fprint_rank(stream, node->interaction->to[param]->param[param], rankbindvar, RANK_VARIABLE);
      }
      mpi_fprintf(stream, ")");
    } else if (node->interaction->msg_cond->dimen == 2) {

      mpi_fprintf(stream, "%s+(", RANK_VARIABLE);
      if (node->interaction->msg_cond->param[0]->type == ST_EXPR_TYPE_RNG
          && node->interaction->msg_cond->param[0]->rng->bindvar != NULL) {
        rankbindvar = strdup(node->interaction->msg_cond->param[0]->rng->bindvar);
      }
      mpi_fprint_rank(stream, node->interaction->to[0]->param[0], rankbindvar, "0");
      mpi_fprintf(stream, ")*(");
      mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[0]->rng->to);
      mpi_fprintf(stream, "-");
      mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[0]->rng->from);
      mpi_fprintf(stream, ")+");
      if (node->interaction->msg_cond->param[1]->type == ST_EXPR_TYPE_RNG
          && node->interaction->msg_cond->param[1]->rng->bindvar != NULL) {
        rankbindvar = strdup(node->interaction->msg_cond->param[1]->rng->bindvar);
        mpi_fprint_rank(stream, node->interaction->to[0]->param[1], rankbindvar, "0");
      } else {
        mpi_fprint_const_or_var(stream, tree, node->interaction->to[0]->param[1]);
        mpi_fprintf(stream, "-");
        mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[1]);
      }

    } else {
      assert(0); // Not supported > 2 dimension
    }
  }
  mpi_fprintf(stream, ", ");
  mpi_fprintf(stream, node->interaction->msgsig.op == NULL ? 0: node->interaction->msgsig.op); // This should be a constant
  mpi_fprintf(stream, ", MPI_COMM_WORLD);\n");

  INDENT;
  mpi_fprintf(stream, "free(buf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}

void mpi_fprint_recv(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_RECV);

  INDENT;
  // If
  if (node->interaction->msg_cond != NULL) {
    mpi_fprint_msg_cond(stream, tree, node->interaction->msg_cond, indent);
  }

  mpi_fprintf(stream, "{\n");

    indent++;
    INDENT;

    // Variable declarations
    mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

    INDENT;
    mpi_fprintf(stream, "%s *buf = malloc(sizeof(%s) * count);\n",
        node->interaction->msgsig.payload,
        node->interaction->msgsig.payload);

    INDENT;
    mpi_fprintf(stream, "MPI_Recv(buf, count, ");
    mpi_fprint_datatype(stream, node->interaction->msgsig);
    mpi_fprintf(stream, ", ");
    char *rankbindvar = NULL;
    if (node->interaction->msg_cond != NULL) { // There is msg_cond, so need to do role->rank conversion
      if (node->interaction->msg_cond->dimen == 0) { // Ordinary roles
        mpi_fprintf(stream, "%s_RANK", node->interaction->msg_cond->name);
      } else if (node->interaction->msg_cond->dimen == 1) { // Linear Parameterised roles
        mpi_fprintf(stream, "%s_RANK(", node->interaction->msg_cond->name);
        for (int param=0; param<node->interaction->msg_cond->dimen; param++) {
          if (param!=0) mpi_fprintf(stream, ",");
          if (node->interaction->msg_cond->param[param]->type == ST_EXPR_TYPE_RNG
              && node->interaction->msg_cond->param[param]->rng->bindvar != NULL) {
            rankbindvar = strdup(node->interaction->msg_cond->param[param]->rng->bindvar);
          }
          mpi_fprint_rank(stream, node->interaction->from->param[param], rankbindvar, RANK_VARIABLE);
        }
        mpi_fprintf(stream, ")");
      } else if (node->interaction->msg_cond->dimen == 2) {

        if (node->interaction->msg_cond->param[0]->type == ST_EXPR_TYPE_RNG
            && node->interaction->msg_cond->param[0]->rng->bindvar != NULL) {
          rankbindvar = strdup(node->interaction->msg_cond->param[0]->rng->bindvar);
        }
        mpi_fprint_rank(stream, node->interaction->from->param[0], rankbindvar, "0");
        mpi_fprintf(stream, ")*(");
        mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[0]->rng->to);
        mpi_fprintf(stream, "-");
        mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[0]->rng->from);
        mpi_fprintf(stream, ")+");
        if (node->interaction->msg_cond->param[1]->type == ST_EXPR_TYPE_RNG
            && node->interaction->msg_cond->param[1]->rng->bindvar != NULL) {
          rankbindvar = strdup(node->interaction->msg_cond->param[1]->rng->bindvar);
          mpi_fprint_rank(stream, node->interaction->from->param[1], rankbindvar, "0");
        } else {
          mpi_fprint_const_or_var(stream, tree, node->interaction->from->param[1]);
          mpi_fprintf(stream, "-");
          mpi_fprint_const_or_var(stream, tree, node->interaction->msg_cond->param[1]);
        }

      } else {
        assert(0); // Not supported > 2 dimension
      }
    }
    mpi_fprintf(stream, ", ");
    mpi_fprintf(stream, node->interaction->msgsig.op == NULL ? 0: node->interaction->msgsig.op); // This should be a constant
    mpi_fprintf(stream, ", MPI_COMM_WORLD, MPI_STATUS_IGNORE);\n"); // Ignore status (this is not non-blocking)

    INDENT;
    mpi_fprintf(stream, "free(buf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}


void mpi_fprint_parallel(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_PARALLEL);

  INDENT;
  mpi_fprintf(stream, "#pragma omp sections\n");

  INDENT;
  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  for (int child=0; child<node->nchild; ++child) {
    INDENT;
    mpi_fprintf(stream, "#pragma omp section\n");

    INDENT;
    mpi_fprintf(stream, "{\n");
    mpi_fprint_node(stream, tree, node->children[child], indent);

    INDENT;
    mpi_fprintf(stream, "}\n");
  }

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}


void mpi_fprint_recur(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_RECUR);

  INDENT;
  mpi_fprintf(stream, "/* Recur\n");
  scribble_fprint_node(stream, node, indent+1);
  INDENT;
  mpi_fprintf(stream, "*/\n");

  INDENT;
  mpi_fprintf(stream, "while (1/* CHANGEME */) {\n");

  _mpi_fprint_children(stream, tree, node, indent);

    indent++;
    INDENT;
    mpi_fprintf(stream, "break;\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}


void mpi_fprint_continue(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_CONTINUE);

  INDENT;
  mpi_fprintf(stream, "continue;\n");
}

void mpi_fprint_for(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_FOR);
  INDENT;

  mpi_fprintf(stream, "for (int %s = ", node->forloop->range->bindvar);
  mpi_fprint_expr(stream, node->forloop->range->from);
  mpi_fprintf(stream, "; %s <= ", node->forloop->range->bindvar);
  mpi_fprint_expr(stream, node->forloop->range->to);
  mpi_fprintf(stream, "; %s++) {\n", node->forloop->range->bindvar);
  indent++;
  for (int child=0; child<node->nchild; ++child) {
    mpi_fprint_node(stream, tree, node->children[child], indent);
  }
  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}


void mpi_fprint_scatter(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_RECV);

  INDENT;
  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  // Variable declarations
  mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

  INDENT;
  mpi_fprintf(stream, "%s *sbuf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "%s *rbuf = malloc(sizeof(%s) * count * size);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "MPI_Scatter(sbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", ");
  mpi_fprintf(stream, "rbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", %s_RANK(", node->interaction->from->name);
  for (int param=0; param<node->interaction->from->dimen; param++) {
    if (param!=0) mpi_fprintf(stream, ",");
    mpi_fprint_const_or_var(stream, tree, node->interaction->from->param[param]);
  }
  mpi_fprintf(stream, "), MPI_COMM_WORLD);\n");

  INDENT;
  mpi_fprintf(stream, "free(sbuf);\n");
  INDENT;
  mpi_fprintf(stream, "free(rbuf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}

void mpi_fprint_gather(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_SEND);

  INDENT;
  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  // Variable declarations
  mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

  INDENT;
  mpi_fprintf(stream, "%s *sbuf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "%s *rbuf = malloc(sizeof(%s) * count * size);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  if (strlen(node->interaction->msgsig.op)>0) {
    mpi_fprintf(stream, "MPI_Reduce(sbuf, count, "); // Reduce (MPI_Op as Label)
  } else {
    mpi_fprintf(stream, "MPI_Gather(sbuf, count, "); // Gather
  }
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", rbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  if (strlen(node->interaction->msgsig.op)>0) {
    mpi_fprintf(stream, ", %s", node->interaction->msgsig.op); // MPI_Op
  }
  mpi_fprintf(stream, ", %s_RANK(", node->interaction->to[0]->name);
  for (int param=0; param<node->interaction->to[0]->dimen; param++) {
    if (param!=0) mpi_fprintf(stream, ",");
    mpi_fprint_const_or_var(stream, tree, node->interaction->to[0]->param[param]);
  }
  mpi_fprintf(stream, "), MPI_COMM_WORLD);\n");

  INDENT;
  mpi_fprintf(stream, "free(sbuf);\n");
  INDENT;
  mpi_fprintf(stream, "free(rbuf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}



void mpi_fprint_allreduce(FILE *stream, st_tree *tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_ALLREDUCE);

  INDENT;
  mpi_fprintf(stream, "{\n");

  indent++;
  INDENT;

  // Variable declarations
  mpi_fprintf(stream, "int count = 1 /* CHANGE ME */;\n");

  INDENT;
  mpi_fprintf(stream, "%s *sbuf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "%s *rbuf = malloc(sizeof(%s) * count);\n",
      node->interaction->msgsig.payload,
      node->interaction->msgsig.payload);

  INDENT;
  mpi_fprintf(stream, "MPI_Reduce(sbuf, rbuf, count, ");
  mpi_fprint_datatype(stream, node->interaction->msgsig);
  mpi_fprintf(stream, ", %s, MPI_COMM_WORLD);\n", node->interaction->msgsig.op); // MPI_Op

  INDENT;
  mpi_fprintf(stream, "free(sbuf);\n");
  INDENT;
  mpi_fprintf(stream, "free(rbuf);\n");

  indent--;
  INDENT;
  mpi_fprintf(stream, "}\n");
}


void mpi_fprint_root(FILE *stream, st_tree* tree, st_node *node, int indent)
{
  assert(node != NULL && node->type == ST_NODE_ROOT);

  mpi_fprintf(stream, "{\n");

  _mpi_fprint_children(stream, tree, node, indent);

  INDENT;
  mpi_fprintf(stream, "}\n");
}


// TODO I am disgusted by this function. Please don't look at it.
void mpi_fprint(FILE *stream, st_tree *tree)
{
  mpi_fprintf(stream, "#include <stdio.h>\n");
  mpi_fprintf(stream, "#include <stdlib.h>\n");
  mpi_fprintf(stream, "#include <string.h>\n");
  mpi_fprintf(stream, "#include <mpi.h>\n\n");

  //
  // Defining MPI tags / Pabble message labels (msgsig.op) as constants
  // - First walk whole st_tree to find unique labels
  // - Then write a #define for each of them using its index in the list
  //
  labels.count = 0;
  mpi_find_labels(tree->root);
  if (labels.count > 0) {
    for (int label=0; label<labels.count; label++) {
      mpi_fprintf(stream, "#define %s %d\n", labels.strings[label], label);
    }
  }


  //
  // Create macros to convert 1D/2D roles to ranks
  // - Go through roles in increasing order
  // - Role definitions are only defined by numeric constants or declared constants
  //   => numeric constants = use value
  //   => declared constants = find value (if defined)
  //                         | use constant as is (1 constant only, defined in terms of SIZE_VARIABLE)
  // - Finally derive and bind values of constants from SIZE_VARIABLE
  //
  int mpi_nroles = 0;

  for (int role=0; role<tree->info->nrole; role++) {
    if (tree->info->roles[role]->dimen == 0) {
      mpi_fprintf(stream, "#define %s_RANK %d\n\n", tree->info->roles[role]->name, mpi_nroles);
      mpi_nroles++;
    } else {
      assert(tree->info->roles[role]->dimen <= 2 /* Won't work with > 2 dimen for now */);

      mpi_fprintf(stream, "#define %s_RANK(", tree->info->roles[role]->name);
      for (int dimen=0; dimen<tree->info->roles[role]->dimen; dimen++) {
        mpi_fprintf(stream, "%s%c", dimen==0?"":",", 'x'+dimen);
      }

      mpi_fprintf(stream, ") (%d", mpi_nroles); // Starting parenthesis + Rank Offset

      for (int dimen=0; dimen<tree->info->roles[role]->dimen; dimen++) {
        assert(tree->info->roles[role]->param[dimen]->type == ST_EXPR_TYPE_RNG);

        if (dimen > 0) { // Prepend 'row-offset', *(x_max-x_min)
          mpi_fprintf(stream, "*((");
          mpi_fprint_const_or_var(stream, tree, tree->info->roles[role]->param[dimen-1]->rng->to); // x_max
          mpi_fprintf(stream, ")-(");
          mpi_fprint_const_or_var(stream, tree, tree->info->roles[role]->param[dimen-1]->rng->from); // x_min
          mpi_fprintf(stream, "))");
        }

        // For dimen > 2 we need to add (ndimen - current dimen) opening brackets

        mpi_fprintf(stream, "+((%c)-(", 'x'+dimen); // y
        mpi_fprint_const_or_var(stream, tree, tree->info->roles[role]->param[dimen]->rng->from); // y_min
        mpi_fprintf(stream, "))");
      }
      mpi_fprintf(stream, ")"); // Ending parenthesis
    }
  }

  mpi_fprintf(stream, "\n\nint main(int argc, char *argv[])\n");
  mpi_fprintf(stream, "{\n");
  mpi_fprintf(stream, "  int %s, %s; // MPI variables\n", RANK_VARIABLE, SIZE_VARIABLE);
  mpi_fprintf(stream, "  MPI_Group world_grp;\n");
  mpi_fprintf(stream, "  MPI_Init(&argc, &argv);\n");
  mpi_fprintf(stream, "  MPI_Comm_rank(MPI_COMM_WORLD, &%s);\n", RANK_VARIABLE);
  mpi_fprintf(stream, "  MPI_Comm_size(MPI_COMM_WORLD, &%s);\n", SIZE_VARIABLE);

  mpi_fprintf(stream, "\n  /** Begin of protocol %s **/\n\n", tree->info->name);

  if (ST_TYPE_LOCAL == tree->info->type) {

    for (int constant=0; constant<tree->info->nconst; constant++) {
      switch (tree->info->consts[constant]->type) {
        case ST_CONST_VALUE:
          mpi_fprintf(stream, "  const int %s = %u;\n",
              tree->info->consts[constant]->name,
              tree->info->consts[constant]->value);
          break;
        case ST_CONST_BOUND:
          mpi_fprintf(stream, "  const int %s = %u/* CHANGEME(%u..%u) */;\n",
              tree->info->consts[constant]->bounds.lbound,
              tree->info->consts[constant]->bounds.lbound,
              tree->info->consts[constant]->bounds.ubound);
          break;
        default:
          fprintf(stderr, "Undefined constant type consts[%d] (%d)\n",
              constant,
              tree->info->consts[constant]->type);
      }
    }

    // Group declaration
    mpi_fprintf(stream, "  MPI_Comm_group(MPI_COMM_WORLD, &world_grp);\n");
    for (int group=0; group<tree->info->ngroup; group++) {
      mpi_fprintf(stream, "  MPI_Group %s;\n", tree->info->groups[group]->name);
      mpi_fprintf(stream, "  MPI_Comm %s_comm;\n", tree->info->groups[group]->name);
      // Define group members
      mpi_fprintf(stream, "  int *grp_%s = NULL;\n");
      mpi_fprintf(stream, "  int ngrp_%s = 0;\n", tree->info->groups[group]->name);
      for (int role=0; role<tree->info->groups[group]->nmemb; role++) {
        for (int param=0; param<tree->info->groups[group]->membs[role]->dimen; param++) {
          if (tree->info->groups[group]->membs[role]->param[param]->type == ST_EXPR_TYPE_RNG) {
            mpi_fprintf(stream, "  for (int param%d=",param);
            mpi_fprint_const_or_var(stream, tree, tree->info->groups[group]->membs[role]->param[param]->rng->from);
            mpi_fprintf(stream, "; param%d<", param);
            mpi_fprint_const_or_var(stream, tree, tree->info->groups[group]->membs[role]->param[param]->rng->to);
            mpi_fprintf(stream, "; param%d++) {\n", param);
          } else if (tree->info->groups[group]->membs[role]->param[param]->type == ST_EXPR_TYPE_VAR) {
            mpi_fprintf(stream, "%s", tree->info->groups[group]->membs[role]->param[param]->var);
          } else if (tree->info->groups[group]->membs[role]->param[param]->type == ST_EXPR_TYPE_CONST) {
            mpi_fprintf(stream, "%d", tree->info->groups[group]->membs[role]->param[param]->num);
          }
        }
        // The actual line
        mpi_fprintf(stream, "    grp_%s = realloc(grp_%s, sizeof(int)*(ngrp_%s+1));\n",
            tree->info->groups[group]->name,
            tree->info->groups[group]->name,
            tree->info->groups[group]->name);
        mpi_fprintf(stream, "    grp_%s[ngrp_%s] = %s_RANK(", tree->info->groups[group]->name, tree->info->groups[group]->name, tree->info->groups[group]->name);
        for (int param=0; param<tree->info->groups[group]->membs[role]->dimen; param++) {
          if (param!=0) mpi_fprintf(stream, ",");
          mpi_fprintf(stream, "param%d", param);
        }
        mpi_fprintf(stream, "); ngrp_%s++;\n", tree->info->groups[group]->name);

        for (int param=0; param<tree->info->groups[group]->membs[role]->dimen; param++) {
          if (tree->info->groups[group]->membs[role]->param[param]->type == ST_EXPR_TYPE_RNG) {
            mpi_fprintf(stream, " }\n", param);
          }
        }
        /*
        if (tree->info->groups[group]->membs[role]->param[0]->type == ST_EXPR_TYPE_CONST) {
          mpi_fprintf(stream, "%d", tree->info->groups[group]->membs[role]->param[0]->num);
        } else if (tree->info->groups[group]->membs[role]->param[0]->type == ST_EXPR_TYPE_RNG) {
          assert(tree->info->groups[group]->membs[role]->param[0]->rng->from->type == ST_EXPR_TYPE_CONST);
          assert(tree->info->groups[group]->membs[role]->param[0]->rng->to->type == ST_EXPR_TYPE_CONST);
          for (int index=tree->info->groups[group]->membs[role]->param[0]->rng->from->num;
                   index<tree->info->groups[group]->membs[role]->param[0]->rng->to->num;
                   index++) {
            mpi_fprintf(stream, "%d", index);
          }
        } else {
          assert(0); //Group defined with role indices other than constant and range
        }
        */
      }
      mpi_fprintf(stream, "  };\n");
      mpi_fprintf(stream, "  MPI_Group_incl(world_grp, %d, &%s);\n",
          tree->info->groups[group]->nmemb,
          tree->info->groups[group]->name);
      mpi_fprintf(stream, "  MPI_Comm_create(MPI_COMM_WORLD, %s, %s_comm);\n",
          tree->info->groups[group]->name,
          tree->info->groups[group]->name);
    }

  } else { // Not Local protocol

    fprintf(stderr, "Protocol type %d not supported for MPI generation\n", tree->info->type);

  }

  //
  // Generate body of protocol
  //
  int indent = 1;
  INDENT;
  mpi_fprint_node(stream, tree, tree->root, indent);

  mpi_fprintf(stream, "\n  /** End of protocol %s **/\n\n", tree->info->name);

  mpi_fprintf(stream, "  MPI_Finalize();\n");
  // Cleanup group construction
  for (int group=0; group<tree->info->ngroup; group++) {
    mpi_fprintf(stream, "  free(%s);\n", tree->info->groups[group]->name);
  }
  mpi_fprintf(stream, "  return EXIT_SUCCESS;\n");
  mpi_fprintf(stream, "}\n");
}

void mpi_print(FILE *stream, st_tree *local_tree)
{
  mpi_fprint(stream, local_tree);
}
