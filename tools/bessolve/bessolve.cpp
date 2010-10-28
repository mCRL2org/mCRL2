// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bessolve.cpp

#include "boost.hpp" // precompiled headers

#define NAME "bessolve"
#define AUTHOR "Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes_gauss_elimination.h"
#include "mcrl2/bes/small_progress_measures.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

typedef enum { gauss, spm } solution_strategy_t;

std::string solution_strategy_to_string(const solution_strategy_t s)
{
  switch(s)
  {
    case gauss:
      return "gauss";
      break;
    case spm:
      return "spm";
      break;
    default:
      return "unknown";
      break;
  }
}

//local declarations

class bessolve_tool: public input_output_tool
{
  private:
    typedef input_output_tool super;

  public:
    bessolve_tool()
      : super(NAME, AUTHOR,
          "solve a BES",
          "Solve the BES in INFILE. If INFILE is not present, stdin is used."
        ),
        strategy(gauss)
    {}

    bool run()
    {
      print_specification();
      return true;
    }

  protected:
    solution_strategy_t strategy;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("strategy", make_mandatory_argument("STRATEGY"),
        "solve the BES using the specified STRATEGY:\n"
        "  'gauss' for Gauss elimination (default),\n"
        "  'spm' for Small Progress Measures,\n", 's');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("strategy")) {
        std::string str_strategy(parser.option_argument("strategy"));
        if (str_strategy == "gauss") {
          strategy = gauss;
        }
        else if (str_strategy == "spm") {
          strategy = spm;
        }
        else
        {
          parser.error("option -s/--strategy has illegal argument '" + str_strategy + "'");
        }
      }
    }

  private:
    void print_specification()
    {
      bes::boolean_equation_system<> bes;
      bes.load(input_filename());

      gsVerboseMsg("solving BES in %s using %s\n",
        input_filename().empty()?"standard input":input_filename().c_str(),
        solution_strategy_to_string(strategy).c_str());

      unsigned int log_level = 0;
      if(core::gsVerbose)
      {
        log_level = 1;
      }
      if(core::gsDebug)
      {
        log_level = 2;
      }

      bool result = false;
      switch(strategy)
      {
        case gauss:
          result = gauss_elimination(bes, log_level);
          break;
        case spm:
          result = small_progress_measures(bes, log_level);
          break;
        default:
          throw mcrl2::runtime_error("unhandled strategy provided");
          break;
      }

      std::cout << "The solution for the initial variable of the BES is " << (result?"true":"false") << std::endl;
    }
};

class bessolve_gui_tool: public mcrl2_gui_tool<bessolve_tool> {
public:
	bessolve_gui_tool() {

		std::vector<std::string> values;

		values.clear();
		values.push_back("gauss");
		values.push_back("spm");
		m_gui_options["strategy"] = create_radiobox_widget(values);
	}
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return bessolve_gui_tool().execute(argc, argv);
}

