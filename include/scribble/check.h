#ifndef SCRIBBLE__CHECK__H__
#define SCRIBBLE__CHECK__H__
/**
 * \file
 * Header file for Scribble validation checks.
 *
 * \headerfile "st_node.h"
 */

#include <stdbool.h>

#include <sesstype/st_node.h>

#define ST_ROLE_ALL "__All"

/**
 * Run all well-formedness checks.
 */
bool scribble_check(st_tree *tree);

/**
 * Checks whether constants satisfy well-formedness conditions.
 */
bool scribble_check_constants(st_tree *tree);

/**
 * Run all checks.
 */
bool pabble_check_indices(st_tree *tree);

#endif // SCRIBBLE__CHECK__H__
