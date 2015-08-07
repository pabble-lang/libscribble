#ifndef SCRIBBLE__PARAMETERISED__AST__STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__STMT_H__

#include <sesstype/parameterised/node.h>

#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class Stmt : public util::SrcLocatable {
  public:
    Stmt() : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};

class ScribbleStmt : public sesstype::parameterised::Node, public Stmt {
};
#endif

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef ast::Stmt pbl_stmt;
#else
typedef struct Stmt pbl_stmt;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__STMT_H__
