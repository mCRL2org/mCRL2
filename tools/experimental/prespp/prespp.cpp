// Author(s): Jan Friso Groote. Based on pbespp.cpp by Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file prespp.cpp

#include "mcrl2/pres/io.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2;
using pres_system::tools::pres_input_tool;

//local declarations

class prespp_tool: public pres_input_tool<input_output_tool>
{
  private:
    using super = pres_input_tool<input_output_tool>;

  public:
    prespp_tool()
      : super("prespp", 
              "Jan Friso Groote",
              "pretty print a PRES",
              "Print the PRES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      pres_system::pres p;
      load_pres(p, input_filename(), pres_input_format());

      mCRL2log(log::verbose) << "printing PRES from "
                            << (input_filename().empty()?"standard input":input_filename())
                            << " to " << (output_filename().empty()?"standard output":output_filename())
                            << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

      if (output_filename().empty())
      {
        if (format == core::print_internal)
        {
          std::cout << pres_to_aterm(p);
        }
        else
        {
          std::cout << pp(p);
        }
      }
      else
      {
        std::ofstream out(output_filename().c_str());
        if (out)
        {
          if (format == core::print_internal)
          {
            out << pres_to_aterm(p);
          }
          else
          {
            out << pp(p);
          }
          out.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
      return true;
    }

  protected:
    core::print_format_type format = core::print_default;

    void add_options(interface_description& desc) override
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<core::print_format_type>("FORMAT")
                      .add_value_desc(core::print_default, "a PRES specification", true)
                      .add_value_desc(core::print_internal, "a textual ATerm representation of the internal format"),
                      "print the PRES in the specified FORMAT:", 'f');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      format = parser.option_argument_as<core::print_format_type>("format");
    }
};

int main(int argc, char* argv[])
{
  return prespp_tool().execute(argc, argv);
}

