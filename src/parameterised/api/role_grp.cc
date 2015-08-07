#include <string>
#include <scribble/parameterised/role_grp.h>

namespace scribble {
namespace parameterised {

pbl_role_grp *pbl_mk_role_grp_init()
{
    return new RoleGrp();
}

pbl_role_grp *pbl_mk_role_grp(char *name)
{
    return new RoleGrp(name);
}

pbl_role_grp *pbl_role_grp_set_name(pbl_role_grp *grp, char *name)
{
    grp->set_name(name);
    return grp;
}

pbl_role_grp *pbl_role_grp_add_role(pbl_role_grp *grp, pbl_role *role)
{
    grp->add_member(role);
    return grp;
}

void pbl_role_grp_free(pbl_role_grp *grp)
{
    delete grp;
}

} // namespace parameterised
} // namespace scribble
