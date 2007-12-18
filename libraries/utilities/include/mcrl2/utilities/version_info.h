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

/**
 * \brief toolset version string
 **/
#define MCRL2_VERSION "January 2008"

/**
 * \brief Gets a string that describes the toolset version information
 * \param[in] tool_name the name of the tool
 **/
inline std::string get_version_information(std::string const& tool_name) {
  std::string text(tool_name);

  return text.append(" " MCRL2_VERSION " (revision " MCRL2_REVISION ")");
}

/**
 * \brief Prints toolset version information
 * \param[in] tool_name the name of the tool
 **/
inline void print_version_information(std::string const& tool_name) {
  std::cerr << get_version_information(tool_name) << std::endl;
}

#endif //MCRL2_UTILITIES_VERSION_INFO_H
