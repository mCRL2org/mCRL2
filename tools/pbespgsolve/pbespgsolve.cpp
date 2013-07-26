// Author(s): Michael Weber, Maks Verver, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespgsolve.cpp

//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <cstdio>

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/pbes_input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/execution_timer.h"

#include "mcrl2/pbes/pbespgsolve.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/pg_parse.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using utilities::tools::input_tool;
using utilities::tools::pbes_input_tool;
using utilities::tools::rewriter_tool;
using namespace mcrl2::log;

// class pg_solver_tool: public pbes_rewriter_tool<rewriter_tool<input_tool> >
// TODO: extend the tool with rewriter options

// scc decomposition can be compiled in using directive
// PBESPGSOLVE_ENABLE_SCC_DECOMPOSITION

class pg_solver_tool : public rewriter_tool<pbes_input_tool<input_tool> >
{
  protected:
    typedef rewriter_tool<pbes_input_tool<input_tool> > super;

    pbespgsolve_options m_options;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("solver-type",
                      make_enum_argument<pbespg_solver_type>("NAME")
                      .add_value(spm_solver, true)
                      .add_value(alternative_spm_solver)
                      .add_value(recursive_solver),
                      "Use the solver type NAME:", 's');
      desc.add_option("scc", "Use scc decomposition", 'c');
      desc.add_option("loop", "Eliminate self-loops", 'L');
      desc.add_option("cycle", "Eliminate cycles", 'C');
      desc.add_option("verify", "Verify the solution", 'e');
      desc.add_option("onlygenerate", "Only generate the BES without solving", 'g');
      desc.add_hidden_option("equation_limit",
                             make_optional_argument("NAME", "-1"),
                             "Set a limit to the number of generated BES equations",
                             'l');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_options.solver_type = parse_solver_type(parser.option_argument("solver-type"));
      m_options.use_scc_decomposition = (parser.options.count("scc") > 0);
      m_options.use_deloop_solver = (parser.options.count("loop") > 0);
      m_options.use_decycle_solver = (parser.options.count("cycle") > 0);
      m_options.verify_solution = (parser.options.count("verify") > 0);
      m_options.only_generate = (parser.options.count("onlygenerate") > 0);
      if (parser.options.count("equation_limit") > 0)
      {
        int limit = parser.option_argument_as<int>("equation_limit");
        pbes_system::detail::set_bes_equation_limit(limit);
      }
    }

  public:

    pg_solver_tool()
      : super(
        "pbespgsolve",
        "Maks Verver and Wieger Wesselink; Michael Weber",
        "Solve a (P)BES or parity game using a parity game solver",
        "Reads a file containing a (P)BES or a max parity game in PGSolver format,"
        "instantiates it into a BES, and applies a\n"
        "parity game solver to it. If INFILE is not present, standard input is used."
      )
    {
    }

    bool run()
    {
      m_options.rewrite_strategy = rewrite_strategy();

      mCRL2log(verbose) << "pbespgsolve parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:        " << input_filename() << std::endl;
      mCRL2log(verbose) << "  solver type:       " << print(m_options.solver_type) << std::endl;
      mCRL2log(verbose) << "  eliminate self-loops: " << (m_options.use_deloop_solver?"yes":"no") << std::endl;
      mCRL2log(verbose) << "  eliminate cycles:  " << (m_options.use_decycle_solver?"yes":"no") << std::endl;
      mCRL2log(verbose) << "  scc decomposition: " << std::boolalpha << m_options.use_scc_decomposition << std::endl;
      mCRL2log(verbose) << "  verify solution:   " << std::boolalpha << m_options.verify_solution << std::endl;
      mCRL2log(verbose) << "  only generate:   " << std::boolalpha << m_options.only_generate << std::endl;

      bool value;
      if(pbes_input_format() == pbes_file_pgsolver)
      {
        pbespgsolve_algorithm algorithm(timer(), m_options);
        ParityGame pg;
        std::ifstream is(input_filename().c_str());
        pg.read_pgsolver(is);
        value = algorithm.run(pg, 0);
      }
      else
      {
        pbes p;
        timer().start("load");
        load_pbes(p, input_filename(), pbes_input_format());
        timer().finish("load");

        value = pbespgsolve(p, timer(), m_options);
      }
      std::string result = (value ? "true" : "false");
      mCRL2log(verbose) << "The solution for the initial variable of the pbes is " << result << std::endl;
      std::cout << result << std::endl;

      return true;
    }
};

int main(int argc, char* argv[])
{
  return pg_solver_tool().execute(argc, argv);
}
