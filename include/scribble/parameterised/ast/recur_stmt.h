#ifndef SCRIBBLE__PARAMETERISED__AST__RECUR_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__RECUR_STMT_H__

#include <sesstype/parameterised/node/recur.h>

#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class RecurStmt : public sesstype::parameterised::RecurNode, public Stmt {
  public:
    RecurStmt(std::string label)
        : sesstype::parameterised::RecurNode(label), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_recurstmt(char *label);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__RECUR_STMT_H__
