// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/input_tool.h
/// \brief Base class for tools that take a file as input.

#ifndef MCRL2_UTILITIES_INPUT_TOOL_H
#define MCRL2_UTILITIES_INPUT_TOOL_H

#include <sstream>
// #include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/tool.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for tools that take a file as input.
class input_tool: public tool
{
  protected:
    /// The input file name
    std::string m_input_filename;

    /// \brief Checks if the number of positional options is OK.
    /// \param parser A command line parser
    void check_positional_options(const command_line_parser& parser)
    {
      if (1 < parser.arguments.size())
      {
        throw parser.error("too many file arguments");
      }
    }

    /// \brief Returns the synopsis of the tool
    /// \return The string "[OPTION]... [INFILE]\n"
    std::string synopsis() const
    {
      return "[OPTION]... [INFILE]\n";
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser)
    {
      tool::parse_options(parser);
      if (0 < parser.arguments.size())
      {
        m_input_filename = parser.arguments[0];
      }
    }

    /// \brief Returns a message about the input filename
    std::string input_file_message() const
    {
      std::ostringstream out;
      out << "Input read from " << ((m_input_filename.empty())? "standard input" : ("'" + m_input_filename + "'"));
      return out.str();
    }

    /// \brief Adds a message about input files to the given description.
    std::string make_tool_description(const std::string& description) const
    {
      // return utilities::word_wrap_text(description + " If INFILE is not present, standard input is used.");
      return description + " If INFILE is not present, standard input is used.";
    }

  public:
    /// \brief Constructor.
    input_tool(const std::string& name,
               const std::string& author,
               const std::string& what_is,
               const std::string& tool_description,
               std::string known_issues = ""
              )
      : tool(name, author, what_is, tool_description, known_issues)
    {
    }

    /// \brief Returns a const reference to the input filename.
    const std::string& input_filename() const
    {
      return m_input_filename;
    }

    /// \brief Returns a reference to the input filename.
    std::string& input_filename()
    {
      return m_input_filename;
    }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_INPUT_TOOL_H
