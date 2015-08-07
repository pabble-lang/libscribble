#ifndef SCRIBBLE__PARAMETERISED__CONST_H__
#define SCRIBBLE__PARAMETERISED__CONST_H__

#ifdef __cplusplus
#include <string>
#endif

#include <sesstype/parameterised/const.h>

#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
class Constant : public util::SrcLocatable {
  public:
    Constant()
        : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) {}
    virtual std::string name() const = 0;
};

class ValueConstant
    : public sesstype::parameterised::ValueConstant,
      public scribble::parameterised::Constant {
  public:
    ValueConstant(std::string name, unsigned int value)
        : sesstype::parameterised::ValueConstant(name, value),
          scribble::parameterised::Constant() { }

    virtual std::string name() const override { return ValueConstant::name(); }
};

class BoundedConstant
    : public sesstype::parameterised::BoundedConstant,
      public scribble::parameterised::Constant {
  public:
    BoundedConstant(std::string name, unsigned int lbound, unsigned int ubound)
        : sesstype::parameterised::BoundedConstant(name, lbound, ubound),
          scribble::parameterised::Constant() { }

    virtual std::string name() const override { return BoundedConstant::name(); }
};

class ScalableConstant
    : public sesstype::parameterised::ScalableConstant,
      public scribble::parameterised::Constant {
  public:
    ScalableConstant(std::string name, unsigned int lbound)
        : sesstype::parameterised::ScalableConstant(name, lbound),
           scribble::parameterised::Constant() { }

    virtual std::string name() const override { return ScalableConstant::name(); }
};
#endif // __cplusplus

#ifdef __cplusplus
typedef Constant pbl_const;
#else
typedef struct Constant pbl_const;
#endif

#ifdef __cplusplus
extern "C" {
#endif

pbl_const *pbl_mk_val_const(char *name, unsigned int val);

pbl_const *pbl_mk_rng_const(char *name, unsigned int lbound, unsigned int ubound);

pbl_const *pbl_mk_inf_const(char *name, unsigned int lbound);

const char *pbl_const_get_name(pbl_const *con);

int pbl_const_get_type(pbl_const *con);

unsigned int pbl_val_const_get_val(pbl_const *con);

unsigned int pbl_rng_const_get_lbound(pbl_const *con);

unsigned int pbl_rng_const_get_ubound(pbl_const *con);

unsigned int pbl_inf_const_get_lbound(pbl_const *con);

void pbl_const_free(pbl_const *con);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__CONST_H__
