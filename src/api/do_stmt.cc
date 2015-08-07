#include <iostream>
#include "scribble/ast/do_stmt.h"
#include "scribble/ast/stmt.h"

namespace scribble {
namespace ast {

sbl_stmt *sbl_mk_dostmt(char *protocol_name)
{
    return new DoStmt(protocol_name);
}

sbl_stmt *sbl_dostmt_set_scope(sbl_stmt *stmt, char *scope)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->set_scope(scope);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

sbl_stmt *sbl_dostmt_add_arg(sbl_stmt *stmt, sbl_msg *arg)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->add_arg(arg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

sbl_stmt *sbl_dostmt_add_rolearg(sbl_stmt *stmt, sbl_role *rolearg)
{
    if (auto dostmt = dynamic_cast<DoStmt *>(stmt)) {
        dostmt->add_arg(rolearg);
    } else {
        std::cerr << __FUNCTION__ << ": Not a dostmt\n";
    }
    return stmt;
}

} // namespace ast
} // namespace scribble
