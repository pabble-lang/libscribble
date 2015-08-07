#ifndef SCRIBBLE__PARAMETERISED__AST__DIV_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__DIV_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/div.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class DivExpr
    : public sesstype::parameterised::DivExpr, public ArithExpr {
  public:
    DivExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::DivExpr(lhs, rhs), ArithExpr(ST_EXPR_DIV) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::DivExpr *>(this));
    }

    sesstype::parameterised::DivExpr *clone() const override
    {
        return sesstype::parameterised::DivExpr::clone();
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
typedef ast::DivExpr pbl_div_expr;
#else
typedef struct DivExpr pbl_div_expr;
#endif

pbl_expr *pbl_mk_div_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__DIV_EXPR_H__
