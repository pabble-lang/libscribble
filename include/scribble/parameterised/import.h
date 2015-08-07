#ifndef SCRIBBLE__PARAMETERISED__IMPORT_H__
#define SCRIBBLE__PARAMETERISED__IMPORT_H__

#include "scribble/import.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
using scribble::Import;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Import pbl_import;
#else
typedef struct Import pbl_import;
#endif

pbl_import *pbl_mk_import(char *name);

pbl_import *pbl_mk_import_full(char *name, char *from, char *as);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__IMPORT_H__
