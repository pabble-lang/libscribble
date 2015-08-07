#ifndef SCRIBBLE__PARAMETERISED__AST__ADD_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__ADD_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/add.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class AddExpr
    : public sesstype::parameterised::AddExpr, public ArithExpr {
  public:
    AddExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::AddExpr(lhs, rhs),
          ArithExpr(ST_EXPR_ADD) { }

    AddExpr(const scribble::parameterised::ast::AddExpr &expr)
        : sesstype::parameterised::AddExpr(expr), ArithExpr(expr) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::AddExpr *>(this));
    }

    sesstype::parameterised::AddExpr *clone() const override
    {
        return sesstype::parameterised::AddExpr::clone();
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
typedef ast::AddExpr pbl_add_expr;
#else
typedef struct AddExpr pbl_add_expr;
#endif

pbl_expr *pbl_mk_add_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__ADD_EXPR_H__
