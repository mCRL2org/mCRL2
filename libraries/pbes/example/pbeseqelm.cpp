// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbeseqelm.cpp

#include "boost.hpp" // precompiled headers

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/eqelm.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;

class pbes_eqelm_tool: public pbes_rewriter_tool<rewriter_tool<input_output_tool> >
{
  protected:
    typedef pbes_rewriter_tool<rewriter_tool<input_output_tool> > super;

    /// \brief If true, the computation is started from the initial state.
    bool m_use_initial_state;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_use_initial_state = parser.options.count("use-initial-state") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("use-initial-state", "start the computation from the initial state", 'i');
    }

  public:
    pbes_eqelm_tool()
      : super(
          "pbeseqelm",
          "Wieger Wesselink",
          "Compute equivalence relations on parameters of a PBES",
          "Reads a file containing a PBES, and applies the eqelm algorithm to detect equivalence "
          "relations on the parameters. If OUTFILE is not present, standard output is used. If "
          "INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cerr << "pbeseqelm parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      // data rewriter
      data::rewriter datar = create_rewriter(p.data());

      // pbes rewriter
      switch (rewriter_type())
      {
        case simplify:
        {
          typedef simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> my_pbes_rewriter;
          my_pbes_rewriter pbesr(datar);
          pbes_eqelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
          data::number_postfix_generator name_generator("UNIQUE_PREFIX");
          algorithm.run(p, m_use_initial_state);
          break;
        }
        case quantifier_all:
        case quantifier_finite:
        {
          typedef pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > my_pbes_rewriter;
          bool enumerate_infinite_sorts = (rewriter_type() == quantifier_all);
          data::number_postfix_generator name_generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, name_generator);
          data::rewriter_with_variables datarv(datar);
          my_pbes_rewriter pbesr(datarv, datae, enumerate_infinite_sorts);
          pbes_eqelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
          algorithm.run(p, m_use_initial_state);
          break;
        }
        default:
        { }
      }

      // save the result
      p.save(m_output_filename);

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pbes_eqelm_tool tool;
  return tool.execute(argc, argv);
}
