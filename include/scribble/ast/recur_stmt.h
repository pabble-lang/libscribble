#ifndef SCRIBBLE__AST__RECUR_STMT_H__
#define SCRIBBLE__AST__RECUR_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/recur.h>

#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class RecurStmt : public sesstype::RecurNode, public Stmt {
  public:
    RecurStmt(std::string label) : sesstype::RecurNode(label), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_recurstmt(char *name);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__RECUR_STMT_H__
