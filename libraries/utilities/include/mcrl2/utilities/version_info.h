// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file version_info.h
/// \brief routines to print version information of mCRL2 tools

#ifndef MCRL2_UTILITIES_VERSION_INFO_H
#define MCRL2_UTILITIES_VERSION_INFO_H

#include <string>
#include <iostream>

#define MCRL2_VERSION "January 2008"

inline std::string get_version_information(const std::string tool_name) {
  std::string text = tool_name;
  text.append(" ");
  text.append(MCRL2_VERSION);
  text.append(" (revision ");
  text.append(MCRL2_REVISION);
  text.append(")");
  return text;
}

inline void print_version_information(const std::string tool_name) {
  std::cerr << get_version_information(tool_name) << std::endl;
}

#endif //MCRL2_UTILITIES_VERSION_INFO_H
