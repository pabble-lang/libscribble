#include <scribble/parameterised/const.h>

namespace scribble {
namespace parameterised {

pbl_const *pbl_mk_val_const(char *name, unsigned int val)
{
    return new ValueConstant(name, val);
}

pbl_const *pbl_mk_rng_const(char *name, unsigned int lbound, unsigned int ubound)
{
    return new BoundedConstant(name, lbound, ubound);
}

pbl_const *pbl_mk_inf_const(char *name, unsigned int lbound)
{
    return new ScalableConstant(name, lbound);
}

const char *pbl_const_get_name(pbl_const *con)
{
    return con->name().c_str();
}

int pbl_const_get_type(pbl_const *con)
{
    if (sesstype::parameterised::Constant *base_constant = dynamic_cast<sesstype::parameterised::Constant *>(con)) {
        return base_constant->type();
    }
    return -1;
}

unsigned int pbl_val_const_get_val(pbl_const *con)
{
    if (ValueConstant *valconst = dynamic_cast<ValueConstant *>(con)) {
        return valconst->value();
    }
    return -1;
}

unsigned int pbl_rng_const_get_lbound(pbl_const *con)
{
    if (BoundedConstant *rngconst = dynamic_cast<BoundedConstant *>(con)) {
        return rngconst->lbound();
    }
    return -1;
}

unsigned int pbl_rng_const_get_ubound(pbl_const *con)
{
    if (BoundedConstant *rngconst = dynamic_cast<BoundedConstant *>(con)) {
        return rngconst->ubound();
    }
    return -1;
}

unsigned int pbl_inf_const_get_lbound(pbl_const *con)
{
    if (ScalableConstant *infconst = dynamic_cast<ScalableConstant *>(con)) {
        return infconst->lbound();
    }
    return -1;
}

void pbl_const_free(pbl_const *con)
{
    delete con;
}

} // namespace parameterised
} // namespace scribble
