// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespp.cpp

#define NAME "pbespp"
#define AUTHOR "Aad Mathijssen and Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/pbes/tools.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2;
using bes::tools::pbes_input_tool;

//local declarations

class pbespp_tool: public pbes_input_tool<input_output_tool>
{
  private:
    typedef pbes_input_tool<input_output_tool> super;

  public:
    pbespp_tool()
      : super(NAME, AUTHOR,
              "pretty print a PBES",
              "Print the PBES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      format(core::print_default)
    {}

    bool run()
    {
      pbes_system::pbespp(input_filename(),
                          output_filename(),
                          pbes_input_format(),
                          format,
                          use_pfnf_printer
                         );
      return true;
    }

  protected:
    core::print_format_type  format;
    bool use_pfnf_printer = false;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<core::print_format_type>("FORMAT")
                      .add_value_desc(core::print_default, "a PBES specification", true)
                      .add_value_desc(core::print_internal, "a textual ATerm representation of the internal format"),
                      "print the PBES in the specified FORMAT:", 'f');
      desc.add_option("pfnf-printer",
                      "format the output according to the structure of PFNF (only has an effect when printing a PBES in PFNF to text)", 
                      'p');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      format = parser.option_argument_as<core::print_format_type>("format");
      if (parser.options.count("pfnf-printer")>0)
      {
        use_pfnf_printer = true;
      }
    }
};

int main(int argc, char* argv[])
{
  return pbespp_tool().execute(argc, argv);
}

