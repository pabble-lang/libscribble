#ifndef SCRIBBLE__PRINT_UTILS___H__
#define SCRIBBLE__PRINT_UTILS___H__
/**
 * \file
 * Header file for Scribble pretty printer (utils).
 *
 * \headerfile <sesstype/st_node.h>
 */
#include <stdio.h>
#include <sesstype/st_node.h>

#ifdef __cplusplus
extern "C" {
#endif

int fprint_keyword(FILE *stream, char *token);
int fprint_keyword2(FILE *stream, char *token);
int fprintf_info(FILE *stream, char *format, ...);
int fprintf_error(FILE *stream, char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // SCRIBBLE__PRINT_UTILS___H__
