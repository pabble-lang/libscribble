#ifndef SCRIBBLE__PARAMETERISED__AST__INTERRUPTIBLE_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__INTERRUPTIBLE_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/interruptible.h>

#include "scribble/ast/interruptible_stmt.h"
#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class InterruptibleStmt
    : public sesstype::parameterised::InterruptibleNode, public Stmt {
  public:
    InterruptibleStmt() : sesstype::parameterised::InterruptibleNode(), Stmt() { }

    InterruptibleStmt(std::string scope)
        : sesstype::parameterised::InterruptibleNode(scope), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_interruptiblestmt_init();

pbl_stmt *pbl_interruptiblestmt_set_scope(pbl_stmt *stmt, char *scope);

pbl_stmt *pbl_interruptiblestmt_add_interrupt(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg);

pbl_stmt *pbl_interruptiblestmt_add_throw(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg);

pbl_stmt *pbl_interruptiblestmt_add_catch(pbl_stmt *stmt, pbl_role *role, pbl_msg *msg);


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__INTERRUPTIBLE_STMT_H__
