// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesiteration.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/tools/pbesiteration.h"

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

class pbesiteration_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    using super = pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    pbesiteration_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_options.check_global_invariant = parser.has_option("check-global-invariant");
      m_options.smt = parser.has_option("smt");
      m_options.early_stopping = parser.has_option("early-stopping");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("check-global-invariant",
                  "check in nu-formulae if the core constraint is a global invariant for each equation (slow)", 'g');
      desc.add_option("smt",
                  "use SMT-based fixpoint checking instead of EQ-BDD-based checking (limited support)", 's');
      desc.add_option("early-stopping",
                  "stop early based on the initial state data parameters", 'e');

    }

  public:
    pbesiteration_tool()
      : super(
        "pbesiteration",
        "Jore Booy",
        "Simplify a pbes by checking if the base contraint is an invariant.",
        "Reads a file containing a PBES, and iterates to a solution. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbesiteration parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();
                 
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());
      complete_data_specification(p);
      algorithms::normalize(p);
      pbesiteration_pbes_fixpoint_iterator fixpoint_iterator;
      fixpoint_iterator.run(p, m_options);
      save_pbes(p, output_filename(), pbes_output_format());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbesiteration_tool().execute(argc, argv);
}
