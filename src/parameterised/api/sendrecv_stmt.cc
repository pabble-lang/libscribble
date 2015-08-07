#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/sendrecv_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_sendrecvstmt_init()
{
    return new ast::SendRecvStmt();
}

pbl_stmt *pbl_mk_sendrecvstmt(pbl_msg *msg)
{

    return new ast::SendRecvStmt(msg);
}

pbl_stmt *pbl_sendrecvstmt_set_from(pbl_stmt *stmt, pbl_role *from)
{
    if (ast::SendRecvStmt *srstmt = dynamic_cast<ast::SendRecvStmt *>(stmt)) {
        srstmt->set_sndr(from);
    }
    return stmt;
}

pbl_stmt *pbl_sendrecvstmt_add_to(pbl_stmt *stmt, pbl_role *to)
{
    if (ast::SendRecvStmt *srstmt = dynamic_cast<ast::SendRecvStmt *>(stmt)) {
        srstmt->add_rcvr(to);
    }
    return stmt;
}

} // namespace parameterised
} // namespace scribble
