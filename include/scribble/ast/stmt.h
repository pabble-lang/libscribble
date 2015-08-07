#ifndef SCRIBBLE__AST__STMT_H__
#define SCRIBBLE__AST__STMT_H__

#include <sesstype/node.h>

#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace ast {
#endif

#ifdef __cplusplus
/**
 * \brief Interface of a Stmt.
 */
class Stmt : public scribble::util::SrcLocatable {
  public:
    Stmt() : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};

class ScribbleStmt : public sesstype::Node, public Stmt {
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace ast
#endif

#ifdef __cplusplus
typedef ast::Stmt sbl_stmt;
#else
typedef struct Stmt sbl_stmt;
#endif

#ifdef __cplusplus
extern "C" {
#endif

sbl_stmt *sbl_stmt_set_src_range(sbl_stmt *stmt,
        unsigned int line_start, unsigned int column_start,
        unsigned int line_end, unsigned int column_end);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__STMT_H__
