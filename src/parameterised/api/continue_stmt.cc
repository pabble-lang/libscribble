#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/continue_stmt.h>

namespace scribble {
namespace parameterised {
namespace ast {

pbl_stmt *pbl_mk_continuestmt(char *label)
{
    return new ContinueStmt(label);
}

} // namespace ast
} // namespace parameterised
} // namespace scribble
