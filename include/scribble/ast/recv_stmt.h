#ifndef SCRIBBLE__AST__RECV_STMT_H__
#define SCRIBBLE__AST__RECV_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/interaction.h>

#include "scribble/msg.h"
#include "scribble/role.h"
#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class RecvStmt : public sesstype::InteractionNode, public Stmt {
  public:
    RecvStmt() : sesstype::InteractionNode(), Stmt() { }

    RecvStmt(Message *msg) : sesstype::InteractionNode(msg), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_recvstmt_init();

sbl_stmt *sbl_mk_recvstmt(sbl_msg *msg);

sbl_stmt *sbl_recvstmt_set_from(sbl_stmt *stmt, sbl_role *from);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__RECV_STMT_H__
