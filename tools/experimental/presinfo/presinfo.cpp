// Author(s): Jan Friso Groote. Based on a presinfo.cpp by Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presinfo.cpp
/// \brief Tool that displays information about a PRES.

// ======================================================================
//
// file          : presinfo
// date          : 11-04-2007
// version       : 0.1.0
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================

//C++
#include <exception>

//MCRL2-specific
#include "mcrl2/pres/tools.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/res/pres_input_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::pres_system;
using namespace mcrl2::utilities::tools;
using res::tools::pres_input_tool;

class presinfo_tool: public pres_input_tool<input_tool>
{
  protected:
    typedef pres_input_tool<input_tool> super;

    bool opt_full;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      opt_full = parser.options.count("full") > 0;
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("full",
                      "display the predicate variables and their signature",
                      'f'
                     )
      ;
    }

  public:
    presinfo_tool()
      : super(
        "presinfo",
        "Jan Friso Groote",
        "display basic information about a PRES",
        super::make_tool_description("Print basic information about the PRES in INFILE.")
      ),
      opt_full(false)
    {}

    /// If PRES can be loaded from file_name, then
    /// - Show if PRES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run() override
    {
      res::res_expression b;
      presinfo(input_filename(),
               input_file_message(),
               pres_input_format(),
               opt_full
              );
      return true;
    }
};

int main(int argc, char** argv)
{
  return presinfo_tool().execute(argc, argv);
}
