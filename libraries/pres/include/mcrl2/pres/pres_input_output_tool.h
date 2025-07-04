// Author(s): Jan Friso Groote. Based on pbes/pbes_input_output_tool.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pres_input_output_tool.h
/// \brief Base class for tools that produce a (P)BES as output.

#ifndef MCRL2_PRES_PRES_INPUT_OUTPUT_TOOL_H
#define MCRL2_PRES_PRES_INPUT_OUTPUT_TOOL_H

#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/pres/pres_output_tool.h"

namespace mcrl2::pres_system::tools
{

/// \brief Base class for filter tools that take a pres as input and
//         produce a pres as output.
/// \pre Tool provides input_filename() and output_filename()
template <typename Tool>
class pres_input_output_tool: public pres_input_tool<pres_output_tool<Tool> >
{
  public:
    /// \brief Constructor.
    /// \param name The name of the tool
    /// \param author The author(s) of the tool
    /// \param what_is One-line "what is" description of the tool
    /// \param tool_description The description of the tool
    /// \param known_issues Known issues with the tool
    pres_input_output_tool(const std::string& name,
                       const std::string& author,
                       const std::string& what_is,
                       const std::string& tool_description,
                       std::string known_issues = ""
                      )
      : pres_input_tool<pres_output_tool<Tool> >(name, author, what_is, tool_description, known_issues)
    {}
};

} // namespace mcrl2::pres_system::tools

#endif // MCRL2_PRES_PRES_INPUT_OUTPUT_TOOL_H
