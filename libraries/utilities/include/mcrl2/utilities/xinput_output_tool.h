// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/xinput_tool.h
/// \brief Base class for tools that take an x number of files as input and one output file.

#ifndef MCRL2_UTILITIES_XINPUT_OUTPUT_TOOL_H
#define MCRL2_UTILITIES_XINPUT_OUTPUT_TOOL_H

#include "mcrl2/utilities/xinput_tool.h"

namespace mcrl2::utilities::tools
{

/// \brief Base class for tools that take an x number of files as input and one output file.
class xinput_output_tool: public xinput_tool
{
  protected:
    /// The output file name
    std::string m_output_filename;

    /// \brief Checks if the number of positional options is OK.
    /// \param parser A command line parser
    void check_positional_options(const command_line_parser& parser) override
    {
      if (max_input_files > 0 && max_input_files + 1 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
      else if (min_input_files + 1 > parser.arguments.size())
      {
        parser.error("too few file arguments");
      }
    }

    /// \brief Returns the synopsis of the tool
    /// \return The string "[OPTION]... [INFILE]\n"
    std::string synopsis() const override
    {
      return "[OPTION]... [INFILES [OUTFILE]]\n";
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser) override
    {
      // Read input filenames from arguments list
      xinput_tool::parse_options(parser);

      // Remove last input filename and set as output filename
      m_output_filename = xinput_tool::input_filenames().back();
      xinput_tool::input_filenames().pop_back();
    }

    /// \brief Returns a message about the input filename
    std::string output_file_message() const
    {
      std::ostringstream out;
      out << "Output written to "
          << ((m_output_filename.empty()) ? "standard output" : ("'" + m_output_filename + "'"));
      return out.str();
    }

    /// \brief Adds a message about input files to the given description.
    std::string make_tool_description(const std::string& description) const
    {
      // return utilities::word_wrap_text(description + " If INFILE is not present, standard input is used.");
      return description;
    }

  public:
    /// \brief Constructor.
    xinput_output_tool(const std::string& name,
                const std::string& author,
                const std::string& what_is,
                const std::string& tool_description,
                std::string known_issues = "",
                uint16_t min = 0,
                uint16_t max = 0
              )
        : xinput_tool(name, author, what_is, tool_description, known_issues, min, max)
    {
    }

    /// \brief Returns a const reference to the output filename.
    const std::string& output_filename() const { return m_output_filename; }

    /// \brief Returns a reference to the output filename.
    std::string& output_filename() { return m_output_filename; }
};

} // namespace mcrl2::utilities::tools

#endif // MCRL2_UTILITIES_XINPUT_OUTPUT_TOOL_H
