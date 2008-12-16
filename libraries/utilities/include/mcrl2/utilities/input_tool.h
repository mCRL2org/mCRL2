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

#include "mcrl2/utilities/command_line_tool.h"

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for tools that take a file as input.
  class input_tool: public command_line_tool
  {
    protected:
      /// The input file name
      std::string m_input_filename;

      /// Returns the synopsis of the tool
      /// \return The synopsis of the tool
      std::string synopsis() const
      {
        return "[OPTION]... [INFILE]\n";
      }

      /// \brief Parse command line options
      /// \param argc Number of command line arguments
      /// \param argv Command line arguments
      bool parse_options(int argc, char* argv[])
      {
        interface_description clinterface(argv[0], m_name, m_author, synopsis(), m_tool_description);
        add_options(clinterface);
        command_line_parser parser(clinterface, argc, argv);
        if (parser.continue_execution())
        {
          if (0 < parser.arguments.size())
          {
            m_input_filename = parser.arguments[0];
          }
          if (1 < parser.arguments.size())
          {
            parser.error("too many file arguments");
          }
          command_line_tool::parse_options(parser);
        }

        return parser.continue_execution();
      }

    public:
      /// \brief Constructor.
      input_tool(const std::string& name,
                 const std::string& author,
                 const std::string& tool_description
                )
        : command_line_tool(name, author, tool_description)
      {
      }

      /// \brief Sets the input filename.
      /// \param filename The name of a file.
      /// \deprecated
      void set_input_filename(const std::string& filename)
      {
        m_input_filename = filename;
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_INPUT_TOOL_H
