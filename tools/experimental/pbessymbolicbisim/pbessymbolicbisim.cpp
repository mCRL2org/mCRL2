// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbessymbolicbisim.cpp

#define TOOLNAME "pbessymbolicbisim"
#define AUTHORS "Thomas Neele"

//C++
#include <exception>

//Tool framework
#include "mcrl2/data/prover_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/utilities/input_tool.h"

#include "symbolic_bisim.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;
using namespace mcrl2::log;


class pbessymbolicbisim_tool: public rewriter_tool<input_tool>
{

protected:
  using super = rewriter_tool<input_tool>;

  simplifier_mode m_mode;
  std::size_t m_num_refine_steps = 1;
  bool m_fine_initial_partition = false;
  bool m_early_termination = false;
  bool m_randomize = false;

  /// Parse the non-default options.
  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);

    m_mode = parser.option_argument_as<simplifier_mode>("simplifier");
    if(parser.options.count("refine-steps") > 0)
    {
      m_num_refine_steps = parser.option_argument_as<std::size_t>("refine-steps");
    }
    m_fine_initial_partition = parser.options.count("fine-initial") > 0;
    m_early_termination = parser.options.count("no-early-termination") == 0;
    m_randomize = parser.options.count("randomize") > 0;
  }

  void add_options(interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("simplifier", make_enum_argument<simplifier_mode>("MODE")
      .add_value(simplify_fm)
#ifdef DBM_PACKAGE_AVAILABLE
      .add_value(simplify_dbm)
#endif
      .add_value(simplify_finite_domain)
      .add_value(simplify_identity)
      .add_value(simplify_auto, true),
      "set the simplifying strategy for expressions",'s');
    desc.add_option("refine-steps",
               make_mandatory_argument("NUM"),
               "perform the given number of refinement steps between each search for a proof graph",
               'n');
    desc.add_option("fine-initial",
               "use a fine initial partition, such that each block contains only one PBES variable");
    desc.add_option("no-early-termination",
               "do not use knowledge of simulation relations to perform early termination detection. "
               "Using this option might lead to a larger proof graph, although the runtime might become "
               "lower since the overhead of early termination checking is avoided.",
               't');
    desc.add_option("randomize",
               "randomly shuffle blocks between splits");
  }

public:
  pbessymbolicbisim_tool()
    : super(
      TOOLNAME,
      AUTHORS,
      "Symbolically compute the solution of the given PBES",
      "Computes the solution to the given PBES using symbolic bismulation techniques. "
      "Mostly useful for PBESs of low complexity with an infinite underlying BES. "
      "This tool is experimental. ")
  {}

  /// Runs the algorithm.
  /// Reads a specification from input_file,
  /// applies real time abstraction to it and writes the result to output_file.
  bool run() override
  {
    mCRL2log(verbose) << "Parameters of pbessymbolicbisim:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
    mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;

    mcrl2::pbes_system::pbes spec = mcrl2::pbes_system::detail::load_pbes(m_input_filename);

    mcrl2::data::symbolic_bisim_algorithm(spec, m_num_refine_steps, m_rewrite_strategy, m_mode, m_fine_initial_partition, m_early_termination, m_randomize).run();

    return true;
  }

};

int main(int argc, char** argv)
{
  return pbessymbolicbisim_tool().execute(argc, argv);
}
