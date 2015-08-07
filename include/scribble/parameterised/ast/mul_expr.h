#ifndef SCRIBBLE__PARAMETERISED__AST__MUL_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__MUL_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/mul.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class MulExpr
    : public sesstype::parameterised::MulExpr, public ArithExpr {
  public:
    MulExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::MulExpr(lhs, rhs), ArithExpr(ST_EXPR_MUL) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::MulExpr *>(this));
    }

    sesstype::parameterised::MulExpr *clone() const override
    {
        return sesstype::parameterised::MulExpr::clone();
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
typedef ast::MulExpr pbl_mul_expr;
#else
typedef struct MulExpr pbl_mul_expr;
#endif

pbl_expr *pbl_mk_mul_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__MUL_EXPR_H__
