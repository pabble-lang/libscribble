#include <scribble/ext_type.h>
#include <scribble/import.h>
#include <scribble/module.h>
#include <scribble/protocol.h>

namespace scribble {

sbl_module *sbl_mk_module_init()
{
    return new Module();
}

sbl_module *sbl_mk_module(char *name)
{
    return new Module(name);
}

sbl_module *sbl_module_add_protocol(sbl_module *module, sbl_protocol *protocol)
{
    module->add_session(protocol);
    return module;
}

sbl_module *sbl_module_add_import(sbl_module *module, sbl_import *import)
{
    module->add_import(import);
    return module;
}

sbl_module *sbl_module_add_datatype(sbl_module *module, sbl_exttype *datatype)
{
    module->add_datatype(datatype);
    return module;
}

void sbl_module_free(sbl_module *module)
{
    delete module;
}

} // namespace scribble
