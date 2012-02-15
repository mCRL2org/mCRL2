// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bespp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "bespp"
#define AUTHOR "Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/print.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

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
      desc.add_option("format", make_mandatory_argument("FORMAT"),
                      "print the PBES in the specified FORMAT:\n"
                      "  'default' for a PBES specification (default)", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("format"))
      {
        std::string str_format(parser.option_argument("format"));
        if (str_format != "default")
        {
          parser.error("option -f/--format has illegal argument '" + str_format + "'");
        }
      }
    }

  private:
    void print_specification()
    {
      bes::boolean_equation_system<> bes;
      load_bes(bes,input_filename(), bes_input_format());

      mCRL2log(verbose) << "printing BES from " << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << pp_format_to_string(format) << " format";

      if (format != print_default)
      {
        throw mcrl2::runtime_error("Pretty printing in " + pp_format_to_string(format) + " format is currently not supported");
      }

      if (output_filename().empty())
      {
        if (format == print_default)
        {
          std::cout << bes::pp(bes);
        }
        else
        {
          atermpp::aterm_appl b = boolean_equation_system_to_aterm(bes);
          std::cout << core::pp(b);
        }
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream.is_open())
        {
          if (format == print_default)
          {
            output_stream << bes::pp(bes);
          }
          else
          {
            atermpp::aterm_appl b = boolean_equation_system_to_aterm(bes);
            output_stream << core::pp(b);
          }
          output_stream.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
    }
};

class bespp_gui_tool: public mcrl2_gui_tool<bespp_tool>
{
  public:
    bespp_gui_tool()
    {

      std::vector<std::string> values;

      values.clear();
      values.push_back("default");
      values.push_back("debug");
      values.push_back("internal");
      values.push_back("internal-debug");
      m_gui_options["format"] = create_radiobox_widget(values);
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return bespp_gui_tool().execute(argc, argv);
}

