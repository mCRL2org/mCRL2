// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/filter_tool.h
/// \brief A class that provides common functionality for filter tools.

#ifndef MCRL2_UTILITIES_FILTER_TOOL_H
#define MCRL2_UTILITIES_FILTER_TOOL_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for filter tools that take a file as input and produces a file
  /// as result.
  class filter_tool
  {
    protected:
      /// The name of the tool
      std::string m_name;

      /// The name of the developer(s)
      std::string m_author;

      /// The description of the tool
      std::string m_tool_description;

      /// The input file name
      std::string m_input_filename;

      /// The output file name
      std::string m_output_filename;

      /// \brief Add options to an interface description.
      /// \param desc An interface description
      virtual void add_options(utilities::interface_description& desc)
      {}

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      virtual void parse_options(const utilities::command_line_parser& parser)
      {}

      /// \brief Parse command line options
      /// \param argc Number of command line arguments
      /// \param argv Command line arguments
      bool parse_options(int argc, char* argv[])
      {
        utilities::interface_description clinterface(argv[0], m_name, m_author, "[OPTION]... [INFILE [OUTFILE]]\n", m_tool_description);
        add_options(clinterface);
        utilities::command_line_parser parser(clinterface, argc, argv);
        if (parser.continue_execution())
        {
          if (0 < parser.arguments.size())
          {
            m_input_filename = parser.arguments[0];
          }
          if (1 < parser.arguments.size())
          {
            m_output_filename = parser.arguments[1];
          }
          if (2 < parser.arguments.size())
          {
            parser.error("too many file arguments");
          }
          parse_options(parser);
        }

        return parser.continue_execution();
      }

    public:
      /// \brief Constructor.
      filter_tool(const std::string& name,
                  const std::string& author,
                  const std::string& tool_description
                 )
        : m_name            (name),
          m_author          (author),
          m_tool_description(tool_description)
      {
      }

      /// \brief Destructor.
      virtual ~filter_tool()
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
          if (parse_options(argc, argv))
          {
            if (!run())
            {
              return EXIT_FAILURE;
            }
          }
        }
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
          return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
      }

      /// \brief Sets the input filename.
      /// \param filename The name of a file.
      void set_input_filename(const std::string& filename)
      {
        m_input_filename = filename;
      }

      /// \brief Sets the output filename.
      /// \param filename The name of a file.
      void set_output_filename(const std::string& filename)
      {
        m_output_filename = filename;
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_FILTER_TOOL_H
