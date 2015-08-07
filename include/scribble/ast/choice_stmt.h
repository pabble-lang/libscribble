#ifndef SCRIBBLE__AST__CHOICE_STMT_H__
#define SCRIBBLE__AST__CHOICE_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/choice.h>

#include "scribble/role.h"
#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class ChoiceStmt : public sesstype::ChoiceNode, public Stmt {
  public:
    ChoiceStmt(Role *at) : sesstype::ChoiceNode(at), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_choicestmt(sbl_role *at);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__CHOICE_STMT_H__
