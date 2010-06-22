// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/mcrl2_gui_tool.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_MCRL2_GUI_TOOL_H
#define MCRL2_UTILITIES_MCRL2_GUI_TOOL_H

#include <map>
#include <string>

namespace mcrl2 {

namespace utilities {

  /// \brief Base class for the mcrl2 gui tools.
  template <typename Tool>
  class mcrl2_gui_tool: public Tool
  {
    protected:
      std::map<std::string, std::string> m_gui_options;
      bool m_gui_options_selected;

      /// \brief Add options to an interface description. Also includes
      /// rewriter options.
      /// \param desc An interface description
      void add_options(interface_description& desc)
      {
        Tool::add_options(desc);
        desc.add_option("mcrl2-gui",
          "outputs information about the visual representation of this option in the mCRL2 GUI")
        ;
      }

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      void parse_options(const command_line_parser& parser)
      {
        Tool::parse_options(parser);
        m_gui_options_selected = parser.options.count("mcrl2-gui") > 0;
      }     

      void print_mcrl2_gui_options() const
      {
        for (std::map<std::string, std::string>::const_iterator i = m_gui_options.begin(); i != m_gui_options.end(); ++i)
        {
          std::cout << i->first << " -> " << i->second << std::endl;
        }
      }

    public:
      mcrl2_gui_tool()
        : m_gui_options_selected(false)
      {
        m_gui_options["help"]    = "unknown";
        m_gui_options["debug"]   = "unknown";
        m_gui_options["verbose"] = "unknown";
      }
      
      bool run()
      {
        if (m_gui_options_selected)
        {
          print_mcrl2_gui_options();
          return true;
        }
        Tool::run();
      }      
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_MCRL2_GUI_TOOL_H
