// Author(s): Jan Friso Groote based on respp.cpp by Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file respp.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/pres/print.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;
using mcrl2::pres_system::tools::pres_input_tool;

//local declarations

class respp_tool: public pres_input_tool<input_output_tool>
{
  private:
    using super = pres_input_tool<input_output_tool>;

  public:
    respp_tool()
      : super("respp", 
              "Jeroen Keiren", 
              "pretty print a RES",
              "Print the RES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      print_specification();
      return true;
    }

  protected:
    print_format_type format = print_default;

    void add_options(interface_description& desc) override
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<print_format_type>("FORMAT")
                      .add_value_desc(print_default, "for a RES specification", true),
                      "print the PRES in the specified FORMAT:", 'f');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      format = parser.option_argument_as<print_format_type>("format");
    }

  private:
    void print_specification()
    {
      pres_system::pres res;
      load_pres(res,input_filename(), pres_input_format());

      mCRL2log(verbose) << "printing RES from " << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << format << " format";

      if (output_filename().empty())
      {
        std::cout << pres_system::pp(res);
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream.is_open())
        {
          output_stream << pres_system::pp(res);
          output_stream.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
    }
};

int main(int argc, char* argv[])
{
  return respp_tool().execute(argc, argv);
}

