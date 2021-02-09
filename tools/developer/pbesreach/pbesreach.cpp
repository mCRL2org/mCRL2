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
#include <boost/dynamic_bitset.hpp>
#include <sylvan_ldd.hpp>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/symbolic_reachability.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/replace_constants_by_variables.h"
#include "mcrl2/pbes/resolve_name_clashes.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

// Returns a data specification containing a structured sort with the names of the propositional variables
// in the PBES as elements.
inline
data::data_specification construct_propositional_variable_data_specification(const pbes_system::srf_pbes& pbesspec, const std::string& sort_name)
{
  // TODO: it should be possible to add a structured sort to pbesspec.data() directly, but I don't know how
  std::vector<std::string> names;
  for (const auto& equation: pbesspec.equations())
  {
    names.push_back(equation.variable().name());
  }
  std::string text = "sort " + sort_name + " = struct " +   utilities::string_join(names, " | ") + ";";
  return data::parse_data_specification(text);
}

struct symbolic_reachability_options: public lps::symbolic_reachability_options
{
  bool make_total = false;
};

inline
std::ostream& operator<<(std::ostream& out, const symbolic_reachability_options& options)
{
  out << static_cast<lps::symbolic_reachability_options>(options);
  out << "total = " << std::boolalpha << options.make_total << std::endl;
  return out;
}

inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(
  const pbes_system::srf_equation& equation,
  const pbes_system::srf_summand& summand,
  const data::variable_list& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;
  using utilities::as_set;

  std::set<data::variable> read_parameters = data::find_free_variables(summand.condition());
  std::set<data::variable> write_parameters;

  // We need special handling for the first parameter, since the clause 'propvar == X' is not in the condition yet
  read_parameters.insert(process_parameters.front());
  if (equation.variable().name() != summand.variable().name())
  {
    write_parameters.insert(process_parameters.front());
  }

  const data::data_expression_list& expressions = summand.variable().parameters();
  auto pi = ++process_parameters.begin(); // skip the first parameter
  auto ei = expressions.begin();
  for (; ei != expressions.end(); ++pi, ++ei)
  {
    if (*pi != *ei)
    {
      write_parameters.insert(*pi);
      data::find_free_variables(*ei, std::inserter(read_parameters, read_parameters.end()));
    }
  }

  auto process_parameter_set = as_set(process_parameters);
  return { set_intersection(read_parameters, process_parameter_set), set_intersection(write_parameters, process_parameter_set) };
}

inline
std::map<data::variable, std::size_t> process_parameter_index(const data::variable_list& process_parameters)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: process_parameters)
  {
    result[v] = i++;
  }
  return result;
}

