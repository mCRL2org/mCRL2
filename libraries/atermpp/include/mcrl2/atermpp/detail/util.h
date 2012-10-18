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
  oss << t.address();
  return oss.str();
} 


} // namespace atermpp

#endif
