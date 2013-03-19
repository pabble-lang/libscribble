#ifndef MPI_PRINT__H__
#define MPI_PRINT__H__

#include <stdio.h>
#include "st_node.h"

#ifdef __cplusplus
extern "C" {
#endif

// Toplevel

void mpi_print(FILE *stream, st_tree *local_tree);
void mpi_fprintf(FILE *stream, const char *format, ...);

// Helpers

void mpi_fprint_msg_cond(FILE *stream, st_tree *tree, const msg_cond_t *msg_cond, int indent);
void mpi_fprint_datatype(FILE *stream, st_node_msgsig_t msgsig);
void mpi_fprint_expr(FILE *stream, st_expr *expr);
void mpi_fprint_rank(FILE *stream, st_expr *param, const char *replace, const char *with);
void mpi_fprint_role(FILE *stream, st_role *role);

void mpi_fprint_node(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_root(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_send(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_recv(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_choice(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_recur(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_continue(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_parallel(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_for(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_allreduce(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_allgather(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_scatter(FILE *stream, st_tree *tree, st_node *node, int indent);
void mpi_fprint_gather(FILE *stream, st_tree *tree, st_node *node, int indent);

#ifdef __cplusplus
}
#endif

#endif // MPI__PRINT__H__
