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
#include "mcrl2/utilities/parallel_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using data::tools::rewriter_tool;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_rewriter_tool;

class pbescegps_tool : public parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>
{
protected:
  using super = parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>;

  pbescegps_options m_options;

  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);
    m_options.init_control_flow = parser.has_option("init-cfp");
    m_options.instantiate_infinite_quantifier_guards = parser.has_option("instantiate-infinite-quantifier-guards");
    m_options.rules_ideal = parser.has_option("rules-ideal");
    m_options.solve_symbolic = parser.has_option("solve-symbolic-args");
    m_options.stategraph = parser.has_option("stategraph");
    m_options.solve_symbolic_args = parser.option_argument_as<std::string>("solve-symbolic-args");
    m_options.optimization = parser.option_argument_as<partial_solve_strategy>("optimization");
    m_options.number_of_threads = number_of_threads();
    m_options.initial_state_file = parser.option_argument_as<std::string>("initial-state");
    m_options.ruling_file = parser.option_argument("ruling");

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
    else if (var_choice_str == "all")
    {
      m_options.var_choice = var_choice_strategy::all;
    }
    else if (var_choice_str == "ruling")
    {
      m_options.var_choice = var_choice_strategy::ruling;
    }
    else
    {
      throw mcrl2::runtime_error("Invalid var-choice option '" + var_choice_str
                                 + "'. "
                                   "Valid options are: 'lhs', 'rhs', 'count', 'all', 'ruling'.");
    }
  }

  void add_options(interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("init-cfp", "Use the (global) control flow parameters as initial parameters.", 'c');
    desc.add_option("rules-ideal",
      "Enforce the order-ideal invariant: if a gate parameter is abstracted, "
      "all data parameters it rules must also be abstracted.");
    desc.add_option("instantiate-infinite-quantifier-guards",
      "Do not abstract parameters that occur in the guards of predicate variable instances in the scope of an "
      "infinite quantifier, to prevent infinitely many vertices in the structure graph.",
      'g');
    desc.add_option("stategraph", "Use stategraph for each under and overapproximation.");
    desc.add_option("optimization",
      utilities::make_enum_argument<partial_solve_strategy>("STRATEGY")
        .add_value_desc(partial_solve_strategy::no_optimisation, "Do not apply any optimizations.", true)
        .add_value_desc(partial_solve_strategy::remove_self_loops, "Remove self loops.")
        .add_value_desc(partial_solve_strategy::propagate_solved_equations_using_substitution,
          "Propagate solved equations using substitution.")
        .add_value_desc(partial_solve_strategy::propagate_solved_equations_using_attractor,
          "Propagate solved equations using an attractor.")
        .add_value_desc(partial_solve_strategy::detect_winning_loops_using_fatal_attractor,
          "Detect winning loops using a fatal attractor.")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_fatal_attractor_local,
          "Solve subgames using a fatal attractor (local version).")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_fatal_attractor_original,
          "Solve subgames using a fatal attractor (original version).")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_solver, "Solve subgames using the solver.")
        .add_value_desc(partial_solve_strategy::detect_winning_loops_original,
          "Detect winning loops (original version)."),
      "Use optimization STRATEGY while solving approximations.",
      's');
    desc.add_option("solve-symbolic-args",
      utilities::make_optional_argument("STR", ""),
      "Solve the PBES symbolically using the following arguments.");
    desc.add_option("var-choice",
      utilities::make_optional_argument("STR", "lhs"),
      "'lhs' (default) the variable order of the left-hand side of the equation\n"
      "'rhs' the variable order of the right-hand side of the equation\n"
      "'count' the free variable that occurs most often (excluding data expressions in PVI)\n"
      "'all' un-abstract all variables that occur on iteration\n"
      "'ruling' prioritize based on the ruled-by ordering: pick the variable that rules the most others\n"
      "Choose the method of chosing a variable on iteration.");
    desc.add_option("initial-state",
      utilities::make_mandatory_argument("FILE"),
      "Read the initial set of abstracted parameters from FILE instead of abstracting all parameters.\n"
      "FILE contains one line per equation as printed by the tool:\n"
      "  Abstracted parameters for X0: value_ValueBool57 value_ValueReal5\n"
      "Lines can also use parameter indices instead of names, and equations that are not mentioned\n"
      "in FILE are not abstracted.",
      'a');
    desc.add_hidden_option("ruling",
      utilities::make_optional_argument("FILE", ""),
      "save the ruling relation in text format");
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
