#ifndef SCRIBBLE__PARAMETERISED__AST__IFBLK_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__IFBLK_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/if.h>

#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/cond.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class IfBlkStmt : public sesstype::parameterised::IfNode, public Stmt {
  public:
    IfBlkStmt(Cond *cond) : sesstype::parameterised::IfNode(cond), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_ifblkstmt(pbl_cond *cond);

#ifdef __cplusplus
} // extern "C"
#endif


#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif


#endif//SCRIBBLE__PARAMETERISED__AST__IFBLK_STMT_H__
