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
#include <sylvan_ldd.hpp>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/explorer_options.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/utilities/detail/io.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

namespace std
{

template <>
struct hash<mcrl2::data::sort_expression>
{
  std::size_t operator()(const mcrl2::data::sort_expression& x) const
  {
    return hash<atermpp::aterm>()(x);
  }
};

} // namespace std

namespace sylvan::ldds {
//-----------------------------------------------------------------------------------//
// LDD functions
//-----------------------------------------------------------------------------------//
inline
void ldd_solutions_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context = nullptr)
{
  std::vector<std::vector<std::uint32_t>>& V = *reinterpret_cast<std::vector<std::vector<std::uint32_t>>*>(context);
  V.push_back(std::vector<std::uint32_t>(v, v + n));
}

inline
std::vector<std::vector<std::uint32_t>> ldd_solutions(const sylvan::ldds::ldd& x)
{
  std::vector<std::vector<std::uint32_t>> result;
  sat_all(x, ldd_solutions_callback, &result);
  return result;
}

inline
std::string print_ldd(const sylvan::ldds::ldd& x)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  bool multiline = solutions.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < solutions.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }
    out << core::detail::print_list(solutions[i]);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

} // sylvan::ldds

//-----------------------------------------------------------------------------------//

inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(const lps::action_summand& summand, const std::set<data::variable>& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;

  std::set<data::variable> used_read_variables = set_union(data::find_free_variables(summand.condition()), lps::find_free_variables(summand.multi_action()));
  std::set<data::variable> used_write_variables;

  for (const auto& assignment: summand.assignments())
  {
    if (assignment.lhs() != assignment.rhs())
    {
      data::find_all_variables(assignment.lhs(), std::inserter(used_write_variables, used_write_variables.end()));
      data::find_all_variables(assignment.rhs(), std::inserter(used_read_variables, used_read_variables.end()));
    }
  }

  return { set_intersection(used_read_variables, process_parameters), set_intersection(used_write_variables, process_parameters) };
}

/// \brief A bidirectional mapping between data expressions of a given sort and numbers
class data_expression_index
{
    friend std::ostream& operator<<(std::ostream&, const data_expression_index&);

  protected:
    data::sort_expression m_sort;
    std::unordered_map<data::data_expression, std::uint32_t> m_value2index;
    std::vector<data::data_expression> m_values;

  public:
    data_expression_index(const data::sort_expression& sort)
      : m_sort(sort)
    {}

    /// \brief Returns the index of the given value. If the value is not present yet, it will be added.
    std::uint32_t index(const data::data_expression& value)
    {
      assert(value.sort() == m_sort);
      auto i = m_value2index.find(value);
      if (i != m_value2index.end())
      {
        return i->second;
      }
      std::uint32_t index = m_values.size();
      m_value2index[value] = index;
      m_values.push_back(value);
      return index;
    }

    /// \brief Returns the value corresponding to index
    const data::data_expression& value(std::uint32_t i) const
    {
      return m_values[i];
    }

    const data::sort_expression& sort() const
    {
      return m_sort;
    }

    bool has_value(const data::data_expression& value) const
    {
      return m_value2index.find(value) != m_value2index.end();
    }

    bool has_index(std::uint32_t i) const
    {
      return i < m_values.size();
    }
};

inline
std::ostream& operator<<(std::ostream& out, const data_expression_index& x)
{
  out << "data index with sort = " << x.sort() << " values = " << core::detail::print_list(x.m_values);
  return out;
}

struct lps_summand
{
  data::data_expression condition;
  data::variable_list condition_variables; // the free variables of the condition
  std::vector<data::data_expression> next_state; // the projected next state vector
  std::vector<data::variable> read_parameters; // the read parameters
  std::vector<std::size_t> read; // indices of the read parameters
  std::vector<std::size_t> read_pos; // indices of the read parameters in a zipped transition xy
  std::vector<data::variable> write_parameters; // the write parameters
  std::vector<std::size_t> write; // indices of the write parameters
  std::vector<std::size_t> write_pos; // indices of the write parameters in a zipped transition xy
  sylvan::ldds::ldd L; // the projected transition relation
  sylvan::ldds::ldd Ldomain; // the domain of L
  sylvan::ldds::ldd Ir; // meta data needed by sylvan::ldds::relprod
  sylvan::ldds::ldd Ip; // meta data needed by sylvan::ldds::project

