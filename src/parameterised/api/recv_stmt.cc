#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/recv_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_recvstmt_init()
{
    return new ast::RecvStmt();
}

pbl_stmt *pbl_mk_recvstmt(pbl_msg *msg)
{
    return new ast::RecvStmt(msg);
}

pbl_stmt *pbl_recvstmt_set_from(pbl_stmt *stmt, pbl_role *from)
{
    if (ast::RecvStmt *recvstmt = dynamic_cast<ast::RecvStmt *>(stmt)) {
        recvstmt->set_sndr(from);
    }
    return stmt;
}

pbl_stmt *pbl_recvstmt_set_cond(pbl_stmt *stmt, pbl_cond *cond)
{
    if (ast::RecvStmt *recvstmt = dynamic_cast<ast::RecvStmt *>(stmt)) {
        recvstmt->set_cond(cond);
    }
    return stmt;
}

} // namespace parameterised
} // namespace scribble
