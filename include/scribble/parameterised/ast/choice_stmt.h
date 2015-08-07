#ifndef SCRIBBLE__PARAMETERISED__AST__CHOICE_STMT_H__
#define SCRIBBLE__PARAMETERISED__AST__CHOICE_STMT_H__

#include <sesstype/parameterised/node/choice.h>

#include <scribble/parameterised/role.h>
#include <scribble/parameterised/ast/stmt.h>

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
namespace ast {
#endif

#ifdef __cplusplus
class ChoiceStmt : public sesstype::parameterised::ChoiceNode, public Stmt {
  public:
    ChoiceStmt(Role *at)
        : sesstype::parameterised::ChoiceNode(at), Stmt() { }
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

pbl_stmt *pbl_mk_choicestmt(pbl_role *at);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
} // namespace ast
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__AST__CHOICE_STMT_H__
