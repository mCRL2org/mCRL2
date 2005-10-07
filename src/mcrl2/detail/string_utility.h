#ifndef MCRL2_STRING_UTILITY_H
#define MCRL2_STRING_UTILITY_H

#include <string>

namespace mcrl2 {
  inline
  std::string unquote(const std::string& s)
  {
    if (s.size() == 0) return s;
    std::string::size_type pos = s.find_first_not_of("\'\"");
    std::string::size_type end_pos = s.find_last_not_of("\'\"");
    return s.substr(pos, end_pos - pos + 1);
  }
} // namespace mcrl2

#endif // MCRL2_STRING_UTILITY_H
