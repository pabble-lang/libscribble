#ifndef SCRIBBLE__PARAMETERISED__AST__SEND_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__SEND_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/interaction.h>

#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/cond.h"
#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class SendStmt : public sesstype::parameterised::InteractionNode, public Stmt {
  public:
    SendStmt() : sesstype::parameterised::InteractionNode(), Stmt() { }

    SendStmt(Message *msg)
        : sesstype::parameterised::InteractionNode(msg), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_sendstmt_init();

pbl_stmt *pbl_mk_sendstmt(pbl_msg *msg);

pbl_stmt *pbl_sendstmt_add_to(pbl_stmt *stmt, pbl_role *to);

pbl_stmt *pbl_sendstmt_set_cond(pbl_stmt *stmt, pbl_cond *cond);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SEND_STMT_H__
