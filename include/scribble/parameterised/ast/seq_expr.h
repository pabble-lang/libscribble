#ifndef SCRIBBLE__PARAMETERISED__AST__SEQ_EXPR_H__
#define SCRIBBLE__PARAMETERISED__AST__SEQ_EXPR_H__

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/seq.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class SeqExpr : public sesstype::parameterised::SeqExpr, public ArithExpr {
  public:
    SeqExpr() : sesstype::parameterised::SeqExpr(), ArithExpr(ST_EXPR_SEQ) { }

    void accept(sesstype::parameterised::util::ExprVisitor &v) override
    {
        v.visit(static_cast<sesstype::parameterised::SeqExpr *>(this));
    }

    sesstype::parameterised::SeqExpr *clone() const override
    {
        return sesstype::parameterised::SeqExpr::clone();
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
typedef ast::SeqExpr pbl_seq_expr;
#else
typedef struct SeqExpr pbl_seq_expr;
#endif

pbl_expr *pbl_mk_seq_expr(unsigned int count, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SEQ_EXPR_H__
