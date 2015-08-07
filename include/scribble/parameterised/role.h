#ifndef SCRIBBLE__PARAMETERISED__ROLE_H__
#define SCRIBBLE__PARAMETERISED__ROLE_H__

#include <sesstype/parameterised/role.h>

#include "scribble/util/srclocation.h"
#include "scribble/parameterised/ast/expr.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
using sesstype::parameterised::Role;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Role pbl_role;
#else
typedef struct Role pbl_role;
#endif

pbl_role *pbl_mk_role_init();
pbl_role *pbl_mk_role(char *name);
pbl_role *pbl_role_set_name(pbl_role *role, char *name);
pbl_role *pbl_role_add_param(pbl_role *role, pbl_expr *param);
pbl_expr *pbl_role_get_param(pbl_role *role, unsigned int dimen);
void pbl_role_free(pbl_role *role);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__ROLE_H__
