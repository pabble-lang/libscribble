#include "scribble/ast/continue_stmt.h"

namespace scribble {
namespace ast {

sbl_stmt *sbl_mk_continuestmt(char *label)
{
    return new ContinueStmt(label);
}

} // namespace ast
} // namesapce scribble
