#ifndef SCRIBBLE__AST__DO_STMT_H__
#define SCRIBBLE__AST__DO_STMT_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/node.h>
#include <sesstype/node/nested.h>

#include "scribble/msg.h"
#include "scribble/role.h"
#include "scribble/ast/stmt.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class DoStmt : public sesstype::NestedNode, public Stmt {
  public:
    DoStmt(std::string protocol_name)
        : sesstype::NestedNode(protocol_name), Stmt() { }

    DoStmt(std::string protocol_name, std::string scope)
        : sesstype::NestedNode(protocol_name, scope), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_dostmt(char *protocol_name);

sbl_stmt *sbl_dostmt_set_scope(sbl_stmt *stmt, char *scope);

char *sbl_dostmt_get_scope(sbl_stmt *stmt);

sbl_stmt *sbl_dostmt_add_arg(sbl_stmt *stmt, sbl_msg *arg);

sbl_stmt *sbl_dostmt_add_rolearg(sbl_stmt *stmt, sbl_role *rolearg);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__DO_STMT_H__
