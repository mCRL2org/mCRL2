// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesreach.cpp

#include <chrono>
#include <iomanip>
#include <sylvan_ldd.hpp>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/symbolic_pbessolve.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/container_utility.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class pbesreach_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    pbes_system::symbolic_reachability_options options;

    // Lace options
    std::size_t lace_n_workers = 1;
    std::size_t lace_dqsize = 1024*1024*4; // set large default
    std::size_t lace_stacksize = 0; // use default

    // Sylvan options
    std::size_t min_tablesize = 22;
    std::size_t max_tablesize = 25;
    std::size_t min_cachesize = 22;
    std::size_t max_cachesize = 25;

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("lace-workers", utilities::make_optional_argument("NAME", "0"), "set number of Lace workers (threads for parallelization), (0=autodetect)");
      desc.add_option("lace-dqsize", utilities::make_optional_argument("NAME", "4194304"), "set length of Lace task queue (default 1024*1024*4)");
      desc.add_option("lace-stacksize", utilities::make_optional_argument("NAME", "0"), "set size of program stack in kilo bytes (0=default stack size)");
      desc.add_option("min-table-size", utilities::make_optional_argument("NAME", "22"), "minimum Sylvan table size (21-27, default 22)");
      desc.add_option("max-table-size", utilities::make_optional_argument("NAME", "26"), "maximum Sylvan table size (21-27, default 26)");
      desc.add_option("min-cache-size", utilities::make_optional_argument("NAME", "22"), "minimum Sylvan cache size (21-27, default 22)");
      desc.add_option("max-cache-size", utilities::make_optional_argument("NAME", "26"), "maximum Sylvan cache size (21-27, default 26)");
      desc.add_hidden_option("no-remove-unused-rewrite-rules", "do not remove unused rewrite rules. ", 'u');
      desc.add_hidden_option("no-one-point-rule-rewrite", "do not apply the one point rule rewriter");
      desc.add_option("no-discard", "do not discard any parameters");
      desc.add_option("no-read", "do not discard only-read parameters");
      desc.add_option("no-write", "do not discard only-write parameters");
      desc.add_option("no-relprod", "use an inefficient alternative version of relprod (for debugging)");
      desc.add_option("groups", utilities::make_optional_argument("GROUPS", ""), "a list of summand groups separated by semicolons, e.g. '0; 1 3 4; 2 5");
      desc.add_option("total", "make the SRF PBES total", 't');
      desc.add_hidden_option("test", "test the successor/predecessor computations");
      desc.add_hidden_option("srf", utilities::make_optional_argument("NAME", ""), "save the preprocessed PBES in SRF format");
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.one_point_rule_rewrite                = !parser.has_option("no-one-point-rule-rewrite");
      options.remove_unused_rewrite_rules           = !parser.has_option("no-remove-unused-rewrite-rules");
      options.replace_constants_by_variables        = false; // This option cannot be used in the symbolic algorithm
      options.no_discard                            = parser.has_option("no-discard");
      options.no_discard_read                       = parser.has_option("no-read");
      options.no_discard_write                      = parser.has_option("no-write");
      options.no_relprod                            = parser.has_option("no-relprod");
      options.summand_groups                        = parser.option_argument("groups");
      options.make_total                            = parser.has_option("total");
      options.test                                  = parser.has_option("test");
      options.srf                                   = parser.option_argument("srf");
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
    pbesreach_tool()
      : super("pbesreach",
              "Wieger Wesselink",
              "applies a symbolic reachability algorithm to a PBES",
              "read a PBES from INFILE and write output to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      using namespace sylvan::ldds;

      lace_init(lace_n_workers, lace_dqsize);
      lace_startup(lace_stacksize, nullptr, nullptr);
      sylvan::sylvan_set_sizes(1LL<<min_tablesize, 1LL<<max_tablesize, 1LL<<min_cachesize, 1LL<<max_cachesize);
      sylvan::sylvan_init_package();
      sylvan::sylvan_init_ldd();

      mCRL2log(log::debug) << options << std::endl;

      pbes_system::pbes pbesspec = pbes_system::detail::load_pbes(input_filename());

      if (pbesspec.initial_state().empty())
      {
        throw mcrl2::runtime_error("PBESses without parameters are not supported");
      }

      pbes_system::pbesreach_algorithm algorithm(pbesspec, options);
      ldd V = algorithm.run();

      if (options.test)
      {
        lps::test_successor_predecessor(algorithm.summand_groups(), V, options.no_relprod);
      }

      sylvan::sylvan_quit();
      lace_exit();
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbesreach_tool().execute(argc, argv);
}
