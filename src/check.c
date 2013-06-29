/**
 * \file
 * This file contains a series of validation checks for a given
 * Scribble protocol.
 *
 * Each individual check returns an int
 *   0        if check successful
 *   non-zero if check failed
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/check.h"
 */

#include <stdio.h>

#include <sesstype/st_node.h>
#include "scribble/check.h"


int scribble_check(st_tree *tree)
{
  if (scribble_check_defined_roles(tree)) {
    fprintf(stderr, "Error: Not all roles referenced are defined\n");
    return 1;
  }

//  if (scribble_check_constants(tree)) {
//    fprintf(stderr, "Error: Constants well-formedness conditions not satisfied\n");
//    error |= 1;
//  }

  if (scribble_check_bound_indices(tree)) {
    fprintf(stderr, "Error: Index binding well-formedness conditions not satisfied\n");
    return 1;
  }

  return 0;
}