  void compute_read_write_pos()
  {
    auto ri = read.begin();
    auto wi = write.begin();
    std::size_t index = 0;
    while (ri != read.end() && wi != write.end())
    {
      if (*ri <= *wi)
      {
        read_pos.push_back(index++);
        ri++;
      }
      else
      {
        write_pos.push_back(index++);
        wi++;
      }
    }
    while (ri != read.end())
    {
      read_pos.push_back(index++);
      ri++;
    }
    while (wi != write.end())
    {
      write_pos.push_back(index++);
      wi++;
    }
  }
};

inline
std::ostream& operator<<(std::ostream& out, const lps_summand& x)
{
  using namespace sylvan::ldds;
  out << "condition = " << x.condition << std::endl;
  out << "condition variables = " << core::detail::print_list(x.condition_variables) << std::endl;
  out << "read = " << core::detail::print_list(x.read) << std::endl;
  out << "read parameters = " << core::detail::print_list(x.read_parameters) << std::endl;
  out << "read_pos = " << core::detail::print_list(x.read_pos) << std::endl;
  out << "write = " << core::detail::print_list(x.write) << std::endl;
  out << "write parameters = " << core::detail::print_list(x.write_parameters) << std::endl;
  out << "write_pos = " << core::detail::print_list(x.write_pos) << std::endl;
  out << "next state = " << core::detail::print_list(x.next_state) << std::endl;
  out << "L = " << print_ldd(x.L) << std::endl;
  out << "Ir = " << print_ldd(x.Ir) << std::endl;
  out << "Ip = " << print_ldd(x.Ip) << std::endl;
  return out;
}

template <typename T>
std::vector<T> project(const std::vector<T>& v, const std::vector<std::size_t>& used)
{
  std::vector<T> result;
  result.reserve(used.size());
  for (std::size_t i = 0; i < used.size(); i++)
  {
    result.push_back(v[used[i]]);
  }
  return result;
}

template <typename T>
std::vector<T> as_vector(const atermpp::term_list<T>& x)
{
  return std::vector<T>(x.begin(), x.end());
}

lps_summand make_summand(const lps::action_summand& summand, const data::variable_list& process_parameters, bool discard_read, bool discard_write)
{
  lps_summand result;

  std::set<data::variable> process_parameter_set(process_parameters.begin(), process_parameters.end());
  auto [read_parameters, write_parameters] = read_write_parameters(summand, process_parameter_set);

  std::vector<std::uint32_t> Ir; // used parameter meta data for relprod
  std::vector<std::uint32_t> Ip; // used parameter meta data for project
  std::size_t j = 0;
  for (const data::variable& param: process_parameters)
  {
    bool read = read_parameters.find(param) != read_parameters.end();
    bool write = write_parameters.find(param) != write_parameters.end();

    if (read || write)
    {
      if (!discard_read)
      {
        read = true;
      }
      if (!discard_write)
      {
        write = true;
      }
    }

    Ip.push_back(read ? 1 : 0);

    if (read)
    {
      result.read.push_back(j);
      Ir.push_back(1);
    }

    if (write)
    {
      result.write.push_back(j);
      Ir.push_back(read ? 2 : 4);
    }

    if (!read && !write)
    {
      Ir.push_back(0);
    }
    j++;
  }

  result.condition = summand.condition();
  result.condition_variables = summand.summation_variables();
  result.next_state = project(as_vector(summand.next_state(process_parameters)), result.write);
  result.read_parameters = project(as_vector(process_parameters), result.read);
  result.write_parameters = project(as_vector(process_parameters), result.write);
  result.L = sylvan::ldds::empty_set();
  result.Ldomain = sylvan::ldds::empty_set();
  result.Ir = sylvan::ldds::cube(Ir);
  result.Ip = sylvan::ldds::cube(Ip);
  result.compute_read_write_pos();

  return result;
}

std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < x.size(); i++)
  {
    result.push_back(data_index[i].value(x[i]));
  }
  return result;
}

