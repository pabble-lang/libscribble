#include <string>
#include <scribble/ext_type.h>

namespace scribble {

sbl_exttype *sbl_mk_exttype_sig(char *name, char *ext_src, char *ext_name, char *ext_schema)
{
    return new ExternalType(name, SCRIBBLE_EXT_TYPE_SIG, ext_src, ext_name, ext_schema);
}

sbl_exttype *sbl_mk_exttype_type(char *name, char *ext_src, char *ext_name, char *ext_schema)
{
    return new ExternalType(name, SCRIBBLE_EXT_TYPE_TYPE, ext_src, ext_name, ext_schema);
}

unsigned int sbl_exttype_get_type(sbl_exttype *exttype)
{
    return exttype->type();
}

const char *sbl_exttype_get_name(sbl_exttype *exttype)
{
    return exttype->name().c_str();
}

const char *sbl_exttype_get_ext_source(sbl_exttype *exttype)
{
    return exttype->ext_source().c_str();
}

const char *sbl_exttype_get_ext_name(sbl_exttype *exttype)
{
    return exttype->ext_name().c_str();
}

const char *sbl_exttype_get_ext_schema(sbl_exttype *exttype)
{
    return exttype->ext_schema().c_str();
}

void sbl_exttype_free(sbl_exttype *exttype)
{
    delete exttype;
}

} // namespace scribble
