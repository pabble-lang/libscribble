#include <scribble/parameterised/role.h>
#include <scribble/parameterised/protocol.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/util/srclocation.h>

namespace scribble {
namespace parameterised {

pbl_protocol *pbl_mk_protocol(char *name)
{
    return new scribble::parameterised::Protocol(name);
}

const char *pbl_protocol_get_name(pbl_protocol *protocol)
{
    return protocol->name().c_str();
}

pbl_protocol *pbl_protocol_set_endpoint(pbl_protocol *protocol, pbl_role *role)
{
    protocol->set_endpoint(role);
    return protocol;
}

pbl_protocol *pbl_protocol_add_role(pbl_protocol *protocol, pbl_role *role)
{
    protocol->add_role(role);
    return protocol;
}

pbl_protocol *pbl_protocol_add_role_grp(pbl_protocol *protocol, pbl_role_grp *role_grp)
{
    protocol->add_group(role_grp);
    return protocol;
}

pbl_protocol *pbl_protocol_set_body(pbl_protocol *protocol, pbl_stmt *body)
{
    protocol->set_root(static_cast<ast::ScribbleStmt *>(body));
    return protocol;
}

void pbl_protocol_free(pbl_protocol *protocol)
{
    delete protocol;
}

} // namespace parameterised
} // namespace scribble
