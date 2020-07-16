// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bdd.cpp

#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbesbddsolve.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/input_output_tool.h"
#include <sylvan.h>

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbesbddsolve_tool : public input_output_tool
{
  protected:
    typedef input_output_tool super;

    bool unary_encoding = false;

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      unary_encoding = parser.options.count("unary-encoding") > 0;
    }

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("unary-encoding", "use a unary encoding of the predicate variables", 'u');
    }

  public:
  pbesbddsolve_tool()
      : super("pbesbddsolve",
              "Wieger Wesselink",
              "solves a PBES in boolean format using BDDs",
              "Solves PBES from INFILE. "
              "If INFILE is not present, stdin is used. "
              "The PBES is transformed into a BDD, "
              "which is then solved using Zielonka's algorithm. "
             )
    {}

    bool run() override
    {
      pbes_system::pbes pbesspec = pbes_system::detail::load_pbes(input_filename());
      normalize(pbesspec);
      srf_pbes p = pbes2srf(pbesspec);
      unify_parameters(p);
      bdd::bdd_sylvan sylvan;
      bool result = pbes_system::bdd::pbesbddsolve(p, sylvan, unary_encoding).run();
      std::cout << (result ? "true" : "false") << std::endl;
      return true;
    }
};

int main(int argc, char* argv[])
{
  // Standard Lace initialization with 1 worker
  lace_init(1, 0);
  lace_startup(0, NULL, NULL);

  // Simple Sylvan initialization, also initialize BDD support
  sylvan::sylvan_set_sizes(1LL<<16, 1LL<<16, 1LL<<16, 1LL<<16);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_bdd();

  return pbesbddsolve_tool().execute(argc, argv);
}
