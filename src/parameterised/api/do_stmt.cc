#include <iostream>
#include "scribble/parameterised/ast/stmt.h"
#include "scribble/parameterised/ast/do_stmt.h"

namespace scribble {
namespace parameterised {
namespace ast {

pbl_stmt *pbl_mk_dostmt(char *protocol_name)
{
    return new DoStmt(protocol_name);
}

pbl_stmt *pbl_dostmt_set_scope(pbl_stmt *stmt, char *scope)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->set_scope(scope);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

pbl_stmt *pbl_dostmt_add_arg(pbl_stmt *stmt, pbl_msg *arg)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->add_arg(arg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

pbl_stmt *pbl_dostmt_add_rolearg(pbl_stmt *stmt, pbl_role *rolearg)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->add_arg(rolearg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

} // namespace ast
} // namespace parameterised
} // namespace scribble
