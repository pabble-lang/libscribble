#ifndef SCRIBBLE__AST__PAR_STMT_H__
#define SCRIBBLE__AST__PAR_STMT_H__

#include <sesstype/node.h>
#include <sesstype/node/par.h>

#include "scribble/ast/stmt.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
class ParStmt : public sesstype::ParNode, public Stmt {
  public:
    ParStmt() : sesstype::ParNode(), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_parstmt_init();

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__DO_STMT_H__
