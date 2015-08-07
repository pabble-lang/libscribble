#ifndef SCRIBBLE__PARAMETERISED__AST__RECV_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__RECV_STMT_H__

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
class RecvStmt : public sesstype::parameterised::InteractionNode, public Stmt {
  public:
    RecvStmt() : sesstype::parameterised::InteractionNode(), Stmt() { }

    RecvStmt(Message *msg)
        : sesstype::parameterised::InteractionNode(msg), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_recvstmt_init();

pbl_stmt *pbl_mk_recvstmt(pbl_msg *msg);

pbl_stmt *pbl_recvstmt_set_from(pbl_stmt *stmt, pbl_role *from);

pbl_stmt *pbl_recvstmt_set_cond(pbl_stmt *stmt, pbl_cond *cond);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__RECV_STMT_H__
