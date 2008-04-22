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
#define MCRL2_VERSION "July 2008 (development)"
#define MCRL2_COPYRIGHT_YEAR "2008"

/**
 * \brief Gets a string that describes the toolset version information
 * \param[in] tool_name the name of the tool
 * \param[in] tool_author the author of the tool
 **/
inline std::string get_version_information(const std::string &tool_name, const std::string &tool_author)
{
  std::string text(tool_name);
  text.append(" " MCRL2_VERSION " (revision " MCRL2_REVISION ")\n");
  text.append("Copyright (C) " MCRL2_COPYRIGHT_YEAR " Eindhoven University of Technology.\n");
  text.append("This is free software.  You may redistribute copies of it under the\n");
  text.append("terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.\n");
  text.append("There is NO WARRANTY, to the extent permitted by law.\n");
  text.append("\n");
  text.append("Written by " + tool_author + ".\n");
  return text;
}

#endif //MCRL2_UTILITIES_VERSION_INFO_H
