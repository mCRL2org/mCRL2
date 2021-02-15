// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsreach.cpp

#include <chrono>
#include <iomanip>
#include <boost/dynamic_bitset.hpp>
#include <sylvan_ldd.hpp>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/symbolic_reachability.h"
#include "mcrl2/utilities/parse_numbers.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(const lps::action_summand& summand, const std::set<data::variable>& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;

  std::set<data::variable> read_parameters = set_union(data::find_free_variables(summand.condition()), lps::find_free_variables(summand.multi_action()));
  std::set<data::variable> write_parameters;

  for (const auto& assignment: summand.assignments())
  {
    if (assignment.lhs() != assignment.rhs())
    {
      write_parameters.insert(assignment.lhs());
      data::find_free_variables(assignment.rhs(), std::inserter(read_parameters, read_parameters.end()));
    }
  }

  return {set_intersection(read_parameters, process_parameters), set_intersection(write_parameters, process_parameters) };
}

inline
std::map<data::variable, std::size_t> process_parameter_index(const lps::specification& lpsspec)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: lpsspec.process().process_parameters())
  {
    result[v] = i++;
  }
  return result;
}

inline
std::vector<boost::dynamic_bitset<>> read_write_patterns(const lps::specification& lpsspec)
{
  using utilities::as_set;

  std::vector<boost::dynamic_bitset<>> result;

  auto process_parameters = as_set(lpsspec.process().process_parameters());
  std::size_t n = process_parameters.size();
  std::map<data::variable, std::size_t> index = process_parameter_index(lpsspec);

  for (const auto& summand: lpsspec.process().action_summands())
  {
    auto [read_parameters, write_parameters] = read_write_parameters(summand, process_parameters);
    auto read = lps::parameter_indices(read_parameters, index);
    auto write = lps::parameter_indices(write_parameters, index);
    boost::dynamic_bitset<> rw(2*n);
    for (std::size_t j: read)
    {
      rw[2*j] = true;
    }
    for (std::size_t j: write)
    {
      rw[2*j + 1] = true;
    }
    result.push_back(rw);
  }

  return result;
}

struct summand_group: public lps::summand_group
{
  summand_group(const lps::specification& lpsspec, const std::set<std::size_t>& group_indices, const boost::dynamic_bitset<>& group_pattern, const std::vector<boost::dynamic_bitset<>>& read_write_patterns)
    : lps::summand_group(lpsspec.process().process_parameters(), group_pattern)
  {
    using lps::project;
    using utilities::as_vector;
    using utilities::as_set;
    using utilities::detail::set_union;

    std::set<std::size_t> used;
    for (std::size_t j: read)
    {
      used.insert(2*j);
    }
    for (std::size_t j: write)
    {
      used.insert(2*j + 1);
    }

    const auto& lps_summands = lpsspec.process().action_summands();
    const auto& process_parameters = lpsspec.process().process_parameters();

    for (std::size_t i: group_indices)
    {
      std::vector<int> copy;
      for (std::size_t j: used)
      {
        bool b = read_write_patterns[i][j];
        copy.push_back(b ? 0 : 1);
      }
      const auto& smd = lps_summands[i];
      summands.emplace_back(smd.condition(), smd.summation_variables(), project(as_vector(smd.next_state(process_parameters)), write), copy);
    }
  }
};

class lpsreach_algorithm
{
  using ldd = sylvan::ldds::ldd;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

