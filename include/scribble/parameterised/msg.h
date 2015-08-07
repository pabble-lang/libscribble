#ifndef SCRIBBLE__PARAMETERISED__MSG_H__
#define SCRIBBLE__PARAMETERISED__MSG_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/msg.h>
#include <sesstype/parameterised/msg.h>

#include "scribble/msg.h"
#include "scribble/parameterised/role.h"
#include "scribble/parameterised/ast/expr.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
using Message = sesstype::parameterised::MsgSig;
using MessagePayload = sesstype::parameterised::MsgPayload;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Message pbl_msg;
typedef MessagePayload pbl_msg_payload;
#else
typedef struct Message pbl_msg;
typedef struct MessagePayload pbl_msg_payload;
#endif

/// \brief Make a new Message (with empty label as default).
pbl_msg *pbl_mk_msg_empty();

pbl_msg *pbl_mk_msg(char *label);

pbl_msg *pbl_msg_add_payload(pbl_msg *msg, pbl_msg_payload *payload);

void pbl_msg_free(sbl_msg *msg);

pbl_msg_payload *pbl_mk_msg_payload(char *type);

pbl_msg_payload *pbl_mk_msg_payload_annotated(char *type, char *name);

pbl_msg_payload *pbl_msg_payload_add_arg(pbl_msg_payload *payload, pbl_expr *arg);

void pbl_msg_payload_free(pbl_msg_payload *payload);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif


#endif//SCRIBBLE__PARAMETERISED__AST__MSG_H__
