#ifndef SCRIBBLE__PARAMETERISED__AST__SUB_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__SUB_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/sub.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class SubExpr
    : public sesstype::parameterised::SubExpr, public ArithExpr {
  public:
    SubExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::SubExpr(lhs, rhs), ArithExpr(ST_EXPR_SUB) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::SubExpr *>(this));
    }

    sesstype::parameterised::SubExpr *clone() const override
    {
        return sesstype::parameterised::SubExpr::clone();
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
typedef ast::SubExpr pbl_sub_expr;
#else
typedef struct SubExpr pbl_sub_expr;
#endif

pbl_expr *pbl_mk_sub_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SUB_EXPR_H__
