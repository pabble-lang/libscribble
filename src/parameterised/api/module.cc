#include <scribble/parameterised/const.h>
#include <scribble/parameterised/ext_type.h>
#include <scribble/parameterised/import.h>
#include <scribble/parameterised/module.h>
#include <scribble/parameterised/protocol.h>
#include <scribble/parameterised/range.h>

namespace scribble {
namespace parameterised {

pbl_module *pbl_mk_module_init()
{
    return new Module();
}

pbl_module *pbl_mk_module(char *name)
{
    return new Module(name);
}

pbl_module *pbl_module_add_protocol(pbl_module *module, pbl_protocol *protocol)
{
    module->add_session(protocol);
    return module;
}

pbl_module *pbl_module_add_import(pbl_module *module, pbl_import *import)
{
    module->add_import(import);
    return module;
}

pbl_module *pbl_module_add_const(pbl_module *module, pbl_const *constant)
{
    module->add_constant(constant);
    return module;
}

pbl_module *pbl_module_add_exttype(pbl_module *module, pbl_exttype *exttype)
{
    module->add_datatype(exttype);
    return module;
}

pbl_module *pbl_module_add_range(pbl_module *module, pbl_rng *rng)
{
    module->add_range(rng);
    return module;
}

void pbl_module_free(pbl_module *module)
{
    delete module;
}

} // namespace parameterised
} // namesapce scribble
