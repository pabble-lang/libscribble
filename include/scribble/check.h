#ifndef SCRIBBLE__CHECK__H__
#define SCRIBBLE__CHECK__H__
/**
 * \file
 * Hedaer file for Scribble validatation checks.
 *
 * \headerfile "st_node.h"
 */

#include <sesstype/st_node.h>

#define ST_ROLE_ALL "__All"

/**
 * Checks whether roles are defined.
 *
 * If role is non-parameterised, check if role name is defined;
 * If role is parameterised, check if role name/parameter dimension is defined.
 * This check only work with global protocols, and do not check for indices validity.
 */
int scribble_check_defined_roles(st_tree *tree);


/**
 * Checks whether all indices are bound.
 *
 * This check only work with global protocols and is applicable only to parameterised
 * message-passing, making sure they are bound by previously defined indices
 * (foreach or from-role).
 */
int scribble_check_bound_indices(st_tree *tree);


/**
 * Checks whether constants satisfy well-formedness conditions.
 */
int scribble_check_constants(st_tree *tree);


/**
 * Run all checks.
 */
int scribble_check(st_tree *tree);

#endif // SCRIBBLE__CHECK__H__
