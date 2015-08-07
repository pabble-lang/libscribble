#ifndef SCRIBBLE__PROTOCOL_H__
#define SCRIBBLE__PROTOCOL_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/session.h>

#include "scribble/ast/stmt.h"
#include "scribble/role.h"
#include "scribble/util/clonable.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
#endif

#ifdef __cplusplus
class Protocol : public sesstype::Session, public util::SrcLocatable {
  public:
    Protocol(std::string name)
        : sesstype::Session(name),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Protocol sbl_protocol;
#else
typedef struct Protocol sbl_protocol;
#endif

sbl_protocol *sbl_mk_protocol(char *name);

const char *sbl_protocol_get_name(sbl_protocol *protocol);

sbl_protocol *sbl_protocol_set_endpoint(sbl_protocol *protocol, sbl_role *role);

sbl_protocol *sbl_protocol_set_body(sbl_protocol *protocol, sbl_stmt *body);

sbl_protocol *sbl_protocol_add_role(sbl_protocol *protocol, sbl_role *role);

void sbl_protocol_free(sbl_protocol *protocol);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__PROTOCOL_H__
