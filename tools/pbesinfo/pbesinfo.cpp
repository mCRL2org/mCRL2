// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinfo.cpp
/// \brief Tool that displays information about a PBES.

// ======================================================================
//
// file          : pbesinfo
// date          : 11-04-2007
// version       : 0.1.0
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

//MCRL2-specific
#include "mcrl2/utilities/logger.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbesinfo_tool: public pbes_input_tool<input_tool>
{
  protected:
    typedef pbes_input_tool<input_tool> super;

    bool opt_full;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      opt_full = parser.options.count("full") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("full",
                      "display the predicate variables and their signature",
                      'f'
                     )
      ;
    }

  public:
    pbesinfo_tool()
      : super(
        "pbesinfo",
        "Wieger Wesselink; Alexander van Dam",
        "display basic information about a PBES",
        super::make_tool_description("Print basic information about the PBES in INFILE.")
      ),
      opt_full(false)
    {}

    /// If PBES can be loaded from file_name, then
    /// - Show if PBES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run()
    {
      pbesinfo(input_filename(),
               input_file_message(),
               pbes_input_format(),
               opt_full
              );
      return true;
    }
};

class pbesinfo_gui_tool: public mcrl2_gui_tool<pbesinfo_tool>
{
  public:
    pbesinfo_gui_tool()
    {
      m_gui_options["full"] = create_checkbox_widget();
    }
};

int main(int argc, char** argv)
{
  return pbesinfo_gui_tool().execute(argc, argv);
}
