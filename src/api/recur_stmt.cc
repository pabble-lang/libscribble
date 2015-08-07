#include "scribble/ast/stmt.h"
#include "scribble/ast/recur_stmt.h"

namespace scribble {
namespace ast {

sbl_stmt *sbl_mk_recurstmt(char *name)
{
    return new RecurStmt(name);
}

} // namespace ast
} // namespace scribble
