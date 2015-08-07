#ifndef SCRIBBLE__MODULE_H__
#define SCRIBBLE__MODULE_H__

#ifdef __cplusplus
#include <string>
#include <unordered_map>
#endif

#include <sesstype/module.h>

#include "scribble/ext_type.h"
#include "scribble/import.h"
#include "scribble/protocol.h"
#include "scribble/util/srclocation.h"

#ifdef __cplusplus
namespace scribble {
#endif

#ifdef __cplusplus
class Module : public sesstype::Module, util::SrcLocatable {
    std::unordered_map<std::string, ExternalType *> datatypes_;

  public:
    Module()
        : sesstype::Module(),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()),
          datatypes_() { }

    Module(std::string name)
        : sesstype::Module(name),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()),
          datatypes_() { }

    ~Module()
    {
        for (auto name_datatype : datatypes_) {
            delete name_datatype.second;
        }
        datatypes_.clear();
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
};
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef class Module sbl_module;
#else
typedef struct Module sbl_module;
#endif

sbl_module *sbl_mk_module_init();

sbl_module *sbl_mk_module(char *name);

sbl_module *sbl_module_add_protocol(sbl_module *module, sbl_protocol *protocol);

sbl_module *sbl_module_add_import(sbl_module *module, sbl_import *import);

sbl_module *sbl_module_add_datatype(sbl_module *module, sbl_exttype *datatype);

void sbl_module_free(sbl_module *module);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif


#endif//SCRIBBLE__MODULE_H__
