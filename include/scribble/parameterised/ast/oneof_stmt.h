#ifndef SCRIBBLE__PARAMETERISED__ONEOF_STMT_H__
#define SCRIBBLE__PARAMETERISED__ONEOF_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/oneof.h>

#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class OneofStmt : public sesstype::parameterised::OneofNode, public Stmt {
  public:
    OneofStmt(Role *selector_role, unsigned int dimen)
        : sesstype::parameterised::OneofNode(selector_role, dimen), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_oneofstmt(pbl_role *selector, unsigned int dimen);

pbl_stmt *pbl_oneofstmt_set_var(pbl_stmt *stmt, char *var);

pbl_stmt *pbl_oneofstmt_set_range(pbl_stmt *stmt, pbl_expr *range_expr);

pbl_stmt *pbl_oneofstmt_set_repeat(pbl_stmt *stmt, bool repeat);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__ONEOF_STMT_H__
