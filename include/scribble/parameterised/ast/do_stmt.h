#ifndef SCRIBBLE__PARAMETERISED__AST__DO_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__DO_STMT_H__

#include <sesstype/parameterised/node/nested.h>

#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class DoStmt : public sesstype::parameterised::NestedNode, public Stmt {
  public:
    DoStmt(std::string protocol_name)
        : sesstype::parameterised::NestedNode(protocol_name), Stmt() { }

    DoStmt(std::string protocol_name, std::string scope)
        : sesstype::parameterised::NestedNode(protocol_name, scope), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_dostmt(char *protocol_name);

pbl_stmt *pbl_dostmt_set_scope(pbl_stmt *stmt, char *scope);

pbl_stmt *pbl_dostmt_add_arg(pbl_stmt *stmt, pbl_msg *msg);

pbl_stmt *pbl_dostmt_add_rolearg(pbl_stmt *stmt, pbl_role *role);

pbl_stmt *pbl_dostmt_set_scope_name(pbl_stmt *stmt, char *scope);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace ast
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__DO_STMT_H__
