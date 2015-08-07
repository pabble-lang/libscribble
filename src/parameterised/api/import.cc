#include <scribble/parameterised/import.h>

namespace scribble {
namespace parameterised {

pbl_import *pbl_mk_import(char *name)
{
    return new Import(name);
}

pbl_import *pbl_mk_import_full(char *name, char *from, char *as)
{
    return new Import(name, from, as);
}

} // namespace parameterised
} // namespace scribble
