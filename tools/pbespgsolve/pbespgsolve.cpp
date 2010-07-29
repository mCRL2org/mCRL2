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

#include "boost.hpp" // precompiled headers

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <cstdio>

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
//#include "mcrl2/utilities/rewriter_tool.h"
//#include "mcrl2/utilities/pbes_rewriter_tool.h"

#include "pbespgsolve.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using utilities::tools::input_tool;
//using utilities::tools::rewriter_tool;
//using utilities::tools::pbes_rewriter_tool;

// class pg_solver_tool: public pbes_rewriter_tool<rewriter_tool<input_tool> >
// TODO: extend the tool with rewriter options

class pg_solver_tool : public input_tool
{
protected:
  typedef input_tool super;

  pbespgsolve_options m_options;

  pbespg_solver_type parse_solver_type(const std::string& s) const
  {
    if (s == "spm")
    {
      return spm_solver;
    }
    else if (s == "recursive")
    {
      return recursive_solver;
    }
    throw mcrl2::runtime_error("pbespgsolve: unknown solver " + s);
  }

  void add_options(interface_description& desc)
  {
    super::add_options(desc);
    desc.add_option("solver-type",
        make_optional_argument("NAME", "spm"),
        "Use the solver type NAME:\n"
        "  'spm' (default), or\n"
        "  'recursive'",
        's');
    desc.add_option("scc", "Use scc decomposition", 'c');
    desc.add_option("verify", "Verify the solution", 'e');
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
    m_options.verify_solution = (parser.options.count("verify") > 0);
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
  "Solve a PBES using a parity game solver",
  "Reads a file containing a PBES, instantiates it into a BES, and applies a\n"
  "parity game solver to it. If INFILE is not present, standard input is used."
  )
  {
  }

  bool run()
  {
    if (mcrl2::core::gsVerbose)
    {
      std::clog << "pbespgsolve parameters:" << std::endl;
      std::clog << "  input file:        " << input_filename() << std::endl;
      std::clog << "  solver type:       " << print(m_options.solver_type) << std::endl;
      std::clog << "  scc decomposition: " << std::boolalpha << m_options.use_scc_decomposition << std::endl;
      std::clog << "  verify solution:   " << std::boolalpha << m_options.verify_solution << std::endl;
    }

    pbes<> p;
    p.load(input_filename());
    unsigned int log_level = 0;
    if (mcrl2::core::gsVerbose)
    {
      log_level = 1;
    }
    if (mcrl2::core::gsDebug)
    {
      log_level = 2;
    }
    set_parity_game_generator_log_level(log_level); 
    pbespgsolve_algorithm algorithm(m_options);
    std::string result = "unknown";
    try
    {
      result = algorithm.run(p) ? "true" : "false";
    }
    catch (std::out_of_range)
    {
    	// value unknown is already set
    }
    std::clog << "The solution for the initial variable of the pbes is " << result << "\n";

    return true;
  }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pg_solver_tool tool;
  return tool.execute(argc, argv);
}
