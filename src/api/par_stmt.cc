#include "scribble/ast/stmt.h"
#include "scribble/ast/par_stmt.h"

namespace scribble {
namespace ast {

sbl_stmt *sbl_mk_parstmt()
{
    return new ParStmt();
}

} // namespace ast
} // namespace scribble
