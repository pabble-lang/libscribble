#ifndef SCRIBBLE__UTIL__CLONABLE_H__
#define SCRIBBLE__UTIL__CLONABLE_H__

#ifdef __cplusplus
namespace scribble {
namespace util {
#endif

#ifdef __cplusplus
/**
 * \brief Pure virtual class for clonable classes.
 */
class Clonable {
  public:
    virtual ~Clonable() { }
    virtual Clonable *clone() const = 0;
};
#endif // __cplusplus

#ifdef __cplusplus
} // namespace util
} // namespace scribble
#endif

#endif//SCRIBBLE__UTIL__CLONABLE_H__
