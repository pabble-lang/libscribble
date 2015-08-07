#include <scribble/parameterised/ast/expr.h>
#include <scribble/parameterised/ast/rng_expr.h>
#include <scribble/parameterised/ast/for_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_forstmt(pbl_expr *bindexpr, pbl_stmt *body)
{
    auto *rng_expr = static_cast<ast::RngExpr *>(bindexpr);
    return new ast::ForStmt(rng_expr);
}

pbl_stmt *pbl_mk_forstmt_except(pbl_expr *bindexpr, pbl_stmt *body, pbl_expr *except)
{
    auto *rng_expr = dynamic_cast<ast::RngExpr *>(bindexpr);
    ast::ForStmt *forstmt = new ast::ForStmt(rng_expr);
    forstmt->set_except(except);
    return forstmt;
}

} // namespace parameterised
} // namespace scribble
