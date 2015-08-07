#include "scribble/role.h"
#include "scribble/ast/stmt.h"
#include "scribble/ast/choice_stmt.h"

namespace scribble {
namespace ast {

sbl_stmt *sbl_mk_choicestmt(sbl_role *at)
{
    return new ChoiceStmt(at);
}

} // namespace ast
} // namespace scribble
