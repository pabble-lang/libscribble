#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/par_stmt.h>

namespace scribble {
namespace parameterised {

pbl_stmt *pbl_mk_parstmt_init()
{
    return new ast::ParStmt();
}

} // namespace parameterised
} // namespace scribble
