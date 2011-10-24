// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpspp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "lpspp"
#define AUTHOR "Aad Mathijssen and Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

class lpspp_tool: public input_output_tool
{
  private:

    typedef input_output_tool super;

  public:
    lpspp_tool()
      : super(NAME, AUTHOR,
              "pretty print an LPS",
              "Print the mCRL2 LPS in INFILE to OUTFILE in a human readable format. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             ),
      format(ppDefault)
    {}

    bool run()
    {
      print_specification();
      return true;
    }

  protected:
    t_pp_format  format;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("format", make_mandatory_argument("FORMAT"),
                      "print the LPS in the specified FORMAT:\n"
                      "  'default' for a process specification (default),\n"
                      "  'internal' for a textual ATerm representation of the internal format", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      input_output_tool::parse_options(parser);
      if (parser.options.count("format"))
      {
        std::string str_format(parser.option_argument("format"));
        if (str_format == "internal")
        {
          format = ppInternal;
        }
        else if (str_format != "default")
        {
          parser.error("option -f/--format has illegal argument '" + str_format + "'");
        }
      }
    }

  private:
    void print_specification()
    {
      lps::specification specification;
      specification.load(input_filename());

      mCRL2log(verbose) << "printing LPS from "
                        << (input_filename().empty()?"standard input":input_filename())
                        << " to " << (output_filename().empty()?"standard output":output_filename())
                        << " in the " << pp_format_to_string(format) << " format" << std::endl;

      if (output_filename().empty())
      {
        if (format == ppInternal)
        {
          std::cout << specification_to_aterm(specification);
        }
        else
        {
          std::cout << lps::pp(specification);
        }
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if (output_stream.is_open())
        {
          if (format == ppInternal)
          {
            output_stream << specification_to_aterm(specification);
          }
          else
          {
            output_stream << lps::pp(specification);
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

class lpspp_gui_tool: public mcrl2_gui_tool<lpspp_tool>
{
  public:
    lpspp_gui_tool()
    {

      std::vector<std::string> values;

      values.clear();
      values.push_back("default");
      values.push_back("internal");
      m_gui_options["format"] = create_radiobox_widget(values);
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return lpspp_gui_tool().execute(argc, argv);
}
