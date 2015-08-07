#include <string>
#include <scribble/msg.h>

namespace scribble {

sbl_msg *sbl_mk_msg_init()
{
    return new Message("");
}

sbl_msg *sbl_mk_msg(char *label)
{
    return new Message(label);
}

sbl_msg *sbl_msg_add_payload(sbl_msg *msg, sbl_msg_payload *payload)
{
    msg->add_payload(payload);
    return msg;
}

void sbl_msg_free(sbl_msg *msg)
{
    delete msg;
}

sbl_msg_payload *sbl_mk_msg_payload(char *type)
{
    return new MessagePayload(type);
}

sbl_msg_payload *sbl_mk_msg_payload_annotated(char *type, char *name)
{
    return new MessagePayload(type, name);
}

void sbl_msg_payload_free(sbl_msg_payload *payload)
{
    delete payload;
}

} // namesapce scribble
