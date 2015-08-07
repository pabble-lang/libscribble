#ifndef SCRIBBLE__PARAMETERISED__EXT_TYPE_H__
#define SCRIBBLE__PARAMETERISED__EXT_TYPE_H__

#include "scribble/ext_type.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
using scribble::ExternalType;
#endif // __cplusplus

#ifdef __cplusplus
typedef ExternalType pbl_exttype;
#else
typedef struct ExternalType pbl_exttype;
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_exttype *pbl_mk_exttype_sig(
        char *name,
        char *ext_src,
        char *ext_name,
        char *ext_schema);

pbl_exttype *pbl_mk_exttype_type(
        char *name,
        char *ext_src,
        char *ext_name,
        char *ext_schema);

unsigned int pbl_exttype_get_type(pbl_exttype *exttype);

const char *pbl_exttype_get_name(pbl_exttype *exttype);

const char *pbl_exttype_get_ext_source(pbl_exttype *exttype);

const char *pbl_exttype_get_ext_name(pbl_exttype *exttype);

const char *pbl_exttype_get_ext_schema(pbl_exttype *exttype);

void pbl_exttype_free(pbl_exttype *exttype);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__EXT_TYPE_H__
