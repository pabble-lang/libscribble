#include <string>
#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/ast/expr.h>

namespace scribble {
namespace parameterised {

pbl_msg *pbl_mk_msg_empty()
{
    return new Message("");
}

pbl_msg *pbl_mk_msg(char *label)
{
    return new Message(label);
}

pbl_msg *pbl_msg_add_payload(pbl_msg *msg, pbl_msg_payload *payload)
{
    msg->add_payload(payload);
    return msg;
}

void pbl_msg_free(sbl_msg *msg)
{
    delete msg;
}

pbl_msg_payload *pbl_mk_msg_payload(char *type)
{
    return new MessagePayload(type);
}

pbl_msg_payload *pbl_mk_msg_payload_annotated(char *type, char *name)
{
    return new MessagePayload(type, name);
}

pbl_msg_payload *pbl_msg_payload_add_arg(pbl_msg_payload *payload, pbl_expr *arg)
{
    if (ast::ArithExpr *expr = dynamic_cast<ast::ArithExpr *>(arg)) {
        payload->add_param(expr);
    }
    return payload;
}

void pbl_msg_payload_free(pbl_msg_payload *payload)
{
    delete payload;
}

} // namespace parameterised
} // namesapce scribble
