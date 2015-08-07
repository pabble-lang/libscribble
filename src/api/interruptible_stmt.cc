#include <iostream>
#include <scribble/msg.h>
#include <scribble/ast/stmt.h>
#include <scribble/ast/interruptible_stmt.h>

namespace scribble {

sbl_stmt *sbl_mk_interruptiblestmt_init()
{
    return new ast::InterruptibleStmt();
}

sbl_stmt *sbl_mk_interruptiblestmt(char *scope_name)
{
    return new ast::InterruptibleStmt(scope_name);
}

sbl_stmt *pbl_interruptiblestmt_set_scope(sbl_stmt *stmt, char *scope)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->set_scope(scope);
    }
    return stmt;
}

sbl_stmt *sbl_interruptiblestmt_add_interrupt(sbl_stmt *stmt, sbl_role *role, sbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_interrupt(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

sbl_stmt *sbl_interruptiblestmt_add_catch(sbl_stmt *stmt, sbl_role *role, sbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_catch(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

sbl_stmt *sbl_interruptiblestmt_add_throw(sbl_stmt *stmt, sbl_role *role, sbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_throw(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

} // namespace scribble
