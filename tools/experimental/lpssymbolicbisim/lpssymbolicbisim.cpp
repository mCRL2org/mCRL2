// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssymbolicbisim.cpp

#define TOOLNAME "lpssymbolicbisim"
#define AUTHORS "Thomas Neele"

//C++
#include <exception>

//LPS
#include "mcrl2/lps/io.h"

//Tool framework
#include "mcrl2/data/prover_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

#include "symbolic_bisim.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::log;


class lpssymbolicbisim_tool: public rewriter_tool<input_output_tool>
{

protected:
  using super = rewriter_tool<input_output_tool>;

  simplifier_mode m_simplifier_mode;

  /// Parse the non-default options.
  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);

    m_simplifier_mode = parser.option_argument_as<simplifier_mode>("simplifier");
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
  }

public:
  lpssymbolicbisim_tool()
    : super(
      TOOLNAME,
      AUTHORS,
      "Output the minimal LTS under strong bisimulation",
      "Performs partition refinement on "
      "INFILE and outputs the resulting LTS. "
      "This tool is highly experimental. ")
  {}

  /// Runs the algorithm.
  /// Reads a specification from input_file,
  /// applies real time abstraction to it and writes the result to output_file.
  bool run() override
  {
    mCRL2log(verbose) << "Parameters of lpssymbolicbisim:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
    mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
    mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;

    stochastic_specification spec;
    load_lps(spec, input_filename());

    mcrl2::data::symbolic_bisim_algorithm<stochastic_specification>(spec, m_simplifier_mode, m_rewrite_strategy).run();

    return true;
  }

};

int main(int argc, char** argv)
{
  return lpssymbolicbisim_tool().execute(argc, argv);
}
