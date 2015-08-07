#ifndef SCRIBBLE__AST__BLOCK_STMT_H__
#define SCRIBBLE__AST__BLOCK_STMT_H__

#ifdef __cplusplus
#include <vector>
#endif

#include <sesstype/node.h>
#include <sesstype/util/node_visitor.h>

#include "scribble/ast/stmt.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
/**
 * \brief Class representing a generic block of Stmt.
 */
class BlockStmt : public sesstype::BlockNode, public Stmt {
  public:
    BlockStmt() : sesstype::BlockNode(), Stmt() { }
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_mk_blkstmt_init();

sbl_stmt *sbl_blkstmt_append_child(sbl_stmt *stmt, sbl_stmt *child);

unsigned int sbl_blkstmt_num_children(sbl_stmt *stmt);

sbl_stmt *sbl_blkstmt_get_child(sbl_stmt *stmt, unsigned int index);

void sbl_blkstmt_free(sbl_stmt *stmt);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__BLOCK_STMT_H__
