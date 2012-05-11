#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <sstream>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

template <typename T>
std::string to_string(const T& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

inline
std::string to_string(const aterm t)
{
  std::ostringstream oss;
  oss << &*t;
  return oss.str();
} 

#ifndef MIN
#  define MIN(a,b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b) ? (a) : (b))
#endif


} // namespace atermpp

#endif
