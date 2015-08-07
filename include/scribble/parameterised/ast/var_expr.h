#ifndef SCRIBBLE__PARAMETERISED__AST__VAR_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__VAR_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/var.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class VarExpr
    : public sesstype::parameterised::VarExpr, public ArithExpr {
  public:
    VarExpr(std::string name)
        : sesstype::parameterised::VarExpr(name),
          scribble::parameterised::ast::ArithExpr(ST_EXPR_VAR) { }

    sesstype::parameterised::VarExpr *clone() const override
    {
        return sesstype::parameterised::VarExpr::clone();
    }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef ast::VarExpr pbl_var_expr;
#else
typedef struct VarExpr pbl_var_expr;
#endif

pbl_expr *pbl_mk_var_expr(char *var);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__VAR_EXPR_H__
