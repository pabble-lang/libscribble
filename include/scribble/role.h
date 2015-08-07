#ifndef SCRIBBLE__ROLE_H__
#define SCRIBBLE__ROLE_H__

#include <sesstype/role.h>

#ifdef __cplusplus
namespace scribble {
#endif

#ifdef __cplusplus
using sesstype::Role;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Role sbl_role;
#else
typedef struct Role sbl_role;
#endif

sbl_role *sbl_mk_role(char *name);
void sbl_role_free(sbl_role *role);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__ROLE_H__
