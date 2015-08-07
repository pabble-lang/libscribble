#ifndef SCRIBBLE__AST__INTERRUPTIBLE_STMT_H__
#define SCRIBBLE__AST__INTERRUPTIBLE_STMT_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/node.h>
#include <sesstype/node/interruptible.h>

#include "scribble/msg.h"
#include "scribble/role.h"
#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class InterruptibleStmt : public sesstype::InterruptibleNode, public Stmt {
  public:
    InterruptibleStmt() : sesstype::InterruptibleNode(), Stmt() { }

    InterruptibleStmt(std::string scope)
        : sesstype::InterruptibleNode(scope), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_interruptiblestmt_init();

sbl_stmt *sbl_mk_interruptiblestmt_set_scope(sbl_stmt *stmt, char *scope);

sbl_stmt *sbl_interruptiblestmt_add_interrupt(sbl_stmt *stmt, sbl_role* role, sbl_msg *msg);

sbl_stmt *sbl_interruptiblestmt_add_catch(sbl_stmt *stmt, sbl_role* role, sbl_msg *msg);

sbl_stmt *sbl_interruptiblestmt_add_throw(sbl_stmt *stmt, sbl_role* role, sbl_msg *msg);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__INTERRUPTIBLE_STMT_H__