std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x, const std::vector<std::size_t>& used)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < used.size(); i++)
  {
    result.push_back(data_index[used[i]].value(x[i]));
  }
  return result;
}

inline
std::string print_state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x)
{
  return core::detail::print_list(ldd2state(data_index, x));
}

inline
std::string print_state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x, const std::vector<std::size_t>& used)
{
  return core::detail::print_list(ldd2state(data_index, x, used));
}

inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  bool multiline = solutions.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < solutions.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }
    out << print_state(data_index, solutions[i]);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x, const std::vector<std::size_t>& used)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  bool multiline = solutions.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < solutions.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }
    out << print_state(data_index, solutions[i], used);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

std::string print_transition(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& xy)
{
  std::size_t n = xy.size() / 2;
  std::vector<std::uint32_t> x(n);
  std::vector<std::uint32_t> y(n);
  for (std::size_t i = 0; i < n; i++)
  {
    x[i] = xy[2*i];
    y[i] = xy[2*i+1];
  }
  return print_state(data_index, x) + " -> " + print_state(data_index, y);
}

std::string print_transition(const std::vector<data_expression_index>& data_index, const std::uint32_t* xy, const std::vector<std::size_t>& read, const std::vector<std::size_t>& write)
{
  std::vector<std::uint32_t> x;
  std::vector<std::uint32_t> y;
  auto ri = read.begin();
  auto wi = write.begin();
  auto xyi = xy;
  while (ri != read.end() && wi != write.end())
  {
    if (*ri <= *wi)
    {
      ri++;
      x.push_back(*xyi++);
    }
    else
    {
      wi++;
      y.push_back(*xyi++);
    }
  }
  while (ri != read.end())
  {
    ri++;
    x.push_back(*xyi++);
  }
  while (wi != write.end())
  {
    wi++;
    y.push_back(*xyi++);
  }
  return print_state(data_index, x, read) + " -> " + print_state(data_index, y, write);
}

std::string print_relation(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& R, const std::vector<std::size_t>& read, const std::vector<std::size_t>& write)
{
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R))
  {
    out << print_transition(data_index, xy.data(), read, write) << std::endl;
  }
  return out.str();
}

void learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context = nullptr);

// A very inefficient implementation of relprod, that matches the specification closely
sylvan::ldds::ldd alternative_relprod(const sylvan::ldds::ldd& todo, const lps_summand& R)
{
  using namespace sylvan::ldds;

  auto split = [&](const std::vector<std::uint32_t>& xy)
  {
    std::vector<std::uint32_t> x;
    std::vector<std::uint32_t> y;
    for (std::size_t j: R.read_pos)
    {
      x.push_back(xy[j]);
    }
    for (std::size_t j: R.write_pos)
    {
      y.push_back(xy[j]);
    }
    return std::make_pair(x, y);
  };

  auto match = [&](const std::vector<std::uint32_t>& x, const std::vector<std::uint32_t>& x_)
  {
    for (std::size_t j = 0; j < x_.size(); j++)
    {
      if (x[R.read[j]] != x_[j])
      {
        return false;
      }
    }
    return true;
  };

  auto replace = [&](std::vector<std::uint32_t> x, const std::vector<std::uint32_t>& y_)
  {
    for (std::size_t j = 0; j < y_.size(); j++)
    {
      x[R.write[j]] = y_[j];
    }
    return x;
  };

  ldd result = empty_set();

  auto todo_elements = ldd_solutions(todo);
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R.L))
  {
    auto [x_, y_] = split(xy);
    for (const auto& x: todo_elements)
    {
      if (match(x, x_))
      {
        auto y = replace(x, y_);
        result = union_cube(result, y);
      }
    }
  }
  return result;
  // return relprod(todo, R.L, R.Ir);
}

