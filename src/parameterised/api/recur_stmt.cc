#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/recur_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_recurstmt(char *label)
{
    return new ast::RecurStmt(label);
}

} // namespace parameterised
} // namespace scribble
