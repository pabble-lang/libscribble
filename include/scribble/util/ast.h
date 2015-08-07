#ifndef SCRIBBLE__UTILS__AST_H__
#define SCRIBBLE__UTILS__AST_H__

#include "scribble/ast.h"

namespace sesstype {
namespace utils {

class ASTVisitor {
  public:
    virtual void visit(SendStmt *stmt) = 0;
    virtual void visit(RecvStmt *stmt) = 0;
    virtual void visit(SendRecvStmt *stmt) = 0;
};

} // namespace utils
} // namespace sesstype


#endif//SCRIBBLE__UTILS__AST_H__
