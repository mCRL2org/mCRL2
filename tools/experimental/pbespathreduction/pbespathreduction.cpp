// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespathreduction.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/tools/pbespathreduction.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_output_tool;
using pbes_system::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

class pbespathreduction_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    using super = pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    pbespathreduction_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      if(parser.has_option("use-bdd-simplifier"))
      {
        m_options.use_bdd_simplifier = true;
        m_options.bdd_timeout = parser.option_argument_as<double>("use-bdd-simplifier");
      }
      m_options.back_substitution = !parser.has_option("no-back-substitution");
      m_options.max_depth = parser.option_argument_as<int>("max-depth");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("use-bdd-simplifier", utilities::make_optional_argument("TIMEOUT", "0.25"),
                  "Use the BDD solver in the simplifier step. "
                  "The timeout should be given in seconds (0 = no timeout).", 'b'); 
      desc.add_option("no-back-substitution",
                  "Do not substitute the solution to an equation backwards if the number "
                  "of predicate variable instances in the equation is zero. "
                  "In some cases, this makes solving the equation faster. However, "
                  "less paths can be reduced.", 's');
      desc.add_option("max-depth", utilities::make_optional_argument("TIMEOUT", "15"),
                  "The maximum depth a single predicate variable instances "
                  "gets unfolded.", 'm'); 
    }

  public:
    pbespathreduction_tool()
      : super(
        "pbespathreduction",
        "Jore Booy",
        "Simplify a pbes by backwards substituting paths.",
        "Reads a file containing a PBES, and iterates to a solution. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbespathreduction parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();
      pbespathreduction(input_filename(),
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
  return pbespathreduction_tool().execute(argc, argv);
}