inline
std::vector<boost::dynamic_bitset<>> read_write_patterns(const pbes_system::srf_pbes& pbesspec, const data::variable_list& process_parameters)
{
  std::vector<boost::dynamic_bitset<>> result;

  std::size_t n = process_parameters.size();
  std::map<data::variable, std::size_t> index = process_parameter_index(process_parameters);

  for (const auto& equation: pbesspec.equations())
  {
    for (const auto& summand: equation.summands())
    {
      auto [read_parameters, write_parameters] = read_write_parameters(equation, summand, process_parameters);
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
  }

  return result;
}

inline
data::data_expression_list make_state(const pbes_system::propositional_variable_instantiation& x, const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map)
{
  data::data_expression_list result = x.parameters();
  result.push_front(propvar_map.at(x.name()));
  return result;
}

struct summand_group: public lps::summand_group
{
  summand_group(
    const pbes_system::srf_pbes& pbesspec,
    const data::variable_list& process_parameters,
    const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map,
    const std::set<std::size_t>& summand_group_indices,
    const boost::dynamic_bitset<>& read_write_pattern,
    const std::vector<boost::dynamic_bitset<>>& read_write_patterns
  )
    : lps::summand_group(process_parameters, read_write_pattern)
  {
    using lps::project;
    using utilities::as_vector;
    using utilities::detail::contains;

    const auto& equations = pbesspec.equations();
    std::size_t k = 0;
    for (std::size_t i = 0; i < equations.size(); i++)
    {
      const core::identifier_string& X_i = equations[i].variable().name();
      const auto& equation_summands = equations[i].summands();
      for (std::size_t j = 0; j < equation_summands.size(); j++, k++)
      {
        if (contains(summand_group_indices, k))
        {
          std::vector<int> copy(read.size(), 0);
          for (std::size_t q = 0; q < write.size(); q++)
          {
            bool r = read_write_pattern[2*q];
            bool w = read_write_pattern[2*q+1];
            bool rk = read_write_patterns[k][2*q];
            bool wk = read_write_patterns[k][2*q+1];
            copy.push_back(!rk && !wk && !r && w);
          }
          const pbes_system::srf_summand& smd = equation_summands[j];
          summands.emplace_back(data::and_(data::equal_to(process_parameters.front(), propvar_map.at(X_i)), smd.condition()), smd.parameters(), project(as_vector(make_state(smd.variable(), propvar_map)), write), copy);
        }
      }
    }
  }
};

class pbesreach_algorithm
{
  using ldd = sylvan::ldds::ldd;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

  template <typename Context>
  friend void lps::learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context);

  protected:
    const symbolic_reachability_options& m_options;
    pbes_system::srf_pbes m_pbes;
    data::rewriter m_rewr;
    mutable data::mutable_indexed_substitution<> m_sigma;
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
      std::pair<pbesreach_algorithm&, summand_group&> context{*this, R};
      sat_all_nopar(X, lps::learn_successors_callback<std::pair<pbesreach_algorithm&, summand_group&>>, &context);
    }

    pbes_system::srf_pbes preprocess(pbes_system::pbes pbesspec, bool make_total)
    {
      pbes_system::detail::instantiate_global_variables(pbesspec);
      normalize(pbesspec);
      if (m_options.one_point_rule_rewrite)
      {
        pbes_system::one_point_rule_rewriter R;
        pbes_system::replace_pbes_expressions(pbesspec, R, false);
      }
      if (m_options.replace_constants_by_variables)
      {
        pbes_system::replace_constants_by_variables(pbesspec, m_rewr, m_sigma);
      }

      pbes_system::srf_pbes result = pbes2srf(pbesspec);
      if (make_total)
      {
        result.make_total();
      }
      unify_parameters(result);
      pbes_system::resolve_summand_variable_name_clashes(result, result.equations().front().variable().parameters()); // N.B. This is a required preprocessing step.

      // add a sort for the propositional variable names
      data::data_specification propvar_dataspec = construct_propositional_variable_data_specification(result, "PropositionalVariable");
      result.data() = data::merge_data_specifications(result.data(), propvar_dataspec);
      mCRL2log(log::debug) << "--- srf pbes ---\n" << result.to_pbes() << std::endl;

      return result;
    }

  public:
    pbesreach_algorithm(const pbes_system::pbes& pbesspec, const symbolic_reachability_options& options_)
      : m_options(options_),
        m_pbes(preprocess(pbesspec, options_.make_total)),
        m_rewr(lps::construct_rewriter(m_pbes.data(), m_options.rewrite_strategy, pbes_system::find_function_symbols(pbesspec), m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, m_pbes.data(), m_rewr, m_id_generator, false)
    {
      data::basic_sort propvar_sort("PropositionalVariable");
      std::unordered_map<core::identifier_string, data::data_expression> propvar_map;
      for (const auto& equation: m_pbes.equations())
      {
        propvar_map[equation.variable().name()] = data::function_symbol(equation.variable().name(), propvar_sort);
      }
      m_process_parameters = m_pbes.equations().front().variable().parameters();
      m_process_parameters.push_front(data::variable("propvar", propvar_sort));
      m_n = m_process_parameters.size();
      m_initial_state = make_state(m_pbes.initial_state(), propvar_map);

      mCRL2log(log::debug) << "process parameters = " << core::detail::print_list(m_process_parameters) << std::endl;
      mCRL2log(log::debug) << "initial state = " << core::detail::print_list(m_initial_state) << std::endl;

      for (const data::variable& param: m_process_parameters)
      {
        m_data_index.push_back(lps::data_expression_index(param.sort()));
      }

      std::vector<boost::dynamic_bitset<>> patterns = read_write_patterns(m_pbes, m_process_parameters);
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
        m_summand_groups.emplace_back(m_pbes, m_process_parameters, propvar_map, groups[j], group_patterns[j], group_patterns);
      }

      for (std::size_t i = 0; i < m_summand_groups.size(); i++)
      {
        mCRL2log(log::debug) << "=== summand group " << i << " ===\n" << m_summand_groups[i] << std::endl;
      }
    }

    void run()
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
    }
};

class pbesreach_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    symbolic_reachability_options options;

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
      options.make_total           = parser.has_option("total");
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

      pbesreach_algorithm algorithm(pbesspec, options);
      algorithm.run();

      sylvan::sylvan_quit();
      lace_exit();
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbesreach_tool().execute(argc, argv);
}
