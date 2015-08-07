#include <iostream>
#include <sesstype/node.h>

#include "scribble/ast/stmt.h"
#include "scribble/ast/block_stmt.h"

namespace scribble {

sbl_stmt *sbl_mk_blockstmt()
{
    return new ast::BlockStmt();
}

sbl_stmt *sbl_blockstmt_append_child(sbl_stmt *stmt, sbl_stmt *child)
{
    if (auto blk_stmt = dynamic_cast<ast::BlockStmt *>(stmt)) {
        if (auto sbl_stmt = dynamic_cast<ast::ScribbleStmt *>(child)) {
            blk_stmt->append_child(sbl_stmt);
        }
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return stmt;
}

unsigned int sbl_blockstmt_num_children(sbl_stmt *stmt)
{
    if (auto blk_stmt = static_cast<ast::BlockStmt *>(stmt)) {
        return blk_stmt->num_children();
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return 0;
}

sbl_stmt *sbl_blockstmt_get_child(sbl_stmt *stmt, unsigned int index)
{
    if (auto blk_stmt = dynamic_cast<ast::BlockStmt *>(stmt)) {
        return dynamic_cast<ast::ScribbleStmt *>(blk_stmt->child(index));
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return stmt;
}

void sbl_blkstmt_free(sbl_stmt *stmt)
{
    delete stmt;
}

} // namespace scribble
