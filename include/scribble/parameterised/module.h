#ifndef SCRIBBLE__PARAMETERISED__MODULE_H__
#define SCRIBBLE__PARAMETERISED__MODULE_H__

#ifdef __cplusplus
#include <string>
#include <unordered_map>
#endif

#include <sesstype/parameterised/module.h>
#include <sesstype/parameterised/const.h>

#include "scribble/parameterised/const.h"
#include "scribble/parameterised/ext_type.h"
#include "scribble/parameterised/protocol.h"
#include "scribble/parameterised/range.h"
#include "scribble/parameterised/import.h"

#ifdef __cplusplus
namespace scribble {
namespace parameterised {
#endif

#ifdef __cplusplus
class Module : public sesstype::parameterised::Module {
    std::unordered_map<std::string, Constant *> constants_;
    std::unordered_map<std::string, ExternalType *> datatypes_;
    std::unordered_map<std::string, Range *> ranges_;

  public:
    Module() : sesstype::parameterised::Module() { }

    Module(std::string name) : sesstype::parameterised::Module(name) { }

    Constant *constant(std::string name) const { return constants_.at(name); }

    unsigned int num_constants() const { return constants_.size(); }

    bool has_constant(std::string name) const
    {
        return (constants_.find(name) != constants_.end());
    }

    void add_constant(Constant *constant)
    {
        constants_.insert({ constant->name(), constant });
    }

    ExternalType *datatype(std::string name) const
    {
        return datatypes_.at(name);
    }

    unsigned int num_datatypes() const { return datatypes_.size(); }

    bool has_datatype(std::string name) const
    {
        return (datatypes_.find(name) != datatypes_.end());
    }

    void add_datatype(ExternalType *datatype)
    {
        datatypes_.insert({ datatype->name(), datatype });
    }

    Range *range(std::string name) const { return ranges_.at(name); }

    /// \returns number of Range in Module.
    unsigned int num_ranges() const { return ranges_.size(); }

    /// \brief Test if Range is in Module.
    /// \returns true if <tt>name</tt> is a Range in Module.
    bool has_range(std::string name) const
    {
        return (ranges_.find(name) != ranges_.end());
    }

    /// \param[in] range to Module.
    void add_range(Range *range)
    {
        ranges_.insert({ range->name(), range });
    }
};
#endif // __cplusplus

#ifdef __cplusplus
typedef Module pbl_module;
#else
typedef struct Module pbl_module;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

pbl_module *pbl_mk_module_init();

pbl_module *pbl_mk_module(char *name);

pbl_module *pbl_module_add_protocol(pbl_module *module, pbl_protocol *protocol);

pbl_module *pbl_module_add_import(pbl_module *module, pbl_import *import);

pbl_module *pbl_module_add_const(pbl_module *module, pbl_const *constant);

pbl_module *pbl_module_add_exttype(pbl_module *module, pbl_exttype *exttype);

pbl_module *pbl_module_add_range(pbl_module *module, pbl_rng *rng);

void pbl_module_free(pbl_module *module);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace parameterised
} // namespace scribble
#endif

#endif//SCRIBBLE__PARAMETERISED__MODULE_H__
