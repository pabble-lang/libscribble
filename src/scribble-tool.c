#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "st_node.h"
#include "st_normalise.h"

#include "check.h"
#include "print.h"
#include "project.h"
#include "mpi_print.h"

extern int yyparse(st_tree *tree);
extern FILE *yyin;


int main(int argc, char *argv[])
{
  int option;
  int check = 0;
  int parse = 0;
  int show_usage = 0;
  int show_version = 0;
  int verbosity_level = 0;
  char *output_file   = NULL;
  char *project_role  = NULL;
  char *scribble_file = NULL;
  FILE *mpi_handle    = NULL;

  while (1) {
    static struct option long_options[] = {
      {"project", required_argument, 0, 'p'},
      {"output",  required_argument, 0, 'o'},
      {"colour",  no_argument,       0,  0 },
      {"mpi",     required_argument, 0, 'm'},
      {"parse",   no_argument,       0, 's'},
      {"check",   no_argument,       0, 'c'},
      {"version", no_argument,       0, 'v'},
      {"verbose", no_argument,       0, 'V'},
      {"help",    no_argument,       0, 'h'},
      {0, 0, 0, 0}
    };

    int option_idx = 0;
    option = getopt_long(argc, argv, "p:o:scvVh", long_options, &option_idx);

    if (option == -1) break;

    switch (option) {
      case 0:
        if (0 == strcmp(long_options[option_idx].name, "colour")) {
          scribble_colour_mode(1);
        }
        break;
      case 'm':
        if (strcmp(optarg, "--") == 0) {
          mpi_handle = stdout;
        } else {
          mpi_handle = fopen(optarg, "a");
        }
        break;
      case 'p':
        project_role = (char *)calloc(sizeof(char), strlen(optarg)+1);
        strcpy(project_role, optarg);
        break;
      case 'o':
        output_file = (char *)calloc(sizeof(char), strlen(optarg)+1);
        strcpy(output_file, optarg);
        break;
      case 's':
        parse = 1;
        break;
      case 'c':
        check = 1;
        break;
      case 'v':
        show_version = 1;
        break;
      case 'V':
        verbosity_level++;
        break;
      case 'h':
        show_usage |= 1;
        break;
    }
  }

  argc -= optind-1;
  argv[optind-1] = argv[0];
  argv += optind-1;

  if (show_usage) {
    fprintf(stderr, "Usage: %s [--parse] [--project role] [--check] [-v] [-h] Scribble.spr\n", argv[0]);
    return EXIT_SUCCESS;
  }

  if (argc < 2) {
    show_usage |= 1;
  }

  if (show_version) {
    fprintf(stderr, "scribble-tool 1.1.0~scribble0.2+pabble\n");
    return EXIT_SUCCESS;
  }

  scribble_file = argv[1];
  yyin = fopen(scribble_file, "r");
  if (yyin == NULL) {
    perror(scribble_file);
    return EXIT_FAILURE;
  }

  st_tree *tree = st_tree_init((st_tree *)malloc(sizeof(st_tree)));
  if (0 != yyparse(tree)) {
    fprintf(stderr, "Error: Parse failed\n");
    return EXIT_FAILURE;
  }

  if (parse) {
    if (verbosity_level > 0) fprintf(stderr, "Parsed %s\n", scribble_file);
    if (verbosity_level > 1) st_tree_print(tree);
    st_node_normalise(tree->root);
    scribble_print(tree);
  }

  if (check) {
    if (verbosity_level > 0) fprintf(stderr, "Well-formedness check\n");
    if (scribble_check(tree) != 0) {
      fprintf(stderr, "Well-formedness checks failed! (see error messages above)\n");
      if (verbosity_level > 1) st_tree_print(tree);
      scribble_print(tree);
    }
    st_node_reset_markedflag(tree->root);
    assert(0 /* Well-formedness checks unimplemented */);
  }

  if (project_role != NULL) {
    if (verbosity_level > 0) fprintf(stderr, "Projection for %s\n", project_role);
    st_tree *local_tree = scribble_project(tree, project_role);
    if (verbosity_level > 1) st_tree_print(local_tree);
    if (local_tree->root != 0) {
      st_node_normalise(local_tree->root);
      if (verbosity_level > 2) st_tree_print(local_tree);
      scribble_print(local_tree);
      if (mpi_handle != NULL) {
        if (verbosity_level > 0) fprintf(stderr, "Writing MPI\n");
        mpi_print(mpi_handle, local_tree);
        if (mpi_handle != stdout) {
          fclose(mpi_handle);
        }
      }
    } else {
      fprintf(stderr, "ERROR: Cannot project for %s.\n", project_role);
    }
  }


  st_tree_free(tree);

  return EXIT_SUCCESS;
}
