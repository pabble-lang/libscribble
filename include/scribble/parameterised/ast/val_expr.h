#ifndef SCRIBBLE__PARAMETERISED__AST__VAL_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__VAL_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/val.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ValExpr
    : public sesstype::parameterised::ValExpr , public ArithExpr {
  public:
    ValExpr(unsigned int num)
        : sesstype::parameterised::ValExpr(num), ArithExpr(ST_EXPR_CONST) { }

    sesstype::parameterised::ValExpr *clone() const override
    {
        return sesstype::parameterised::ValExpr::clone();
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
typedef ast::ValExpr pbl_val_expr;
#else
typedef struct ValExpr pbl_val_expr;
#endif

pbl_expr *pbl_mk_val_expr(unsigned int val);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__VAL_EXPR_H__
