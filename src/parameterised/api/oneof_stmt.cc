#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/oneof_stmt.h>
#include <scribble/parameterised/ast/expr.h>
#include <scribble/parameterised/ast/rng_expr.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_oneofstmt(pbl_role *selector, unsigned int dimen)
{
    return new ast::OneofStmt(selector, dimen);
}

pbl_stmt *pbl_oneofstmt_set_var(pbl_stmt *stmt, char *var)
{
    if (ast::OneofStmt *oneof = dynamic_cast<ast::OneofStmt *>(stmt)) {
        oneof->set_var(var);
    }
    return stmt;
}

pbl_stmt *pbl_oneofstmt_set_range(pbl_stmt *stmt, pbl_expr *range_expr)
{
    if (ast::OneofStmt *oneof = dynamic_cast<ast::OneofStmt *>(stmt)) {
        if (ast::RngExpr *expr = dynamic_cast<ast::RngExpr *>(range_expr)) {
            oneof->set_range(expr);
        }
    }
    return stmt;
}

pbl_stmt *pbl_oneofstmt_set_repeat(pbl_stmt *stmt, bool repeat)
{
    if (ast::OneofStmt *oneof = dynamic_cast<ast::OneofStmt *>(stmt)) {
        oneof->set_repeat(repeat);
    }
    return stmt;
}

} // namespace parameterised
} // namespace scribble
