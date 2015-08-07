#include <scribble/role.h>

namespace scribble {

sbl_role *sbl_mk_role(char *name)
{
    return new Role(name);
}

void sbl_role_free(sbl_role *role)
{
    delete role;
}

} // namespace scribble
