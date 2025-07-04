// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/xinput_tool.h
/// \brief Base class for tools that take an x number of files as input.

#ifndef MCRL2_UTILITIES_XINPUT_TOOL_H
#define MCRL2_UTILITIES_XINPUT_TOOL_H

#include "mcrl2/utilities/tool.h"

namespace mcrl2::utilities::tools
{

/// \brief Base class for tools that take an x number of files as input.
class xinput_tool: public tool
{
  protected:
    /// The input file names
    std::vector<std::string> m_input_filenames;

    // The minimum and maximum number of files
    std::size_t min_input_files;
    std::size_t max_input_files;

    /// \brief Checks if the number of positional options is OK.
    /// \param parser A command line parser
    void check_positional_options(const command_line_parser& parser) override
    {
      if (max_input_files > 0 && max_input_files < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
      else if (min_input_files > parser.arguments.size())
      {
        parser.error("too few file arguments");
      }
    }

    /// \brief Returns the synopsis of the tool
    /// \return The string "[OPTION]... [INFILE]\n"
    std::string synopsis() const override
    {
      return "[OPTION]... [INFILES]\n";
    }

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser) override
    {
      tool::parse_options(parser);

      // Read input filenames from the given arguments
      for (auto& input : parser.arguments)
      {
        m_input_filenames.push_back(input);
      }
    }

    /// \brief Returns a message about the input filename
    std::string input_file_message() const
    {
      std::ostringstream out;
      out << "Input read from ";
      for (auto& filename : m_input_filenames)
      {
        out << "'" + filename + "', ";
      }
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
    xinput_tool(const std::string& name,
                const std::string& author,
                const std::string& what_is,
                const std::string& tool_description,
                std::string known_issues = "",
                uint16_t min = 0,
                uint16_t max = 0
              )
      : tool(name, author, what_is, tool_description, known_issues)
    {
      min_input_files = min;
      max_input_files = max;
    }

    /// \brief Returns a const reference to the input filenames.
    const std::vector<std::string>& input_filenames() const
    {
      return m_input_filenames;
    }

    /// \brief Returns a reference to the input filenames.
    std::vector<std::string>& input_filenames()
    {
      return m_input_filenames;
    }
};

} // namespace mcrl2::utilities::tools

#endif // MCRL2_UTILITIES_XINPUT_TOOL_H
