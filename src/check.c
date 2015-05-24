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
#include <stdbool.h>

#include <sesstype/st_node.h>
#include "scribble/check.h"
#include "scribble/print_utils.h"


bool scribble_check(st_tree *tree)
{
  if (!scribble_check_constants(tree)) {
    fprintf_error(stderr, "Error: Constants well-formedness conditions not satisfied\n");
    return false;
  }

  if (!pabble_check_indices(tree)) {
    fprintf_error(stderr, "Error: Index binding well-formedness conditions not satisfied\n");
    return false;
  }

  return true;
}
