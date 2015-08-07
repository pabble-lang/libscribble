#include <stdarg.h>
#include <scribble/parameterised/ast/expr.h>
#include <scribble/parameterised/ast/val_expr.h>
#include <scribble/parameterised/ast/var_expr.h>
#include <scribble/parameterised/ast/add_expr.h>
#include <scribble/parameterised/ast/sub_expr.h>
#include <scribble/parameterised/ast/mul_expr.h>
#include <scribble/parameterised/ast/div_expr.h>
#include <scribble/parameterised/ast/mod_expr.h>
#include <scribble/parameterised/ast/shl_expr.h>
#include <scribble/parameterised/ast/shr_expr.h>
#include <scribble/parameterised/ast/seq_expr.h>
#include <scribble/parameterised/ast/rng_expr.h>

namespace scribble {
namespace parameterised {

pbl_expr *pbl_mk_val_expr(unsigned int val)
{
    return new ast::ValExpr(val);
}

pbl_expr *pbl_mk_var_expr(char *var)
{
    return new ast::VarExpr(var);
}

pbl_expr *pbl_mk_add_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::AddExpr(lhs, rhs);
}

pbl_expr *pbl_mk_sub_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::SubExpr(lhs, rhs);
}

pbl_expr *pbl_mk_mul_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::MulExpr(lhs, rhs);
}

pbl_expr *pbl_mk_div_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::DivExpr(lhs, rhs);
}

pbl_expr *pbl_mk_mod_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::ModExpr(lhs, rhs);
}

pbl_expr *pbl_mk_shl_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::ShlExpr(lhs, rhs);
}

pbl_expr *pbl_mk_shr_expr(pbl_expr *lhs, pbl_expr *rhs)
{
    return new ast::ShrExpr(lhs, rhs);
}

pbl_expr *pbl_mk_seq_expr(unsigned int count, ...)
{
    ast::SeqExpr *seqexpr = new ast::SeqExpr();
    int value;
    va_list int_seq;
    va_start(int_seq, count);
    for (int i=0; i<count; i++) {
        value = va_arg(int_seq, int);
    }
    va_end(int_seq);
    return seqexpr;
}

pbl_expr *pbl_mk_rng_expr(char *bindvar, pbl_expr *from, pbl_expr *to)
{
    return new ast::RngExpr(bindvar, from, to);
}

pbl_expr *pbl_rng_expr_set_bindvar(pbl_expr *expr, char *bindvar)
{
    if (ast::RngExpr *rng_expr = dynamic_cast<ast::RngExpr *>(expr)) {
        rng_expr->set_bindvar(bindvar);
    }
    return expr;
}

void pbl_expr_free(pbl_expr *expr)
{
    delete expr;
}

} // namespace parameterised
} // namespace scribble
