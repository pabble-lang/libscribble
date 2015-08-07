#include <scribble/parameterised/cond.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/ifblk_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_ifblkstmt(pbl_cond *cond)
{
    return new ast::IfBlkStmt(cond);
}

} // namespace parameterised
} // namespace scribble
