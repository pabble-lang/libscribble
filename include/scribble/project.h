#ifndef SCRIBBLE__PROJECT__H__
#define SCRIBBLE__PROJECT__H__
/**
 * \file
 * This file contains the implmentation of projection algorithm
 * from global Scribble into endpoint Scribble.
 *
 */

#include <sesstype/st_node.h>
#include <sesstype/st_expr.h>

#define ST_ROLE_ALL "__All"

#ifdef __cplusplus
extern "C" {
#endif

st_node *scribble_project_root(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_message(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_choice(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_parallel(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_recur(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_continue(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_foreach(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_allreduce(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);
st_node *scribble_project_node(st_tree *tree, st_node *node, char *projectrole, st_expr_list *env);

/**
 * \brief Project a global st_tree to endpoint st_tree.
 * 
 * @param[in] global Global st_tree.
 * @param[in] role   Role to project against.
 *
 * \returns Projected st_tree.
 */
st_tree *scribble_project(st_tree *global, char *role);

#ifdef __cplusplus
}
#endif

#endif // SCRIBBLE__PROJECT__H__
