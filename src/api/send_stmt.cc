#include <scribble/msg.h>
#include <scribble/role.h>
#include <scribble/ast/stmt.h>
#include <scribble/ast/send_stmt.h>

namespace scribble {

sbl_stmt *sbl_mk_sendstmt_init()
{
    return new ast::SendStmt();
}

sbl_stmt *sbl_mk_sendstmt(sbl_msg *msg)
{
    return new ast::SendStmt(msg);
}

sbl_stmt *sbl_sendstmt_add_to(sbl_stmt *stmt, sbl_role *to)
{
    if (ast::SendStmt *sendstmt = dynamic_cast<ast::SendStmt *>(stmt)) {
        sendstmt->add_rcvr(to);
    }
    return stmt;
}

} // namespace scribble
