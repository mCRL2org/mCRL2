///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/lps/detail/string_utility.h
/// Add your file description here.
//
//  Copyright 2007 Wieger Wesselink. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPS_STRING_UTILITY_H
#define LPS_STRING_UTILITY_H

#include <string>

namespace lps {
  inline
  std::string unquote(const std::string& s)
  {
    if (s.size() == 0) return s;
    std::string::size_type pos = s.find_first_not_of("\'\"");
    std::string::size_type end_pos = s.find_last_not_of("\'\"");
    return s.substr(pos, end_pos - pos + 1);
  }
} // namespace lps

#endif // LPS_STRING_UTILITY_H
