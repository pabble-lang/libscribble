#ifndef SCRIBBLE__PARAMETERISED__AST__BLOCK_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__BLOCK_STMT_H__

#include <sesstype/parameterised/node/block.h>

#include "scribble/parameterised/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class BlockStmt : public sesstype::parameterised::BlockNode, public Stmt {
  public:
    BlockStmt() : sesstype::parameterised::BlockNode(), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_blkstmt_init();

bool pbl_stmt_is_blkstmt(pbl_stmt *stmt);

unsigned int pbl_blkstmt_num_children(pbl_stmt *stmt);

pbl_stmt *pbl_blkstmt_append_child(pbl_stmt *stmt, pbl_stmt *child);

pbl_stmt *pbl_blkstmt_get_child(pbl_stmt *stmt, unsigned int index);

void pbl_blkstmt_free(pbl_stmt *stmt);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__BLOCK_STMT_H__
