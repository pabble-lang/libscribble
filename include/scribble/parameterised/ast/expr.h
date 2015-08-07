#ifndef SCRIBBLE__PARAMETERISED__AST__EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__EXPR_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/val.h>
#include <sesstype/parameterised/util/expr_visitor.h>

#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class Expr : public util::SrcLocatable {
  public:
    Expr() : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};

class ArithExpr : public sesstype::parameterised::Expr, public Expr {
  public:
    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::Expr *>(this));
    }

  protected:
    ArithExpr(int type)
        : sesstype::parameterised::Expr(type),
          scribble::parameterised::ast::Expr() { }
};

class BinExpr : public sesstype::parameterised::BinExpr, public ArithExpr {
  public:
    BinExpr(int op, ArithExpr *lhs, ArithExpr *rhs)
        : sesstype::parameterised::BinExpr(op, lhs, rhs),
          scribble::parameterised::ast::ArithExpr(op) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::BinExpr *>(this));
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
typedef ast::ArithExpr pbl_expr;
typedef ast::BinExpr pbl_bin_expr;
#else
typedef struct ArithExpr pbl_expr;
typedef struct BinExpr pbl_bin_expr;
#endif

void pbl_expr_free(pbl_expr *expr);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__EXPR_H__
