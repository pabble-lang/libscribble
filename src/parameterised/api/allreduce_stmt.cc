#include <scribble/parameterised/ast/stmt.h>
#include <scribble/parameterised/ast/allreduce_stmt.h>

namespace scribble {
namespace parameterised {
namespace ast {

pbl_stmt *pbl_mk_allreducestmt_init()
{
    return new AllReduceStmt();
}

pbl_stmt *pbl_mk_allreducestmt(pbl_msg *msg)
{
    return new AllReduceStmt(msg);
}

} // namespace ast
} // namespace parameterised
} // namespace scribble
