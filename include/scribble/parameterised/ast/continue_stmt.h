#ifndef SCRIBBLE__PARAMETERISED__AST__CONTINUE_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__CONTINUE_STMT_H__

#include <sesstype/parameterised/node/continue.h>

#include "scribble/ast/continue_stmt.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ContinueStmt : public sesstype::parameterised::ContinueNode, public Stmt {
  public:
    /// \brief ContinueStmt constructor.
    ContinueStmt(std::string label)
        : sesstype::parameterised::ContinueNode(label), Stmt() { }
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_continuestmt(char *label);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace ast
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__CONTINUE_STMT_H__
