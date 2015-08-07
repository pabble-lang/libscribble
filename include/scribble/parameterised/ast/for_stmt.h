#ifndef SCRIBBLE__PARAMETERISED__AST__FOR_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__FOR_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/for.h>

#include "scribble/parameterised/ast/stmt.h"
#include "scribble/parameterised/ast/expr.h"
#include "scribble/parameterised/ast/rng_expr.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ForStmt : public sesstype::parameterised::ForNode, public Stmt {
  public:
    ForStmt(scribble::parameterised::ast::RngExpr *bindexpr)
        : sesstype::parameterised::ForNode(bindexpr), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_forstmt(pbl_expr *bindexpr, pbl_stmt *body);

pbl_stmt *pbl_mk_forstmt_except(pbl_expr *bindexpr, pbl_stmt *body, pbl_expr *except);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__FOR_STMT_H__
