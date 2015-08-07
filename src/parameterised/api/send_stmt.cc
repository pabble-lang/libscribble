#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/send_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_sendstmt_init()
{
    return new ast::SendStmt();
}

pbl_stmt *pbl_mk_sendstmt(pbl_msg *msg)
{
    return new ast::SendStmt(msg);
}

pbl_stmt *pbl_sendstmt_add_to(pbl_stmt *stmt, pbl_role *to)
{
    if (ast::SendStmt *sendstmt = dynamic_cast<ast::SendStmt *>(stmt)) {
        sendstmt->add_rcvr(to);
    }
    return stmt;
}

pbl_stmt *pbl_sendstmt_set_cond(pbl_stmt *stmt, pbl_cond *cond)
{
    if (ast::SendStmt *sendstmt = dynamic_cast<ast::SendStmt *>(stmt)) {
        sendstmt->set_cond(cond);
    }
    return stmt;
}

} // namespace parameterised
} // namespace scribble
