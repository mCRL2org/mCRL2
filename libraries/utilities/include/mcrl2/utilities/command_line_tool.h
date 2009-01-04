// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/command_line_tool.h
/// \brief Base class for command line tools.

#ifndef MCRL2_UTILITIES_COMMAND_LINE_TOOL_H
#define MCRL2_UTILITIES_COMMAND_LINE_TOOL_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for command line tools.
  /// as result.
  class command_line_tool
  {
    protected:
      /// The name of the tool
      std::string m_name;

      /// The name of the developer(s)
      std::string m_author;

      /// The description of the tool
      std::string m_tool_description;

      /// \brief Add options to an interface description.
      /// \param desc An interface description
      virtual void add_options(interface_description& desc)
      {}

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      virtual void parse_options(const command_line_parser& parser)
      {
      }

      /// \brief Checks if the number of positional options is OK.
      /// By default this function does nothing.
      /// \param parser PARAM_DESCRIPTION
      virtual void check_positional_options(const command_line_parser& parser)
      {}

      /// \brief Returns the synopsis of the tool
      /// \return The string "[OPTION]...\n"
      virtual std::string synopsis() const
      {
        return "[OPTION]...\n";
      }

    public:
      /// \brief Constructor.
      command_line_tool(const std::string& name,
                  const std::string& author,
                  const std::string& tool_description
                 )
        : m_name            (name),
          m_author          (author),
          m_tool_description(tool_description)
      {
      }

      /// \brief Destructor.
      virtual ~command_line_tool()
      {}

      /// \brief Run the tool. The options must be set manually.
      /// \return True if the tool execution was successful.
      virtual bool run() = 0;

      /// \brief Run the tool with the given command line options.
      /// \param argc Number of command line arguments
      /// \param argv Command line arguments
      /// \return The execution result
      int execute(int argc, char* argv[])
      {
        try {
          interface_description clinterface(argv[0], m_name, m_author, synopsis(), m_tool_description);
          add_options(clinterface);
          command_line_parser parser(clinterface, argc, argv);
          check_positional_options(parser);
          parse_options(parser);
	        if (!parser.continue_execution())
	        {
	          return EXIT_SUCCESS;
          }
        }
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
          return EXIT_FAILURE;
        }
        if (run())
        {
          return EXIT_SUCCESS;
        }
        return EXIT_FAILURE;
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_COMMAND_LINE_TOOL_H
