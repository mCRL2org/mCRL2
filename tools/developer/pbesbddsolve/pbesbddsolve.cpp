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
    bdd::bdd_granularity granularity = bdd::bdd_granularity::per_pbes;
    bool apply_sylvan_optimization = true;
    bool remove_unreachable_vertices = false;

    // Lace options
    std::size_t lace_n_workers = 0; // autodetect
    std::size_t lace_dqsize = 1024*1024*4; // set large default
    std::size_t lace_stacksize = 0; // use default

    // Sylvan options
    std::size_t min_tablesize = 22;
    std::size_t max_tablesize = 26;
    std::size_t min_cachesize = 22;
    std::size_t max_cachesize = 26;

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("unary-encoding", utilities::make_optional_argument("NAME", "0"), "use a unary encoding of the predicate variables", 'u');
      desc.add_option("granularity", utilities::make_optional_argument("NAME", "pbes"), "the granularity of the edge relation (pbes, equation or summand)", 'g');
      desc.add_option("no-sylvan-optimization", "disable the Sylvan optimization for applying a relation", 'o');
      desc.add_option("remove-unreachable-vertices", "remove unreachable vertices before applying Zielonka", 'r');
      desc.add_option("lace-workers", utilities::make_optional_argument("NAME", "0"), "set number of Lace workers (threads for parallelization), (0=autodetect)");
      desc.add_option("lace-dqsize", utilities::make_optional_argument("NAME", "4194304"), "set length of Lace task queue (default 1024*1024*4)");
      desc.add_option("lace-stacksize", utilities::make_optional_argument("NAME", "0"), "set size of program stack in kilo bytes (0=default stack size)");
      desc.add_option("min-table-size", utilities::make_optional_argument("NAME", "22"), "minimum Sylvan table size (21-27, default 22)");
      desc.add_option("max-table-size", utilities::make_optional_argument("NAME", "26"), "maximum Sylvan table size (21-27, default 26)");
      desc.add_option("min-cache-size", utilities::make_optional_argument("NAME", "22"), "minimum Sylvan cache size (21-27, default 22)");
      desc.add_option("max-cache-size", utilities::make_optional_argument("NAME", "26"), "maximum Sylvan cache size (21-27, default 26)");
    }

    bdd::bdd_granularity parse_granularity(const std::string& value) const
    {
      if (value == "pbes")
      {
        return bdd::bdd_granularity::per_pbes;
      }
      else if (value == "equation")
      {
        return bdd::bdd_granularity::per_equation;
      }
      else if (value == "summand")
      {
        return bdd::bdd_granularity::per_summand;
      }
      throw mcrl2::runtime_error("unknown granularity value " + value);
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      unary_encoding = parser.has_option("unary-encoding");
      granularity = parse_granularity(parser.option_argument("granularity"));
      apply_sylvan_optimization = !parser.has_option("no-sylvan-optimization");
      remove_unreachable_vertices = parser.has_option("remove-unreachable-vertices");
      if (parser.has_option("lace-workers"))
      {
        lace_n_workers = parser.option_argument_as<int>("lace-workers");
      }
      if (parser.has_option("lace-dqsize"))
      {
        lace_dqsize = parser.option_argument_as<int>("lace-dqsize");
      }
      if (parser.has_option("lace-stacksize"))
      {
        lace_stacksize = parser.option_argument_as<int>("lace-stacksize");
      }
      if (parser.has_option("min-table-size"))
      {
        min_tablesize = parser.option_argument_as<std::size_t>("min-table-size");
      }
      if (parser.has_option("max-table-size"))
      {
        max_tablesize = parser.option_argument_as<std::size_t>("max-table-size");
      }
      if (parser.has_option("min-cache-size"))
      {
        min_cachesize = parser.option_argument_as<std::size_t>("min-cache-size");
      }
      if (parser.has_option("max-cache-size"))
      {
        max_cachesize = parser.option_argument_as<std::size_t>("max-cache-size");
      }
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
      // Standard Lace initialization with 1 worker
      // lace_init(1, 0);
      // lace_startup(0, NULL, NULL);
      lace_init(lace_n_workers, lace_dqsize);
      lace_startup(lace_stacksize, nullptr, nullptr);

      // Initialize Sylvan
      // With starting size of the nodes table 1 << 21, and maximum size 1 << 27.
      // With starting size of the cache table 1 << 20, and maximum size 1 << 20.
      // Memory usage: 24 bytes per node, and 36 bytes per cache bucket
      // - 1<<24 nodes: 384 MB
      // - 1<<25 nodes: 768 MB
      // - 1<<26 nodes: 1536 MB
      // - 1<<27 nodes: 3072 MB
      // - 1<<24 cache: 576 MB
      // - 1<<25 cache: 1152 MB
      // - 1<<26 cache: 2304 MB
      // - 1<<27 cache: 4608 MB
      sylvan::sylvan_set_sizes(1LL<<min_tablesize, 1LL<<max_tablesize, 1LL<<min_cachesize, 1LL<<max_cachesize);
      sylvan::sylvan_init_package();

      // Initialize the BDD module with granularity 1 (cache every operation)
      // A higher granularity (e.g. 6) often results in better performance in practice
      sylvan::sylvan_init_bdd();

      bdd::bdd_sylvan sylvan;
      pbes_system::pbes pbesspec = pbes_system::detail::load_pbes(input_filename());
      normalize(pbesspec);
      srf_pbes p = pbes2srf(pbesspec);
      if (remove_unreachable_vertices)
      {
        p.make_total();
      }
      unify_parameters(p);
      bool result = pbes_system::bdd::pbesbddsolve(p, sylvan, unary_encoding, granularity, &timer()).run(apply_sylvan_optimization, remove_unreachable_vertices);
      std::cout << (result ? "true" : "false") << std::endl;
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbesbddsolve_tool().execute(argc, argv);
}
