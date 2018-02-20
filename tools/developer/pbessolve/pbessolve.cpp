// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbessolve.cpp

#include <iostream>
#include <string>

#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;
using mcrl2::bes::tools::pbes_input_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;

class pbessolve_tool: public rewriter_tool<pbes_input_tool<input_tool>>
{
  protected:
    typedef rewriter_tool<pbes_input_tool<input_tool>> super;

    transformation_strategy m_transformation_strategy; // The strategy to substitute the value of variables with
                                                       // a trivial rhs (aka true or false) in other equations when generating a BES.
    search_strategy m_search_strategy;                 // The search strategy (depth first/breadth first)

    bool check_strategy = false;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      check_strategy = parser.options.count("check-strategy") > 0;
    }

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("check-strategy", "do a sanity check on the computed strategy", 's');
    }

  public:
    pbessolve_tool()
      : super("pbessolve",
              "Wieger Wesselink",
              "Generate a BES from a PBES and solve it. ",
              "Solves (P)BES from INFILE. "
              "If INFILE is not present, stdin is used. "
             ),
      m_transformation_strategy(lazy),
      m_search_strategy(breadth_first)
    {}

    bool run()
    {
      pbes_system::pbes pbesspec;
      pbes_system::load_pbes(pbesspec, input_filename());
      structure_graph G;
      pbesinst_structure_graph(pbesspec, G, rewrite_strategy(), m_search_strategy, m_transformation_strategy);
      bool result = solve_structure_graph(G, check_strategy);
      std::cout << (result ? "true" : "false") << std::endl;
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbessolve_tool().execute(argc, argv);
}
