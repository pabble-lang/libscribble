#ifndef SCRIBBLE__PARAMETERISED__AST__SENDRECV_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__SENDRECV_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/interaction.h>

#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class SendRecvStmt
    : public sesstype::parameterised::InteractionNode, public Stmt {
  public:
    SendRecvStmt() : sesstype::parameterised::InteractionNode(), Stmt() { }

    SendRecvStmt(Message *msg)
        : sesstype::parameterised::InteractionNode(msg), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_sendrecvstmt_init();

pbl_stmt *pbl_mk_sendrecvstmt(pbl_msg *msg);

pbl_stmt *pbl_sendrecvstmt_set_from(pbl_stmt *stmt, pbl_role *from);

pbl_stmt *pbl_sendrecvstmt_add_to(pbl_stmt *stmt, pbl_role *to);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__SENDRECV_STMT_H__
