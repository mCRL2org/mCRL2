// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_INPUT_INPUT_OUTPUT_TOOL_H
#define MCRL2_UTILITIES_INPUT_INPUT_OUTPUT_TOOL_H

#include "mcrl2/utilities/input_tool.h"

#include <sstream>

namespace mcrl2
{

namespace utilities
{

namespace tools
{

/// \brief Base class for tools that take one file as input, and writes the results
///        to two files.
class input_output_output_tool : public input_tool
{
protected:
  /// The output file names.
  std::string m_output_filename1;
  std::string m_output_filename2;

  /// \brief Checks if the number of positional options is OK.
  /// \param parser A command line parser
  void check_positional_options(const command_line_parser& parser)
  {
    if (3 < parser.arguments.size())
    {
      parser.error("too many file arguments");
    }
  }

  /// \brief Returns the synopsis of the tool.
  std::string synopsis() const
  {
    return "[OPTION]... INFILE [OUTFILE1 [OUTFILE2]]\n";
  }

  /// \brief Parse non-standard options
  /// \param parser A command line parser
  void parse_options(const command_line_parser& parser)
  {
    input_tool::parse_options(parser);
    if (parser.arguments.size() <= 2)
    {
      parser.error("This tool requires at least an input and two output filenames.");
    }

    m_output_filename1 = parser.arguments[1];
    m_output_filename2 = parser.arguments[2];
  }

  /// \brief Returns a message about the output filename
  std::string output_file_message() const
  {
    std::ostringstream out;
    out << "Output written to " << m_output_filename1 << " and " << m_output_filename2;
    return out.str();
  }

  /// \brief Adds a message about input and output files to the given description.
  std::string make_tool_description(const std::string& description) const
  {
    return description + " If INFILE is not present, standard input is used.";
  }

public:
  /// \brief Constructor.
  input_output_output_tool(const std::string& name,
                           const std::string& author,
                           const std::string& what_is,
                           const std::string& tool_description,
                           std::string known_issues = ""
                         )
    : input_tool(name, author, what_is, tool_description, known_issues)
  {}

  /// \brief Returns a const reference to the first output filename.
  const std::string& output_filename1() const { return m_output_filename1; }
  std::string& output_filename1() { return m_output_filename1; }

  /// \brief Returns a const reference to the second output filename.
  const std::string& output_filename2() const { return m_output_filename2; }
  std::string& output_filename2() { return m_output_filename2; }
};

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_INPUT_INPUT_OUTPUT_TOOL_H
