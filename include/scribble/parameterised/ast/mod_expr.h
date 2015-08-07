#ifndef SCRIBBLE__PARAMETERISED__AST__MOD_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__MOD_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/mod.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ModExpr
    : public sesstype::parameterised::ModExpr, public ArithExpr {
  public:
    ModExpr(ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::ModExpr(lhs, rhs), ArithExpr(ST_EXPR_MOD) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::ModExpr *>(this));
    }

    sesstype::parameterised::ModExpr *clone() const override
    {
        return sesstype::parameterised::ModExpr::clone();
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
typedef ast::ModExpr pbl_mod_expr;
#else
typedef struct ModExpr pbl_mod_expr;
#endif

pbl_expr *pbl_mk_mod_expr(pbl_expr *lhs, pbl_expr *rhs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__MOD_EXPR_H__
