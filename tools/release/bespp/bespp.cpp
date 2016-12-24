// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bespp.cpp

#define NAME "bespp"
#define AUTHOR "Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/print.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;
using mcrl2::bes::tools::bes_input_tool;

//local declarations

class bespp_tool: public bes_input_tool<input_output_tool>
{
  private:
    typedef bes_input_tool<input_output_tool> super;

  public:
    bespp_tool()
      : super(NAME, AUTHOR,
              "pretty print a BES",
              "Print the BES in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      format(print_default)
    {}

    bool run()
    {
      print_specification();
      return true;
    }

  protected:
    print_format_type  format;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_enum_argument<print_format_type>("FORMAT")
                      .add_value_desc(print_default, "for a BES specification", true),
                      "print the PBES in the specified FORMAT:", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      format = parser.option_argument_as<print_format_type>("format");
    }

  private:
    void print_specification()
    {
      bes::boolean_equation_system bes;
      load_bes(bes,input_filename(), bes_input_format());

      mCRL2log(verbose) << "printing BES from " << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << format << " format";

      if (output_filename().empty())
      {
        std::cout << bes::pp(bes);
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream.is_open())
        {
          output_stream << bes::pp(bes);
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

