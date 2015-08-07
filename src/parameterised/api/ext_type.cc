#include <string>
#include <scribble/parameterised/ext_type.h>

namespace scribble {
namespace parameterised {

pbl_exttype *pbl_mk_exttype_sig(char *name, char *ext_src, char *ext_name, char *ext_schema)
{
    return new ExternalType(name, SCRIBBLE_EXT_TYPE_SIG, ext_src, ext_name, ext_schema);
}

pbl_exttype *pbl_mk_exttype_type(char *name, char *ext_src, char *ext_name, char *ext_schema)
{
    return new ExternalType(name, SCRIBBLE_EXT_TYPE_TYPE, ext_src, ext_name, ext_schema);
}

unsigned int pbl_exttype_get_type(pbl_exttype *exttype)
{
    return exttype->type();
}

const char *pbl_exttype_get_name(pbl_exttype *exttype)
{
    return exttype->name().c_str();
}

const char *pbl_exttype_get_ext_source(pbl_exttype *exttype)
{
    return exttype->ext_source().c_str();
}

const char *pbl_exttype_get_ext_name(pbl_exttype *exttype)
{
    return exttype->ext_name().c_str();
}

const char *pbl_exttype_get_ext_schema(pbl_exttype *exttype)
{
    return exttype->ext_schema().c_str();
}

void pbl_exttype_free(pbl_exttype *exttype)
{
    delete exttype;
}

} // namespace parameterised
} // namespace scribble
