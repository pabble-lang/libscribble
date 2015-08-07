#include <iostream>
#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/interruptible_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_interruptiblestmt_init()
{
    return new ast::InterruptibleStmt();
}

pbl_stmt *pbl_mk_interruptiblestmt(char *scope_name)
{
    return new ast::InterruptibleStmt(scope_name);
}

pbl_stmt *pbl_interruptiblestmt_set_scope(pbl_stmt *stmt, char *scope)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->set_scope(scope);
    }
    return stmt;
}

pbl_stmt *pbl_interruptiblestmt_add_interrupt(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_interrupt(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

pbl_stmt *pbl_interruptiblestmt_add_catch(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_catch(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

pbl_stmt *pbl_interruptiblestmt_add_throw(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg)
{
    if (ast::InterruptibleStmt *istmt = dynamic_cast<ast::InterruptibleStmt *>(stmt)) {
        istmt->add_throw(role, msg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a interruptiblestmt\n";
    }
    return stmt;
}

} // namespace parameterised
} // namespace scribble
