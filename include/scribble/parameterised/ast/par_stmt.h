#ifndef SCRIBBLE__PARAMETERISED__AST__PAR_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__PAR_STMT_H__

#include <sesstype/parameterised/node/par.h>

#include "scribble/ast/par_stmt.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ParStmt : public sesstype::parameterised::ParNode, public Stmt {
  public:
    ParStmt() : sesstype::parameterised::ParNode(), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_parstmt_init();

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__PAR_STMT_H__
