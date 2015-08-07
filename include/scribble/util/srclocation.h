/**
 * \file scribble/util/srclocation.h
 * \brief Scribble location information for debugging.
 */
#ifndef SCRIBBLE__UTIL__SRCLOCATION_H__
#define SCRIBBLE__UTIL__SRCLOCATION_H__

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
namespace scribble {
namespace util {
#endif

#ifdef __cplusplus
/**
 * \brief Class representing a location in source.
 */
class SrcLocation {
    std::string file_;
    unsigned int line_, column_;

  public:
    /// \brief SrcLocation constructor with default F:L:C of *:0:0.
    SrcLocation() : file_("*"), line_(0), column_(0) { }

    /// \brief SrcLocation constructor.
    SrcLocation(std::string file, unsigned int line, unsigned int column)
        : file_(file), line_(line), column_(column) { }

    SrcLocation(const SrcLocation &srcloc)
        : file_(srcloc.file_), line_(srcloc.line_), column_(srcloc.column_) { }

    void set_file(std::string file) { file_ = file; }

    std::string file() const { return file_; }

    void set_line(unsigned int line) { line_ = line; }

    unsigned int line() const { return line_; }

    void set_column(unsigned int column) { column_ = column; }

    unsigned int column() const { return column_; }
};

/**
 * \brief Abstract class for adding SrcLocation to ast classes.
 */
class SrcLocatable {
  public:
    SrcLocatable(SrcLocation startloc, SrcLocation endloc)
        : startloc_(startloc), endloc_(endloc) { }

    virtual ~SrcLocatable() { }

    void set_startloc(SrcLocation loc) { startloc_ = loc; }

    SrcLocation startloc() const { return startloc_; }

    void set_endloc(SrcLocation loc) { endloc_ = loc; }

    SrcLocation endloc() const { return endloc_; }

  protected:
    SrcLocation startloc_, endloc_;
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace util
} // namespace scribble
#endif

#endif//SCRIBBLE__UTIL__SRCLOCATION_H__
