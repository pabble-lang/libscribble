#include <scribble/msg.h>
#include <scribble/role.h>
#include <scribble/ast/stmt.h>
#include <scribble/ast/recv_stmt.h>

namespace scribble {


sbl_stmt *sbl_mk_recvstmt_init()
{
    return new ast::RecvStmt();
}

sbl_stmt *sbl_mk_recvstmt(sbl_msg *msg)
{
    return new ast::RecvStmt(msg);
}

sbl_stmt *sbl_recvstmt_set_from(sbl_stmt *stmt, sbl_role *from)
{
    if (ast::RecvStmt *recvstmt = dynamic_cast<ast::RecvStmt *>(stmt)) {
        recvstmt->set_sndr(from);
    }
    return stmt;
}

} // namespace scribble
