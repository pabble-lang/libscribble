#ifndef SCRIBBLE__EXT_TYPE_H__
#define SCRIBBLE__EXT_TYPE_H__

#include "scribble/msg.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
#endif

#define SCRIBBLE_EXT_TYPE_SIG  0
#define SCRIBBLE_EXT_TYPE_TYPE 1

#ifdef __cplusplus
class ExternalType : public util::SrcLocatable {
    std::string name_;
    unsigned int type_;
    std::string ext_source_;
    std::string ext_name_;
    std::string ext_schema_;

  public:
    /// \brief ExternalType constructor.
    ExternalType(std::string name, unsigned int type,
            std::string ext_source,
            std::string ext_name,
            std::string ext_schema)
        : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()),
          name_(name), type_(type),
          ext_source_(ext_source),
          ext_name_(ext_name),
          ext_schema_(ext_schema) { }

    /// \brief ExternalType destructor.
    ~ExternalType() { }

    /// \returns name/alias of the external type declaration.
    std::string name() const { return name_; }

    /// \returns type of the external message (signature or type)
    unsigned int type() const { return type_; }

    /// \returns external source (e.g. rt.jar) of declaration.
    std::string ext_source() const { return ext_source_; }

    /// \returns external name (e.g. String) of declaration.
    std::string ext_name() const { return ext_name_; }

    /// \returns external schema (e.g. java.io.String) of declaration.
    std::string ext_schema() const { return ext_schema_; }
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef ExternalType sbl_exttype;
#else
typedef struct ExternalType sbl_exttype;
#endif

sbl_exttype *sbl_mk_exttype_sig(
        char *name,
        char *ext_src,
        char *ext_name,
        char *ext_schema);

sbl_exttype *sbl_mk_exttype_type(
        char *name,
        char *ext_src,
        char *ext_name,
        char *ext_schema);

unsigned int sbl_exttype_get_type(sbl_exttype *exttype);

const char *sbl_exttype_get_name(sbl_exttype *exttype);

const char *sbl_exttype_get_ext_source(sbl_exttype *exttype);

const char *sbl_exttype_get_ext_name(sbl_exttype *exttype);

const char *sbl_exttype_get_ext_schema(sbl_exttype *exttype);

void sbl_exttype_free(sbl_exttype *exttype);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__EXT_TYPE_H__
