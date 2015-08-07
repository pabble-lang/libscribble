#include <iostream>
#include <scribble/util/srclocation.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/block_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_blkstmt_init()
{
    return new ast::BlockStmt();
}

bool pbl_stmt_is_blkstmt(pbl_stmt *stmt)
{
    return (true && static_cast<ast::BlockStmt *>(stmt));
}

unsigned int pbl_blkstmt_num_children(pbl_stmt *stmt)
{
    if (ast::BlockStmt *blkstmt = static_cast<ast::BlockStmt *>(stmt)) {
        return blkstmt->num_children();
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return 0;
}

pbl_stmt *pbl_blkstmt_append_child(pbl_stmt *stmt, pbl_stmt *child)
{
    if (ast::BlockStmt *blkstmt = static_cast<ast::BlockStmt *>(stmt)) {
        if (ast::ScribbleStmt *sblstmt = static_cast<ast::ScribbleStmt *>(child)) {
            blkstmt->append_child(sblstmt);
        }
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return stmt;
}

pbl_stmt *pbl_blkstmt_get_child(pbl_stmt *stmt, unsigned int index)
{
    if (ast::BlockStmt *blkstmt = static_cast<ast::BlockStmt *>(stmt)) {
        return static_cast<ast::ScribbleStmt *>(blkstmt->child(index));
    } else {
        std::cerr << __FUNCTION__ << ": Not a blockstmt\n";
    }
    return stmt;
}

void pbl_blkstmt_free(pbl_stmt *stmt)
{
    delete stmt;
}

} // namespace parameterised
} // namespace scribble
