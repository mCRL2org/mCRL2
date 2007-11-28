// Author(s): Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file version_info.h
/// \brief routines to print version information of mCRL2 tools

#include <string>
#include <iostream>

#define MCRL2_VERSION "July 2007"

std::string get_version_information(const std::string tool_name) {
  std::string text = tool_name;
  text.append(" ");
  text.append(MCRL2_VERSION);
  text.append(" (revision ");
  text.append(MCRL2_REVISION);
  text.append(")");
  return text;
}

void print_version_information(const std::string tool_name) {
  std::cerr << get_version_information(tool_name) << std::endl;
}
