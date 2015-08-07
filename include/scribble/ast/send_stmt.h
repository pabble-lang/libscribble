#ifndef SCRIBBLE__AST__SEND_STMT_H__
#define SCRIBBLE__AST__SEND_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/interaction.h>

#include "scribble/msg.h"
#include "scribble/role.h"
#include "scribble/util/srclocation.h"
#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class SendStmt : public sesstype::InteractionNode, public Stmt {
  public:
    SendStmt() : sesstype::InteractionNode(), Stmt() { }

    SendStmt(Message *msg) : sesstype::InteractionNode(msg), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_sendstmt_init();

sbl_stmt *sbl_mk_sendstmt(sbl_msg *msg);

sbl_stmt *sbl_sendstmt_add_to(sbl_stmt *stmt, sbl_role *to_role);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__SEND_STMT_H__
