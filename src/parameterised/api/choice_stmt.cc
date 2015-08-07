#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/choice_stmt.h>

namespace scribble {
namespace parameterised {
namespace ast {

pbl_stmt *pbl_mk_choicestmt(pbl_role *at)
{
    return new ChoiceStmt(at);
}

} // namespace ast
} // namespace parameterised
} // namespace scribble