  template <typename Context>
  friend void lps::learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context);

  protected:
    const lps::symbolic_reachability_options& m_options;
    data::rewriter m_rewr;
    data::mutable_indexed_substitution<> m_sigma;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<> m_enumerator;
    data::variable_list m_process_parameters;
    std::size_t m_n;
    std::vector<lps::data_expression_index> m_data_index;
    std::vector<summand_group> m_summand_groups;
    data::data_expression_list m_initial_state;

    ldd state2ldd(const data::data_expression_list& x)
    {
      std::uint32_t v[x.size()]; // TODO: avoid this C99 construction
      auto vi = v;
      auto di = m_data_index.begin();
      auto xi = x.begin();
      for (; di != m_data_index.end(); ++vi, ++di, ++xi)
      {
        *vi = di->index(*xi);
      }
      return sylvan::ldds::cube(v, x.size());
    };

    // R.L := R.L U {(x,y) in R | x in X}
    void learn_successors(std::size_t i, summand_group& R, const ldd& X)
    {
      mCRL2log(log::debug) << "learn successors of summand group " << i << " for X = " << print_states(m_data_index, X, R.read) << std::endl;

      using namespace sylvan::ldds;
      std::pair<lpsreach_algorithm&, summand_group&> context{*this, R};
      sat_all_nopar(X, lps::learn_successors_callback<std::pair<lpsreach_algorithm&, summand_group&>>, &context);
    }

    template <typename Specification>
    Specification preprocess(const Specification& lpsspec)
    {
      Specification result = lpsspec;
      lps::detail::instantiate_global_variables(result);
      lps::order_summand_variables(result);
      resolve_summand_variable_name_clashes(result); // N.B. This is a required preprocessing step.
      if (m_options.one_point_rule_rewrite)
      {
        one_point_rule_rewrite(result);
      }
      if (m_options.replace_constants_by_variables)
      {
        replace_constants_by_variables(result, m_rewr, m_sigma);
      }
      return result;
    }

  public:
    lpsreach_algorithm(const lps::specification& lpsspec, const lps::symbolic_reachability_options& options_)
      : m_options(options_),
        m_rewr(lps::construct_rewriter(lpsspec.data(), m_options.rewrite_strategy, lps::find_function_symbols(lpsspec), m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, lpsspec.data(), m_rewr, m_id_generator, false)
    {
      lps::specification lpsspec_ = preprocess(lpsspec);

      m_process_parameters = lpsspec_.process().process_parameters();
      m_n = m_process_parameters.size();
      m_initial_state = lpsspec_.initial_process().expressions();

      mCRL2log(log::debug) << "process parameters = " << core::detail::print_list(m_process_parameters) << std::endl;
      mCRL2log(log::debug) << "initial state = " << core::detail::print_list(m_initial_state) << std::endl;

      for (const data::variable& param: m_process_parameters)
      {
        m_data_index.push_back(lps::data_expression_index(param.sort()));
      }

      std::vector<boost::dynamic_bitset<>> patterns = read_write_patterns(lpsspec_);
      mCRL2log(log::debug) << lps::print_read_write_patterns(patterns);
      lps::adjust_read_write_patterns(patterns, m_options);
      std::vector<std::set<std::size_t>> groups = utilities::parse_summand_groups(m_options.summand_groups, patterns.size());
      for (const auto& group: groups)
      {
        mCRL2log(log::debug) << "group " << core::detail::print_set(group) << std::endl;
      }
      std::vector<boost::dynamic_bitset<>> group_patterns = lps::compute_summand_group_patterns(patterns, groups);
      for (std::size_t j = 0; j < group_patterns.size(); j++)
      {
        m_summand_groups.emplace_back(lpsspec_, groups[j], group_patterns[j], patterns);
      }

      for (std::size_t i = 0; i < m_summand_groups.size(); i++)
      {
        mCRL2log(log::debug) << "=== summand group " << i << " ===\n" << m_summand_groups[i] << std::endl;
      }
    }

    ldd run()
    {
      using namespace sylvan::ldds;
      auto& R = m_summand_groups;
      std::size_t iteration_count = 0;

      mCRL2log(log::debug) << "initial state = " << core::detail::print_list(m_initial_state) << std::endl;

      auto start = std::chrono::steady_clock::now();
      ldd x = state2ldd(m_initial_state);
      std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
      ldd visited = x;
      ldd todo = x;
      mCRL2log(log::verbose) << "found " << std::setw(12) << satcount(visited) << " states after " << std::setw(3) << iteration_count << " iterations (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;

      while (todo != empty_set())
      {
        auto loop_start = std::chrono::steady_clock::now();
        iteration_count++;
        mCRL2log(log::debug) << "--- iteration " << iteration_count << " ---" << std::endl;
        mCRL2log(log::debug) << "todo = " << print_states(m_data_index, todo) << std::endl;
        for (std::size_t i = 0; i < R.size(); i++)
        {
          learn_successors(i, R[i], minus(project(todo, R[i].Ip), R[i].Ldomain));
          mCRL2log(log::debug) << "L =\n" << print_relation(m_data_index, R[i].L, R[i].read, R[i].write) << std::endl;
        }
        ldd todo1 = empty_set();
        for (std::size_t i = 0; i < R.size(); i++)
        {
          if (m_options.no_relprod)
          {
            ldd z = lps::alternative_relprod(todo, R[i]);
            mCRL2log(log::debug) << "relprod(" << i << ", todo) = " << print_states(m_data_index, z) << std::endl;
            todo1 = union_(z, todo1);
          }
          else
          {
            mCRL2log(log::debug) << "relprod(" << i << ", todo) = " << print_states(m_data_index, relprod(todo, R[i].L, R[i].Ir)) << std::endl;
            todo1 = relprod_union(todo, R[i].L, R[i].Ir, todo1);
          }
        }
        todo = minus(todo1, visited);
        visited = union_(visited, todo);
        elapsed_seconds = std::chrono::steady_clock::now() - loop_start;
        mCRL2log(log::verbose) << "found " << std::setw(12) << satcount(visited) << " states after " << std::setw(3) << iteration_count << " iterations (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
      }

      elapsed_seconds = std::chrono::steady_clock::now() - start;
      std::cout << "number of states = " << satcount(visited) << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;

      return visited;
    }

    const std::vector<summand_group>& summand_groups() const
    {
      return m_summand_groups;
    }
};

class lpsreach_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    lps::symbolic_reachability_options options;

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
      desc.add_hidden_option("test", "test the successor/predecessor computations");
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
      options.test                                  = parser.has_option("test");
      options.summand_groups                        = parser.option_argument("groups");
      options.rewrite_strategy                      = rewrite_strategy();
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
    lpsreach_tool()
      : super("lpsreach",
              "Wieger Wesselink",
              "applies a symbolic reachability algorithm to an LPS",
              "read an LPS from INFILE and write output to OUTFILE. If OUTFILE "
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

      lps::stochastic_specification stochastic_lpsspec;
      lps::load_lps(stochastic_lpsspec, input_filename());
      lps::specification lpsspec = lps::remove_stochastic_operators(stochastic_lpsspec);

      if (lpsspec.process().process_parameters().empty())
      {
        throw mcrl2::runtime_error("Processes without parameters are not supported");
      }

      lpsreach_algorithm algorithm(lpsspec, options);
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
  return lpsreach_tool().execute(argc, argv);
}
