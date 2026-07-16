// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbescegps.cpp

#include "mcrl2/pbes/tools/pbescegps.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using data::tools::rewriter_tool;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_rewriter_tool;

class pbescegps_tool : public pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>
{
protected:
  using super = pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>;

  pbescegps_options m_options;

  void parse_options(const command_line_parser& parser) override { 
      super::parse_options(parser);
      m_options.init_control_flow = parser.has_option("init-cfp");
      m_options.solve_symbolic = parser.has_option("solve-symbolic-args");
      m_options.solve_symbolic_args = parser.option_argument_as<std::string>("solve-symbolic-args");
      
      std::string var_choice_str = parser.option_argument_as<std::string>("var-choice");
      if (var_choice_str == "lhs")
      {
        m_options.var_choice = var_choice_strategy::lhs;
      }
      else if (var_choice_str == "rhs")
      {
        m_options.var_choice = var_choice_strategy::rhs;
      }
      else if (var_choice_str == "count")
      {
        m_options.var_choice = var_choice_strategy::count;
      }
      else
      {
        throw mcrl2::runtime_error("Invalid var-choice option '" + var_choice_str + "'. "
          "Valid options are: 'lhs', 'rhs', 'count'.");
      }
  }

  void add_options(interface_description& desc) override { 
      super::add_options(desc);
      desc.add_option("init-cfp",
                  "Use the (global) control flow parameters as initial parameters.", 'c');
      desc.add_option("solve-symbolic-args",
          utilities::make_optional_argument("STR", ""),
                  "Solve the PBES symbolically using the following arguments.", 's');
      desc.add_option("var-choice",
          utilities::make_optional_argument("STR", "lhs"),
          "'lhs' (default) the variable order of the left-hand side of the equation\n"
          "'rhs' the variable order of the right-hand side of the equation\n"
          "'count' the free variable that occurs most often (excluding data expressions in PVI)\n"
          "Choose the method of chosing a variable on iteration.");
  }

public:
  pbescegps_tool()
    : super("pbescegps",
        "Jore Booy",
        "Iteratively solve PBES by under- and overapproximating relevant parameters.",
        "Reads a file containing a PBES. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used.")
  {}

  bool run() override
  {
    mCRL2log(verbose) << "pbescegps parameters:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;

    m_options.rewrite_strategy = rewrite_strategy();
    pbescegps(input_filename(), pbes_input_format(), m_options);

    return true;
  }
};

int main(int argc, char* argv[])
{
  return pbescegps_tool().execute(argc, argv);
}
