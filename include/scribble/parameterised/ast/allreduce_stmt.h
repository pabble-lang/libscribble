#ifndef SCRIBBLE__PARAMETERISED__AST__ALLREDUCE_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__ALLREDUCE_STMT_H__

#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/node/allreduce.h>

#include "scribble/parameterised/msg.h"
#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class AllReduceStmt
    : public sesstype::parameterised::AllReduceNode, public Stmt {
  public:
    AllReduceStmt() : sesstype::parameterised::AllReduceNode(), Stmt() { }

    AllReduceStmt(Message *msg)
        : sesstype::parameterised::AllReduceNode(msg), Stmt() { }
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_allreducestmt_init();

pbl_stmt *pbl_mk_allreducestmt(pbl_msg *msg);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
} // namespace ast
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__ALLREDUCE_STMT_H__
