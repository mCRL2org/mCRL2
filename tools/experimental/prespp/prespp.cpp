// Author(s): Jan Friso Groote. Based on pbespp.cpp by Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file prespp.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/res/pres_input_tool.h"
#include "mcrl2/pres/tools.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2;
using res::tools::pres_input_tool;

//local declarations

class prespp_tool: public pres_input_tool<input_output_tool>
{
  private:
    typedef pres_input_tool<input_output_tool> super;

  public:
    prespp_tool()
      : super("prespp", 
              "Jan Friso Groote",
              "pretty print a PRES",
              "Print the PRES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      format(core::print_default)
    {}

    bool run() override
    {
      pres_system::prespp(input_filename(),
                          output_filename(),
                          pres_input_format(),
                          format,
                          use_pfnf_printer
                         );
      return true;
    }

  protected:
    core::print_format_type  format;
    bool use_pfnf_printer = false;

    void add_options(interface_description& desc) override
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<core::print_format_type>("FORMAT")
                      .add_value_desc(core::print_default, "a PRES specification", true)
                      .add_value_desc(core::print_internal, "a textual ATerm representation of the internal format"),
                      "print the PRES in the specified FORMAT:", 'f');
      desc.add_option("pfnf-printer",
                      "format the output according to the structure of PFNF (only has an effect when printing a PRES in PFNF to text)", 
                      'p');
    }

    void parse_options(const command_line_parser& parser) override
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
  return prespp_tool().execute(argc, argv);
}

