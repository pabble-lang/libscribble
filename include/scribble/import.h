#ifndef SCRIBBLE__IMPORT_H__
#define SCRIBBLE__IMPORT_H__

#include <sesstype/import.h>
#include <scribble/util/srclocation.h>

#ifdef __cplusplus
namespace scribble {
#endif

#ifdef __cplusplus
class Import : public sesstype::Import, public util::SrcLocatable {
  public:
    Import(std::string name)
        : sesstype::Import(name),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }

    Import(std::string name, std::string from, std::string as)
        : sesstype::Import(name, from, as),
          util::SrcLocatable(util::SrcLocation(), util::SrcLocation()) { }
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
typedef Import sbl_import;
#else
typedef struct Import sbl_import;
#endif

sbl_import *sbl_mk_import(char *name);

sbl_import *sbl_mk_import_full(char *name, char *from, char *as);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
} // namespace scribble
#endif

#endif//SCRIBBLE__IMPORT_H__
