#ifndef SCRIBBLE__PARAMETERISED__AST__RNG_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__RNG_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/rng.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class RngExpr : public sesstype::parameterised::RngExpr, public ArithExpr {
  public:
    RngExpr(ArithExpr *from, ArithExpr *to)
        : sesstype::parameterised::RngExpr(from, to), ArithExpr(ST_EXPR_RNG) { }

    RngExpr(std::string bindvar, ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::RngExpr(bindvar, lhs, rhs),
          ArithExpr(ST_EXPR_RNG) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::RngExpr *>(this));
    }

    sesstype::parameterised::RngExpr *clone() const override
    {
        return sesstype::parameterised::RngExpr::clone();
    }
};
#endif // __cpluscplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef ast::RngExpr pbl_rng_expr;
#else
typedef struct RngExpr pbl_rng_expr;
#endif

pbl_expr *pbl_mk_rng_expr(char *bindvar, pbl_expr *from, pbl_expr *to);

pbl_expr *pbl_rng_expr_set_bindvar(pbl_expr *expr, char *bindvar);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__RNG_EXPR_H__