class lpsreach_algorithm
{
  using ldd = sylvan::ldds::ldd;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;
  friend void learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context);

  protected:
    const lps::explorer_options& m_options; // TODO: make a separate lpsreach_options class
    data::rewriter m_rewr;
    mutable data::mutable_indexed_substitution<> m_sigma;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<> m_enumerator;
    data::variable_list m_process_parameters;
    std::size_t m_n;
    std::vector<data_expression_index> m_data_index;
    std::vector<lps_summand> m_summands;
    data::data_expression_list m_initial_state;
    bool m_discard_read;
    bool m_discard_write;
    bool m_use_alternative_relprod;

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
    void learn_successors(std::size_t i, lps_summand& R, const ldd& X)
    {
      mCRL2log(log::debug) << "learn successors of summand " << i << " for X = " << print_states(m_data_index, X, R.read) << std::endl;

      using namespace sylvan::ldds;
      std::pair<lpsreach_algorithm&, lps_summand&> context{*this, R};
      sat_all_nopar(X, learn_successors_callback, &context);
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

    // Add operations on reals that are needed for the exploration.
    std::set<data::function_symbol> add_real_operators(std::set<data::function_symbol> s) const
    {
      std::set<data::function_symbol> result = std::move(s);
      result.insert(data::less_equal(data::sort_real::real_()));
      result.insert(data::greater_equal(data::sort_real::real_()));
      result.insert(data::sort_real::plus(data::sort_real::real_(), data::sort_real::real_()));
      return result;
    }

    data::rewriter construct_rewriter(const lps::specification& lpsspec, bool remove_unused_rewrite_rules)
    {
      if (remove_unused_rewrite_rules)
      {
        return data::rewriter(lpsspec.data(),
                              data::used_data_equation_selector(lpsspec.data(), add_real_operators(lps::find_function_symbols(lpsspec)), lpsspec.global_variables()),
                              m_options.rewrite_strategy);
      }
      else
      {
        return data::rewriter(lpsspec.data(), m_options.rewrite_strategy);
      }
    }

  public:
    lpsreach_algorithm(const lps::specification& lpsspec, const lps::explorer_options& options_, bool discard_read, bool discard_write, bool use_alternative_relprod = false)
      : m_options(options_),
        m_rewr(construct_rewriter(lpsspec, m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, lpsspec.data(), m_rewr, m_id_generator, false),
        m_discard_read(discard_read), m_discard_write(discard_write), m_use_alternative_relprod(use_alternative_relprod)
    {
      lps::specification lpsspec_ = preprocess(lpsspec);
      m_process_parameters = lpsspec_.process().process_parameters();
      m_n = m_process_parameters.size();
      m_initial_state = lpsspec_.initial_process().expressions();

      for (const data::variable& param: m_process_parameters)
      {
        m_data_index.push_back(data_expression_index(param.sort()));
      }

      for (const lps::action_summand& summand: lpsspec_.process().action_summands())
      {
        m_summands.push_back(make_summand(summand, m_process_parameters, m_discard_read, m_discard_write));
      }

      for (std::size_t i = 0; i < m_summands.size(); i++)
      {
        mCRL2log(log::debug) << "=== summand " << i << " ===\n" << m_summands[i] << std::endl;
      }
    }

    void run()
    {
      using namespace sylvan::ldds;
      auto& R = m_summands;
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
        start = std::chrono::steady_clock::now();
        iteration_count++;
        mCRL2log(log::debug) << "--- iteration " << ++iteration_count << " ---" << std::endl;
        mCRL2log(log::debug) << "todo = " << print_states(m_data_index, todo) << std::endl;
        for (std::size_t i = 0; i < R.size(); i++)
        {
          learn_successors(i, R[i], minus(project(todo, R[i].Ip), R[i].Ldomain));
          mCRL2log(log::debug) << "L =\n" << print_relation(m_data_index, R[i].L, R[i].read, R[i].write) << std::endl;
        }
        ldd todo1 = empty_set();
        for (std::size_t i = 0; i < R.size(); i++)
        {
          if (m_use_alternative_relprod)
          {
            ldd z = alternative_relprod(todo, R[i]);
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
        elapsed_seconds = std::chrono::steady_clock::now() - start;
        mCRL2log(log::verbose) << "found " << std::setw(12) << satcount(visited) << " states after " << std::setw(3) << iteration_count << " iterations (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
      }

      std::cout << "number of states = " << satcount(visited) << std::endl;
    }
};

template <typename EnumeratorElement>
void check_enumerator_solution(const EnumeratorElement& p, const lps_summand& summand)
{
  if (p.expression() != data::sort_bool::true_())
  {
    throw data::enumerator_error("Expression does not rewrite to true or false");
  }
}

void learn_successors_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{
  using namespace sylvan::ldds;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

  auto p = reinterpret_cast<std::pair<lpsreach_algorithm&, lps_summand&>*>(context);
  auto& algorithm = p->first;
  auto& summand = p->second;
  auto& sigma = algorithm.m_sigma;
  const auto& rewr = algorithm.m_rewr;
  auto& data_index = algorithm.m_data_index;
  const auto& enumerator = algorithm.m_enumerator;
  std::size_t x_size = summand.read.size();
  std::size_t y_size = summand.write.size();
  std::size_t xy_size = x_size + y_size;
  std::uint32_t xy[xy_size]; // TODO: avoid this C99 construction

  // add the assignments corresponding to x to sigma
  // add x to the transition xy
  for (std::size_t j = 0; j < x_size; j++)
  {
    sigma[summand.read_parameters[j]] = data_index[summand.read[j]].value(x[j]);
    xy[summand.read_pos[j]] = x[j];
  }

  data::data_expression condition = rewr(summand.condition, sigma);
  if (!data::is_false(condition))
  {
    enumerator.enumerate(enumerator_element(summand.condition_variables, condition),
                         sigma,
                         [&](const enumerator_element& p) {
                           check_enumerator_solution(p, summand);
                           p.add_assignments(summand.condition_variables, sigma, rewr);
                           for (std::size_t j = 0; j < y_size; j++)
                           {
                             xy[summand.write_pos[j]] = data_index[summand.write[j]].index(rewr(summand.next_state[j], sigma));
                           }
                           mCRL2log(log::debug) << "  " << print_transition(data_index, xy, summand.read, summand.write) << std::endl;
                           summand.Ldomain = union_cube(summand.Ldomain, x, x_size);
                           summand.L = union_cube(summand.L, xy, xy_size);
                           return false;
                         },
                         data::is_false
    );
  }
  data::remove_assignments(sigma, summand.condition_variables);
  data::remove_assignments(sigma, summand.read_parameters);
}

class lpsreach_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    lps::explorer_options options;
    bool discard_read;
    bool discard_write;
    bool use_alternative_relprod;

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
      desc.add_option("no-projections", "do not use projections on used parameters");
      desc.add_option("read", "discard read independent parameters to increase efficiency");
      desc.add_option("write", "discard write independent parameters to increase efficiency");
      desc.add_option("relprod", "use an inefficient alternative version of relprod (for debugging)");
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.one_point_rule_rewrite                = !parser.has_option("no-one-point-rule-rewrite");
      options.remove_unused_rewrite_rules           = !parser.has_option("no-remove-unused-rewrite-rules");
      options.replace_constants_by_variables        = false; // This option cannot be used in the symbolic algorithm
      discard_read                                  = parser.has_option("read");
      discard_write                                 = parser.has_option("write");
      use_alternative_relprod                       = parser.has_option("relprod");
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
      if (discard_write && !discard_read && !use_alternative_relprod)
      {
        mCRL2log(log::warning) << "WARNING: the option --write doesn't work correctly without --read, unless --relprod is set" << std::endl;
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
      lace_init(lace_n_workers, lace_dqsize);
      lace_startup(lace_stacksize, nullptr, nullptr);
      sylvan::sylvan_set_sizes(1LL<<min_tablesize, 1LL<<max_tablesize, 1LL<<min_cachesize, 1LL<<max_cachesize);
      sylvan::sylvan_init_package();
      sylvan::sylvan_init_ldd();

      lps::stochastic_specification stochastic_lpsspec;
      lps::load_lps(stochastic_lpsspec, input_filename());
      lps::specification lpsspec = lps::remove_stochastic_operators(stochastic_lpsspec);

      if (lpsspec.process().process_parameters().empty())
      {
        throw mcrl2::runtime_error("Processes without parameters are not supported");
      }

      lpsreach_algorithm algorithm(lpsspec, options, discard_read, discard_write, use_alternative_relprod);
      algorithm.run();

      sylvan::sylvan_quit();
      lace_exit();
      return true;
    }
};

int main(int argc, char* argv[])
{
  return lpsreach_tool().execute(argc, argv);
}
