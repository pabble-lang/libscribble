/**
 * \file
 * This file contains a pretty printer of (multiparty) session
 * in form of a Scribble protocol.
 *
 * \headerfile <sesstype/st_node.h>
 * \headerfile "scribble/print_utils.h"
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#include "scribble/print_utils.h"

int fprint_keyword(FILE *stream, char *token)
{
  // Yellow
  return fprintf(stream, "\033[33m%s\033[0m ", token);
}

int fprint_keyword2(FILE *stream, char *token)
{
  // Green
  return fprintf(stream, "\033[32m%s\033[0m ", token);
}

int fprintf_info(FILE *stream, char *format, ...)
{
  va_list args;
  int res;
  char *_format = malloc(strlen("INFO/")+strlen(format)+strlen("\033[1;32m%s\033[0m")+1);
  strcpy(_format, "\033[1;32m");
  strcat(_format, "INFO/");
  strcat(_format, "\033[0m");
  strcat(_format, format);
  va_start(args, format);
  res = vfprintf(stream, _format, args);
  va_end(args);
  free(_format);

  return res;
}

int fprintf_error(FILE *stream, char *format, ...)
{
  va_list args;
  int res;
  char *_format = malloc(strlen("ERROR/")+strlen(format)+strlen("\033[1;31m%s\033[0m")+1);
  strcpy(_format, "\033[1;31m");
  strcat(_format, "ERROR/");
  strcat(_format, "\033[0m");
  strcat(_format, format);
  va_start(args, format);
  res = vfprintf(stream, _format, args);
  va_end(args);
  free(_format);

  return res;
}
