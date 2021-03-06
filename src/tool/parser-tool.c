/**
 * \file parser-tool.c
 * \brief Standalone Scribble parser.
 */
#include <stdio.h>
#include <stdlib.h>

#include <sesstype/session.h>
#include <sesstype/node.h>

extern int yyparse(st_tree *tree);
extern FILE *yyin;


int main(int argc, char *argv[])
{
  if (argc > 1) {
    yyin = fopen(argv[1], "r");
  } else {
    fprintf(stderr, "Warning: reading from stdin\n");
  }

  st_tree *tree = st_tree_mk_init("default");
  yyparse(tree);
  st_tree_free(tree);

  if (argc > 1)
    fclose(yyin);
  return EXIT_SUCCESS;
}
