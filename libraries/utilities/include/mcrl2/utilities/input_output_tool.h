// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/input_output_tool.h
/// \brief Base class for tools that take a file as input, and write the results to a file.

#ifndef MCRL2_UTILITIES_INPUT_OUTPUT_TOOL_H
#define MCRL2_UTILITIES_INPUT_OUTPUT_TOOL_H

#include "mcrl2/utilities/input_tool.h"

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for tools that take a file as input,
  /// and write the results to a file.
  class input_output_tool: public input_tool
  {
    protected:
      /// The output file name
      std::string m_output_filename;

      /// \brief Checks if the number of positional options is OK.
      /// \param parser A command line parser
      void check_positional_options(const command_line_parser& parser)
      {
        if (2 < parser.arguments.size())
        {
          parser.error("too many file arguments");
        }
      }

      /// Returns the synopsis of the tool
      /// \return The synopsis of the tool
      std::string synopsis() const
      {
        return "[OPTION]... [INFILE [OUTFILE]]\n";
      }

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      void parse_options(const command_line_parser& parser)
      {
      	input_tool::parse_options(parser);
        if (1 < parser.arguments.size())
        {
          m_output_filename = parser.arguments[1];
        }
      }

    public:
      /// \brief Constructor.
      input_output_tool(const std::string& name,
                 const std::string& author,
                 const std::string& tool_description
                )
        : input_tool(name, author, tool_description)
      {
      }

      /// \brief Sets the output filename.
      /// \param filename The name of a file.
      /// \deprecated
      void set_output_filename(const std::string& filename)
      {
        m_output_filename = filename;
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_INPUT_OUTPUT_TOOL_H
