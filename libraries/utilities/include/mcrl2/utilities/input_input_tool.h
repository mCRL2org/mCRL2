// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/input_input_tool.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_INPUT_INPUT_TOOL_H
#define MCRL2_UTILITIES_INPUT_INPUT_TOOL_H

#include <sstream>
#include "mcrl2/utilities/tool.h"

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for tools that take two files as input.
class input_input_tool: public tool
{
  protected:
    /// The first input file name
    std::string m_input_filename1;

    /// The second input file name
    std::string m_input_filename2;

    /// \brief Checks if the number of positional options is OK.
    /// \param parser A command line parser
    void check_positional_options(const command_line_parser& parser)
    {
      if (2 < parser.arguments.size())
      {
        throw parser.error("too many file arguments");
      }
    }

    /// \brief Returns the synopsis of the tool.
    std::string synopsis() const
    {
      return "[OPTION]... INFILE1 [INFILE2]\n";
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser)
    {
      tool::parse_options(parser);
      if (0 < parser.arguments.size())
      {
        m_input_filename1 = parser.arguments[0];
      }
      if (1 < parser.arguments.size())
      {
        m_input_filename2 = parser.arguments[1];
      }
    }

    /// \brief Returns a message about the input filename
    std::string input_file_message() const
    {
      std::ostringstream out;
      out << "Input read from " << m_input_filename1 << " and " << ((m_input_filename2.empty())? "standard input" : ("'" + m_input_filename2 + "'"));
      return out.str();
    }

    /// \brief Adds a message about input and output files to the given description.
    std::string make_tool_description(const std::string& description) const
    {
      return description + " If INFILE2 is not present, standard input is used.";
    }

  public:
    /// \brief Constructor.
    input_input_tool(const std::string& name,
                     const std::string& author,
                     const std::string& what_is,
                     const std::string& tool_description,
                     std::string known_issues = ""
                    )
      : tool(name, author, what_is, tool_description, known_issues)
    {
    }

    /// \brief Returns a const reference to the first input filename.
    const std::string& input_filename1() const
    {
      return m_input_filename1;
    }

    /// \brief Returns a reference to the first input filename.
    std::string& input_filename1()
    {
      return m_input_filename1;
    }

    /// \brief Returns a const reference to the second input filename.
    const std::string& input_filename2() const
    {
      return m_input_filename2;
    }

    /// \brief Returns a reference to the first second filename.
    std::string& input_filename2()
    {
      return m_input_filename2;
    }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_INPUT_INPUT_TOOL_H
