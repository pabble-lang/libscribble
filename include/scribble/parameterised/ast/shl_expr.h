#ifndef SCRIBBLE__PARAMETERISED__AST__SHL_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__SHL_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/shl.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ShlExpr
    : public sesstype::parameterised::ShlExpr, public ArithExpr {
  public:
    ShlExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::ShlExpr(lhs, rhs), ArithExpr(ST_EXPR_SHL) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::ShlExpr *>(this));
    }

    sesstype::parameterised::ShlExpr *clone() const override
    {
        return sesstype::parameterised::ShlExpr::clone();
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
typedef ast::ShlExpr pbl_shl_expr;
#else
typedef struct ShlExpr pbl_shl_expr;
#endif

pbl_expr *pbl_mk_shl_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SHL_EXPR_H__
