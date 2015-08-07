#include <scribble/msg.h>
#include <scribble/role.h>
#include <scribble/ast/stmt.h>
#include <scribble/ast/sendrecv_stmt.h>

namespace scribble {

sbl_stmt *sbl_mk_sendrecvstmt_init()
{
    return new ast::SendRecvStmt();
}

sbl_stmt *sbl_mk_sendrecvstmt(sbl_msg *msg)
{
    return new ast::SendRecvStmt(msg);
}

sbl_stmt *sbl_sendrecvstmt_add_to(sbl_stmt *stmt, sbl_role *to)
{
    if (ast::SendRecvStmt *srstmt = dynamic_cast<ast::SendRecvStmt *>(stmt)) {
        srstmt->add_rcvr(to);
    }
    return stmt;
}

sbl_stmt *sbl_sendrecvstmt_set_from(sbl_stmt *stmt, sbl_role *from)
{
    if (ast::SendRecvStmt *srstmt = dynamic_cast<ast::SendRecvStmt *>(stmt)) {
        srstmt->set_sndr(from);
    }
    return stmt;
}

} // namespace scribble
