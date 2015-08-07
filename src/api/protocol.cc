#include <scribble/protocol.h>
#include <scribble/role.h>
#include <scribble/ast/stmt.h>

namespace scribble {

sbl_protocol *sbl_mk_protocol(char *name)
{
    return new Protocol(name);
}

const char *sbl_protocol_get_name(sbl_protocol *protocol)
{
    return protocol->name().c_str();
}

sbl_protocol *sbl_protocol_set_endpoint(sbl_protocol *protocol, sbl_role *role)
{
    protocol->add_role(role);
    return protocol;
}

sbl_protocol *sbl_protocol_set_body(sbl_protocol *protocol, sbl_stmt *body)
{
    if (ast::ScribbleStmt *stmt = dynamic_cast<ast::ScribbleStmt *>(body)) {
        protocol->set_root(stmt);
    }
    return protocol;
}

sbl_protocol *sbl_protocol_add_role(sbl_protocol *protocol, sbl_role *role)
{
    protocol->add_role(role);
    return protocol;
}

void sbl_protocol_free(sbl_protocol *protocol)
{
    delete protocol;
}

} // namespace scribble
