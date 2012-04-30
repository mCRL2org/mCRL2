#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <sstream>

namespace aterm
{

template <typename T>
std::string to_string(const T& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

inline
std::string to_string(const ATerm t)
{
  std::ostringstream oss;
  oss << &*t;
  return oss.str();
} 

#ifndef streq
#  define streq(s,t)  (!(strcmp(s,t)))
#endif

#ifndef MIN
#  define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b) ? (a) : (b))
#endif


} // namespace aterm

#endif
