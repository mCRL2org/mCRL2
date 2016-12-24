// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbeseqelm.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/bes/pbes_rewriter_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/tools.h"
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

class pbes_eqelm_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    typedef pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > > super;

    /// \brief If true, the initial state is ignored.
    bool m_ignore_initial_state;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_ignore_initial_state = parser.options.count("ignore-initial-state") > 0;
    }

    void add_options(interface_description& desc)
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

    bool run()
    {
      mCRL2log(verbose) << "pbeseqelm parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      pbespareqelm(input_filename(),
                   output_filename(),
                   pbes_input_format(),
                   pbes_output_format(),
                   rewrite_strategy(),
                   rewriter_type(),
                   m_ignore_initial_state
                 );
      return true;
    }
};

int main(int argc, char* argv[])
{
  pbes_eqelm_tool tool;
  return tool.execute(argc, argv);
}
