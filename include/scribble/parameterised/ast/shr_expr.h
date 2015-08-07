#ifndef SCRIBBLE__PARAMETERISED__AST__SHR_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__SHR_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/shr.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ShrExpr
    : public sesstype::parameterised::ShrExpr, public ArithExpr {
  public:
    ShrExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::ShrExpr(lhs, rhs), ArithExpr(ST_EXPR_SHR) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::ShrExpr *>(this));
    }

    sesstype::parameterised::ShrExpr *clone() const override
    {
        return sesstype::parameterised::ShrExpr::clone();
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
typedef ast::ShrExpr pbl_shr_expr;
#else
typedef struct ShrExpr pbl_shr_expr;
#endif

pbl_expr *pbl_mk_shr_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SHR_EXPR_H__
