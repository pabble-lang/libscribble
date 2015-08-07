#ifndef SCRIBBLE__PARAMETERISED__UTIL__EXPR_VISITOR_H__
#define SCRIBBLE__PARAMETERISED__UTIL__EXPR_VISITOR_H__

#include <sesstype/parameterised/util/expr_visitor.h>

#include "scribble/parameterised/ast/expr.h"
#include "scribble/parameterised/ast/var_expr.h"
#include "scribble/parameterised/ast/val_expr.h"
#include "scribble/parameterised/ast/add_expr.h"
#include "scribble/parameterised/ast/sub_expr.h"
#include "scribble/parameterised/ast/mul_expr.h"
#include "scribble/parameterised/ast/div_expr.h"
#include "scribble/parameterised/ast/mod_expr.h"
#include "scribble/parameterised/ast/shl_expr.h"
#include "scribble/parameterised/ast/shr_expr.h"
#include "scribble/parameterised/ast/seq_expr.h"
#include "scribble/parameterised/ast/rng_expr.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace util {
#endif

#ifdef __cplusplus
class ExprVisitor : sesstype::parameterised::util::ExprVisitor {
  public:
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace util
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__UTIL__EXPR_VISITOR_H__
