#ifndef SCRIBBLE__AST__MSG_H__
#define SCRIBBLE__AST__MSG_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/msg.h>

#ifdef __cplusplus
namespace scribble {
#endif

#ifdef __cplusplus
using Message = sesstype::MsgSig;
using MessagePayload = sesstype::MsgPayload;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Message sbl_msg;
typedef MessagePayload sbl_msg_payload;
#else
typedef struct Message sbl_msg;
typedef struct MessagePayload sbl_msg_payload;
#endif

/// \brief Make a new Message (with empty label as default).
sbl_msg *sbl_mk_msg_empty();

sbl_msg *sbl_mk_msg(char *label);

sbl_msg *sbl_msg_add_payload(sbl_msg *msg, sbl_msg_payload *payload);

void sbl_msg_free(sbl_msg *msg);

sbl_msg_payload *sbl_mk_msg_payload(char *type);

sbl_msg_payload *sbl_mk_msg_payload_annotated(char *type, char *name);

void sbl_msg_payload_free(sbl_msg_payload *payload);

#ifdef __cplusplus
} // __cplusplus
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__AST__MSG_H__
