#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/expr.h>
#include <scribble/util/srclocation.h>

namespace scribble {
namespace parameterised {

pbl_role *pbl_mk_role_init()
{
    return new scribble::parameterised::Role();
}

pbl_role *pbl_mk_role(char *name)
{
    return new scribble::parameterised::Role(name);
}

pbl_role *pbl_role_set_name(pbl_role *role, char *name)
{
    role->set_name(name);
    return role;
}

pbl_role *pbl_role_add_param(pbl_role *role, pbl_expr *param)
{
    role->add_param(param);
    return role;
}

pbl_expr *pbl_role_get_param(pbl_role *role, unsigned int dimen)
{
    return dynamic_cast<scribble::parameterised::ast::ArithExpr *>((*role)[dimen]);
}

void pbl_role_free(pbl_role *role)
{
    delete role;
}

} // namespace parameterised
} // namespace scribble
