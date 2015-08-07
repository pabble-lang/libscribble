#ifndef SCRIBBLE__PARAMETERISED__PROTOCOL_H__
#define SCRIBBLE__PARAMETERISED__PROTOCOL_H__

#include <sesstype/parameterised/session.h>

#include "scribble/parameterised/role.h"
#include "scribble/parameterised/role_grp.h"
#include "scribble/parameterised/ast/stmt.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
class Protocol
    : public sesstype::parameterised::Session, public util::SrcLocatable {
  public:
    Protocol()
        : sesstype::parameterised::Session(),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }

    Protocol(std::string name)
        : sesstype::parameterised::Session(name),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};
#endif

#ifdef __cplusplus
typedef Protocol pbl_protocol;
#else
typedef struct Protocol pbl_protocol;
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_protocol *pbl_mk_protocol(char *name);

const char *pbl_protocol_get_name(pbl_protocol *protocol);

pbl_protocol *pbl_protocol_set_endpoint(pbl_protocol *protocol, pbl_role *role);

pbl_protocol *pbl_protocol_add_role(pbl_protocol *protocol, pbl_role *role);

pbl_protocol *pbl_protocol_add_role_grp(pbl_protocol *protocol, pbl_role_grp *role_grp);

pbl_protocol *pbl_protocol_set_body(pbl_protocol *protocol, pbl_stmt *body);

void pbl_protocol_free(pbl_protocol *protocol);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__PROTOCOL_H__
