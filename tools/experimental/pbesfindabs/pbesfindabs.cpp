// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesfindabs.cpp

#include "mcrl2/pbes/tools/pbesfindabs.h"
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

class pbesfindabs_tool : public parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>
{
protected:
  using super = parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>;

  pbesfindabs_options m_options;

  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);
    m_options.cepgps.init_control_flow = parser.has_option("init-cfp");
    m_options.cepgps.instantiate_infinite_quantifier_guards
      = parser.has_option("instantiate-infinite-quantifier-guards");
    m_options.cepgps.solve_symbolic = parser.has_option("solve-symbolic-args");
    m_options.cepgps.stategraph = parser.has_option("stategraph");
    m_options.cepgps.solve_symbolic_args = parser.option_argument_as<std::string>("solve-symbolic-args");
    m_options.cepgps.optimization = parser.option_argument_as<partial_solve_strategy>("optimization");
    m_options.output_file = parser.option_argument("abstraction-file");
    if (m_options.output_file.empty())
    {
      throw mcrl2::runtime_error("The option -a/--abstraction-file with an output file name is required.");
    }
    m_options.number_of_threads = number_of_threads();
  }

  void add_options(interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("abstraction-file",
      utilities::make_mandatory_argument("FILE"),
      "Write every valid abstraction set to FILE as soon as it is found.",
      'a');
    desc.add_option("init-cfp", "Only consider the non-(global) control flow parameters for abstraction.", 'c');
    desc.add_option("instantiate-infinite-quantifier-guards",
      "Do not consider parameters that occur in the guards of predicate variable instances in the scope of an "
      "infinite quantifier for abstraction, to prevent infinitely many vertices in the structure graph.",
      'g');
    desc.add_option("stategraph", "Use stategraph for each approximation.");
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
      "Solve the PBES and the approximations symbolically using the following arguments.");
  }

public:
  pbesfindabs_tool()
    : super("pbesfindabs",
        "Jore Booy",
        "Find all valid abstraction sets of a PBES.",
        "Reads a file containing a PBES and enumerates all valid abstraction "
        "sets, writing each of them to the file given by -a. If INFILE is not "
        "present, standard input is used.")
  {}

  bool run() override
  {
    mCRL2log(verbose) << "pbesfindabs parameters:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
    mCRL2log(verbose) << "  abstraction file:   " << m_options.output_file << std::endl;
    mCRL2log(verbose) << "  number of threads:  " << m_options.number_of_threads << std::endl;

    m_options.cepgps.rewrite_strategy = rewrite_strategy();
    pbesfindabs(input_filename(), pbes_input_format(), m_options);

    return true;
  }
};

int main(int argc, char* argv[])
{
  return pbesfindabs_tool().execute(argc, argv);
}
