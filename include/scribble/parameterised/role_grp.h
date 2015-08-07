#ifndef SCRIBBLE__PARAMETERISED__ROLE_GRP_H__
#define SCRIBBLE__PARAMETERISED__ROLE_GRP_H__

#include <sesstype/parameterised/role.h>
#include <sesstype/parameterised/role_grp.h>

#include "scribble/parameterised/role.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
using sesstype::parameterised::RoleGrp;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef RoleGrp pbl_role_grp;
#else
typedef struct RoleGrp pbl_role_grp;
#endif

pbl_role_grp *pbl_mk_role_grp_init();

pbl_role_grp *pbl_mk_role_grp(char *name);

pbl_role_grp *pbl_role_grp_set_name(pbl_role_grp *grp, char *name);

pbl_role_grp *pbl_role_grp_add_role(pbl_role_grp *grp, pbl_role *role);

void pbl_role_grp_free(pbl_role_grp *grp);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__ROLE_GRP_H__
