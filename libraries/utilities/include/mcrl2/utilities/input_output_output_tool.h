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

namespace mcrl2::utilities::tools
{

/// \brief Base class for tools that optionally takes one file as input, and writes the results
///        to two output files.
class input_output_output_tool: public tool
{
protected:
  /// The input file name
  std::string m_input_filename;

  /// The output file names.
  std::string m_output_filename1;
  std::string m_output_filename2;

  /// \brief Checks if the number of positional options is OK.
  /// \param parser A command line parser
  void check_positional_options(const command_line_parser& parser) override
  {
    if (3 < parser.arguments.size())
    {
      parser.error("too many file arguments");
    }
  }

  /// \brief Returns the synopsis of the tool.
  std::string synopsis() const override { return "[OPTION]... [INFILE] OUTFILE1 OUTFILE2\n"; }

  /// \brief Parse non-standard options
  /// \param parser A command line parser
  void parse_options(const command_line_parser& parser) override
  {
    tool::parse_options(parser);

    if (parser.arguments.size() < 2)
    {
      parser.error("This tool requires at least two output filenames.");
    }

    // Take the input filename if there are at least three file arguments.
    std::size_t index = 0;
    if (parser.arguments.size() >= 3)
    {
      m_input_filename = parser.arguments[index];
      ++index;
    }

    m_output_filename1 = parser.arguments[index];
    ++index;
    m_output_filename2 = parser.arguments[index];
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
    : tool(name, author, what_is, tool_description, known_issues)
  {}

  /// \brief Returns a const reference to the input filename.
  const std::string& input_filename() const { return m_input_filename; }
  std::string& input_filename() { return m_input_filename; }

  /// \brief Returns a const reference to the first output filename.
  const std::string& output_filename1() const { return m_output_filename1; }
  std::string& output_filename1() { return m_output_filename1; }

  /// \brief Returns a const reference to the second output filename.
  const std::string& output_filename2() const { return m_output_filename2; }
  std::string& output_filename2() { return m_output_filename2; }
};

} // namespace mcrl2::utilities::tools

#endif // MCRL2_UTILITIES_INPUT_INPUT_OUTPUT_TOOL_H
