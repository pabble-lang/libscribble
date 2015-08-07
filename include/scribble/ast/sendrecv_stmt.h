#ifndef SCRIBBLE__AST__SENDRECV_STMT_H__
#define SCRIBBLE__AST__SENDRECV_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/interaction.h>

#include "scribble/msg.h"
#include "scribble/role.h"
#include "scribble/ast/stmt.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

class SendRecvStmt : public sesstype::InteractionNode, public Stmt {
  public:
    SendRecvStmt() : sesstype::InteractionNode(), Stmt() { }

    SendRecvStmt(Message *msg) : sesstype::InteractionNode(msg), Stmt() { }
};

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_sendrecvstmt_init();

sbl_stmt *sbl_mk_sendrecvstmt(sbl_msg *msg);

sbl_stmt *sbl_sendrecvstmt_add_to(sbl_stmt *stmt, sbl_role *to);

sbl_stmt *sbl_sendrecvstmt_set_from(sbl_stmt *stmt, sbl_role *from);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__SENDRECV_STMT_H__
