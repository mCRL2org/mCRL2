// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bespp.cpp

#define NAME "bespp"
#define AUTHOR "Jeroen Keiren"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/print.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;
using mcrl2::pbes_system::tools::pbes_input_tool;

//local declarations

class bespp_tool: public pbes_input_tool<input_output_tool>
{
  private:
    using super = pbes_input_tool<input_output_tool>;

  public:
    bespp_tool()
      : super(NAME, AUTHOR,
              "pretty print a BES",
              "Print the BES in INFILE to OUTFILE in a human readable format. If OUTFILE "
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
                      .add_value_desc(print_default, "for a BES specification", true),
                      "print the PBES in the specified FORMAT:", 'f');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      format = parser.option_argument_as<print_format_type>("format");
    }

  private:
    void print_specification()
    {
      pbes_system::pbes bes;
      load_pbes(bes,input_filename(), pbes_input_format());

      mCRL2log(verbose) << "printing BES from " << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << format << " format";

      if (output_filename().empty())
      {
        std::cout << pbes_system::pp(bes);
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream.is_open())
        {
          output_stream << pbes_system::pp(bes);
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
  return bespp_tool().execute(argc, argv);
}

