#include <string>
#include <scribble/parameterised/range.h>

namespace scribble {
namespace parameterised {

pbl_rng *pbl_mk_rng(char *name, unsigned int from, unsigned int to)
{
    return new Range(name, from, to);
}

const char *pbl_rng_get_name(pbl_rng *rng)
{
    return rng->name().c_str();
}

unsigned int pbl_rng_get_from(pbl_rng *rng)
{
    return rng->from();
}

unsigned int pbl_rng_get_to(pbl_rng *rng)
{
    return rng->to();
}

void pbl_rng_free(pbl_rng *rng)
{
    delete rng;
}

} // namespace parameterised
} // namespace scribble
