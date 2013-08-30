// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/pbes_input_output_tool.h
/// \brief Base class for tools that produce a (P)BES as output.

#ifndef MCRL2_UTILITIES_PBES_INPUT_OUTPUT_TOOL_H
#define MCRL2_UTILITIES_PBES_INPUT_OUTPUT_TOOL_H

#include <set>
#include <string>
#include <iostream>
#include <stdexcept>
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/pbes_output_tool.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for filter tools that take a pbes as input and
//         produce a pbes as output.
/// \pre Tool provides input_filename() and output_filename()
template <typename Tool>
class pbes_input_output_tool: public pbes_input_tool<pbes_output_tool<Tool> >
{
  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    pbes_input_output_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : pbes_input_tool<pbes_output_tool<Tool> >(name, author, what_is, tool_description, known_issues)
    {}
};

/// \brief Base class for filter tools that take a bes as input and
//         produce a bes as output.
/// \pre Tool provides input_filename() and output_filename()
template <typename Tool>
class bes_input_output_tool: public bes_input_tool<bes_output_tool<Tool> >
{
  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    bes_input_output_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : bes_input_tool<bes_output_tool<Tool> >(name, author, what_is, tool_description, known_issues)
    {}
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_PBES_INPUT_OUTPUT_TOOL_H
