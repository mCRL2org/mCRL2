// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespor.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/bes/pbes_rewriter_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/tools/pbespor.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using bes::tools::pbes_input_tool;
using bes::tools::pbes_output_tool;
using bes::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

class pbespor_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    typedef pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>> super;

    pbespor_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_options.use_condition_L = !parser.has_option("no-l");
      m_options.compute_weak_conditions = parser.has_option("weak-conditions");
      m_options.compute_determinism = !parser.has_option("no-determinism");
      m_options.compute_triangle_accordance = parser.has_option("triangle");
      m_options.compute_left_accordance = parser.has_option("left");
      m_options.compute_NES = parser.has_option("nes");
      m_options.reduction = !parser.has_option("full");
      if(parser.has_option("use-smt-solver"))
      {
        m_options.use_smt_solver = true;
        m_options.smt_timeout = std::chrono::milliseconds{parser.option_argument_as<std::size_t>("use-smt-solver")};
      }
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("no-l",
                  "do not apply the condition L (might affect correctness)");
      desc.add_option("weak-conditions",
                  "use weak accordance conditions (cheaper, but less exact, static analysis)", 'w');
      desc.add_option("no-determinism",
                  "do not check whether transitions are deterministic (cheaper, but less powerful, static analysis)");
      desc.add_option("triangle",
                  "compute the triangle accordance relation (more expensive, more powerful, static analysis)");
      desc.add_option("left",
                  "compute the left accordance relation (more expensive, more powerful, static analysis)");
      desc.add_option("nes",
                  "compute the necessary enabling relation (more expensive, more powerful, static analysis)");
      desc.add_option("full",
                  "explore the full state space. No static analysis is performed.");
      desc.add_option("use-smt-solver", utilities::make_optional_argument("TIMEOUT", "0"),
                  "Use the SMT solver Z3 (must be in the path). "
                  "The timeout should be given in milliseconds (0 = no timeout). "
                  "Very small values may lead to errors.", 's');
    }

  public:
    pbespor_tool()
      : super(
        "pbespor",
        "Wieger Wesselink; Thomas Neele and Tim Willemse",
        "apply partial order reduction to a PBES",
        "Reads a file containing a PBES, and applies partial order reduction to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbespor parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();
      pbespor(input_filename(),
              output_filename(),
              pbes_input_format(),
              pbes_output_format(),
              m_options
             );

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbespor_tool().execute(argc, argv);
}
