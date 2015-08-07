#include <scribble/import.h>

namespace scribble {

sbl_import *sbl_mk_import(char *name)
{
    return new Import(name);
}

sbl_import *sbl_mk_import_full(char *name, char *from, char *as)
{
    return new Import(name, from ,as);
}

} // namespace scribble
