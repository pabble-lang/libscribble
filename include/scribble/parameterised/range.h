#ifndef SCRIBBLE__PARAMETERISED__RANGE_H__
#define SCRIBBLE__PARAMETERISED__RANGE_H__

#ifdef __cplusplus
#include <string>
#endif

#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
class Range : public util::SrcLocatable {
    std::string name_;
    unsigned int from_;
    unsigned int to_;

  public:
    Range(std::string name, unsigned int from, unsigned int to)
        : util::SrcLocatable(util::SrcLocation(), util::SrcLocation()),
          name_(name), from_(from), to_(to) { }

    ~Range() { }

    std::string name() const { return name_; }

    unsigned int from() const { return from_; }

    unsigned int to() const { return to_; }
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Range pbl_rng;
#else
typedef struct Range pbl_rng;
#endif

pbl_rng *pbl_mk_rng(char *name, unsigned int from, unsigned int to);

const char *pbl_rng_get_name(pbl_rng *rng);

unsigned int pbl_rng_get_from(pbl_rng *rng);

unsigned int pbl_rng_get_to(pbl_rng *rng);

void pbl_rng_free(pbl_rng *rng);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__RANGE_H__
