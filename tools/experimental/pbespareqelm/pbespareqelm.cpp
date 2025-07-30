// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbeseqelm.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/eqelm.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

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

class pbes_eqelm_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    using super = pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    /// \brief If true, the initial state is ignored.
    bool m_ignore_initial_state = false;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_ignore_initial_state = parser.options.count("ignore-initial-state") > 0;
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("ignore-initial-state", "ignore the initial state in the computation", 'I');
    }

  public:
    pbes_eqelm_tool()
      : super(
        "pbeseqelm",
        "Wieger Wesselink; Simona Orzan and Tim Willemse",
        "Compute equivalence relations on parameters of a PBES",
        "Reads a file containing a PBES, and applies the eqelm algorithm to detect equivalence "
        "relations between the parameters. If OUTFILE is not present, standard output is used. "
        "If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbeseqelm parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
                 
      // load the pbes
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      // apply the eqelm algorithm
      eqelm(p, rewrite_strategy(), rewriter_type(), m_ignore_initial_state);

      // save the result
      save_pbes(p, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char* argv[])
{
  pbes_eqelm_tool tool;
  return tool.execute(argc, argv);
}
