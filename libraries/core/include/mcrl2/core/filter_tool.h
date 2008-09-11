// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/filter_tool.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_FILTER_TOOL_H
#define MCRL2_CORE_FILTER_TOOL_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2 {

namespace core {

  class filter_tool
  {
    protected:
      std::string m_name;
      std::string m_author;
      std::string m_tool_description;
      std::string m_input_filename;
      std::string m_output_filename;
  
      void parse_options(int argc, char* argv[])
      {
        utilities::interface_description clinterface(argv[0], m_name, m_author, "[OPTION]... [INFILE [OUTFILE]]\n", m_tool_description);
        add_options(clinterface);
        utilities::command_line_parser parser(clinterface, argc, argv);
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
        
      }
  
      /// run the tool
      virtual void run() = 0;   
  
      /// add additional options
      virtual void add_options(utilities::interface_description& desc)
      {}
  
      /// parse non-standard options
      virtual void parse_options(const utilities::command_line_parser& parser)
      {}
    
    public:
      filter_tool(const std::string& name,
                  const std::string& author,
                  const std::string& tool_description
                 )
        : m_name            (name),
          m_author          (author),        
          m_tool_description(tool_description)
      {
      }

      virtual ~filter_tool()
      {}

      int execute(int argc, char* argv[])
      {
        try {
          parse_options(argc, argv);
          run();
          return EXIT_SUCCESS;
        }
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
          return EXIT_FAILURE;
        }
      }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_FILTER_TOOL_H
