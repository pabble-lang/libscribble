#ifndef SCRIBBLE__AST__CONTINUE_STMT_H__
#define SCRIBBLE__AST__CONTINUE_STMT_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/node.h>
#include <sesstype/node/continue.h>

#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class ContinueStmt : public sesstype::ContinueNode, public Stmt {
  public:
    ContinueStmt(std::string label) : sesstype::ContinueNode(label), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_continuestmt(char *label);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__CONTINUE_STMT_H__
