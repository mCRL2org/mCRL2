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

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/execution_timer.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/gauss_elimination.h"
#include "mcrl2/bes/small_progress_measures.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

typedef enum { gauss, spm } solution_strategy_t;

static
std::string solution_strategy_to_string(const solution_strategy_t s)
{
  switch (s)
  {
    case gauss:
      return "gauss";
      break;
    case spm:
      return "spm";
      break;
  }
  throw mcrl2::runtime_error("unknown solution strategy");
}

static
std::ostream& operator<<(std::ostream& os, const solution_strategy_t s)
{
  os << solution_strategy_to_string(s);
  return os;
}

static
solution_strategy_t parse_solution_strategy(const std::string& s)
{
  if (s == "gauss")
  {
    return gauss;
  }
  else if (s == "spm")
  {
    return spm;
  }
  else
  {
    throw mcrl2::runtime_error("unsupported solution strategy '" + s + "'");
  }
}

static
std::istream& operator>>(std::istream& is, solution_strategy_t& s)
{
  try
  {
    std::string str;
    is >> str;
    s = parse_solution_strategy(str);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

static
std::string description(const solution_strategy_t s)
{
  switch (s)
  {
    case gauss:
      return "Gauss elimination";
      break;
    case spm:
      return "Small Progress Measures";
      break;
  }
  throw mcrl2::runtime_error("unknown solution strategy");
}

//local declarations

class bessolve_tool: public bes_input_tool<input_output_tool>
{
  private:
    typedef bes_input_tool<input_output_tool> super;

  public:
    bessolve_tool()
      : super(NAME, AUTHOR,
              "solve a BES",
              "Solve the BES in INFILE. If INFILE is not present, stdin is used."
             ),
      strategy(spm)
    {}

    bool run()
    {
      bes::boolean_equation_system<> bes;
      load_bes(bes,input_filename(),bes_input_format());

      mCRL2log(verbose) << "solving BES in " <<
                   (input_filename().empty()?"standard input":input_filename()) << " using " <<
                   solution_strategy_to_string(strategy) << "" << std::endl;

      bool result = false;

      timer().start("solving");
      switch (strategy)
      {
        case gauss:
          result = gauss_elimination(bes);
          break;
        case spm:
          result = small_progress_measures(bes);
          break;
        default:
          throw mcrl2::runtime_error("unhandled strategy provided");
          break;
      }
      timer().finish("solving");

      std::cout << "The solution for the initial variable of the BES is " << (result?"true":"false") << std::endl;

      return true;
    }

  protected:
    solution_strategy_t strategy;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("strategy", make_enum_argument<solution_strategy_t>("STRATEGY")
                      .add_value(spm, true)
                      .add_value(gauss),
                      "solve the BES using the specified STRATEGY:", 's');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      parser.option_argument_as<solution_strategy_t>("strategy");
    }
};

class bessolve_gui_tool: public mcrl2_gui_tool<bessolve_tool>
{
  public:
    bessolve_gui_tool()
    {

      std::vector<std::string> values;

      values.clear();
      values.push_back("spm");
      values.push_back("gauss");
      m_gui_options["strategy"] = create_radiobox_widget(values);
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return bessolve_gui_tool().execute(argc, argv);
}

