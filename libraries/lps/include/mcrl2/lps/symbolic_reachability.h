// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/symbolic_reachability.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_SYMBOLIC_REACHABILITY_H
#define MCRL2_LPS_SYMBOLIC_REACHABILITY_H

#include <algorithm>
#include <iterator>
#include <random>
#include <sylvan_ldd.hpp>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/parse_numbers.h"

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
ldd make_ldd(const std::vector<std::vector<std::uint32_t>>& V)
{
  ldd result = empty_set();
  for (const auto& v: V)
  {
    result = union_cube(result, v);
  }
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
    out << mcrl2::core::detail::print_list(solutions[i]);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

} // sylvan::ldds

namespace mcrl2 {

namespace utilities {

template <typename T>
std::vector<T> as_vector(const atermpp::term_list<T>& x)
{
  return std::vector<T>(x.begin(), x.end());
}

template <typename T>
std::vector<T> as_vector(const std::set<T>& x)
{
  return std::vector<T>(x.begin(), x.end());
}

template <typename T>
std::set<T> as_set(const atermpp::term_list<T>& x)
{
  return std::set<T>(x.begin(), x.end());
}

template <typename T>
std::set<T> as_set(const std::vector<T>& x)
{
  return std::set<T>(x.begin(), x.end());
}

inline
std::vector<std::set<std::size_t>> parse_summand_groups(std::string text, std::size_t n)
{
  using utilities::regex_split;
  using utilities::trim_copy;
  using utilities::as_set;

  std::vector<std::set<std::size_t>> result;

  text = trim_copy(text);
  if (text.empty())
  {
    for (std::size_t i = 0; i < n; i++)
    {
      result.push_back({i});
    }
  }
  else
  {
    for (const std::string& numbers: regex_split(text, "\\s*;\\s*"))
    {
      result.push_back(as_set(utilities::parse_natural_number_sequence(numbers)));
    }
  }

  return result;
}

} // namespace utilities

namespace lps {

constexpr std::uint32_t relprod_ignore = std::numeric_limits<std::uint32_t>::max();

struct symbolic_reachability_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool remove_unused_rewrite_rules = false;
  bool no_discard = false;
  bool no_discard_read = false;
  bool no_discard_write = false;
  bool no_relprod = false;
  bool test = false;
  std::string summand_groups;
};

inline
std::ostream& operator<<(std::ostream& out, const symbolic_reachability_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "one-point-rule-rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "no-discard = " << std::boolalpha << options.no_discard << std::endl;
  out << "no-read = " << std::boolalpha << options.no_discard_read << std::endl;
  out << "no-write = " << std::boolalpha << options.no_discard_write << std::endl;
  out << "no-relprod = " << std::boolalpha << options.no_relprod << std::endl;
  out << "test = " << std::boolalpha << options.test << std::endl;
  out << "groups = " << options.summand_groups << std::endl;
  return out;
}

inline
std::set<std::size_t> parameter_indices(const std::set<data::variable>& parameters, const std::map<data::variable, std::size_t>& index)
{
  std::set<std::size_t> result;
  for (const data::variable& p: parameters)
  {
    result.insert(index.at(p));
  }
  return result;
}

inline
std::string print_read_write_patterns(const std::vector<boost::dynamic_bitset<>>& patterns)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < patterns.size(); i++)
  {
    out << std::setw(4) << i << " " << patterns[i] << std::endl;
  }
  return out.str();
}

// Set some additional read and write bits to true in patterns. This is only used for testing purposes.
template <typename Options>
void adjust_read_write_patterns(std::vector<boost::dynamic_bitset<>>& patterns, const Options& options)
{
  for (boost::dynamic_bitset<>& pattern: patterns)
  {
    std::size_t n = pattern.size() / 2;
    for (std::size_t j = 0; j < n; j++)
    {
      bool read = pattern[2*j];
      bool write = pattern[2*j + 1];
      if (options.no_discard)
      {
        read = true;
        write = true;
      }
      else if (read || write)
      {
        if (options.no_discard_read)
        {
          read = true;
        }
        if (options.no_discard_write)
        {
          write = true;
        }
      }
      pattern[2*j] = read;
      pattern[2*j + 1] = write;
    }
  }
}

inline
std::vector<boost::dynamic_bitset<>> compute_summand_group_patterns(const std::vector<boost::dynamic_bitset<>>& patterns, const std::vector<std::set<std::size_t>> groups)
{
  std::vector<boost::dynamic_bitset<>> result;
  boost::dynamic_bitset<> empty(patterns.front().size());

  for (const std::set<std::size_t>& group: groups)
  {
    if (group.empty())
    {
      continue;
    }
    boost::dynamic_bitset<> pattern = empty;
    for (std::size_t i: group)
    {
      pattern |= patterns[i];
    }
    result.push_back(pattern);
  }
  return result;
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

struct summand_group
{
  struct summand
  {
    data::data_expression condition;
    data::variable_list variables; // the summand variables
    std::vector<data::data_expression> next_state; // the projected next state vector
    std::vector<int> copy; // copy node information that is needed by sylvan::ldds::relprod

    summand(const data::data_expression& condition_, const data::variable_list& variables_, const std::vector<data::data_expression>& next_state_, const std::vector<int>& copy_)
      : condition(condition_), variables(variables_), next_state(next_state_), copy(copy_)
    {}
  };

  std::vector<summand> summands; // the summands of the group
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

  summand_group(const data::variable_list& process_parameters, const boost::dynamic_bitset<>& read_write_pattern)
  {
    using utilities::as_vector;

    std::size_t n = process_parameters.size();

    std::vector<std::uint32_t> Ir_values;
    std::vector<std::uint32_t> Ip_values;

    for (std::size_t j = 0; j < n; j++)
    {
      bool is_read = read_write_pattern[2*j];
      bool is_write = read_write_pattern[2*j + 1];

      Ip_values.push_back(is_read ? 1 : 0);

      if (is_read)
      {
        read.push_back(j);
        Ir_values.push_back(is_write ? 1 : 3);
      }

      if (is_write)
      {
        write.push_back(j);
        Ir_values.push_back(is_read ? 2 : 4);
      }

      if (!is_read && !is_write)
      {
        Ir_values.push_back(0);
      }
    }

    read_parameters = project(as_vector(process_parameters), read);
    write_parameters = project(as_vector(process_parameters), write);
    L = sylvan::ldds::empty_set();
    Ldomain = sylvan::ldds::empty_set();
    Ir = sylvan::ldds::cube(Ir_values);
    Ip = sylvan::ldds::cube(Ip_values);
    compute_read_write_pos();
  }
};

inline
std::ostream& operator<<(std::ostream& out, const summand_group& x)
{
  using namespace sylvan::ldds;
  for (const auto& smd: x.summands)
  {
    out << "condition = " << smd.condition << std::endl;
    out << "variables = " << core::detail::print_list(smd.variables) << std::endl;
    out << "next state = " << core::detail::print_list(smd.next_state) << std::endl;
    out << "copy = " << core::detail::print_list(smd.copy) << std::endl;
    std::vector<std::string> assignments;
    auto vi = x.write_parameters.begin();
    auto ni = smd.next_state.begin();
    for (; vi != x.write_parameters.end(); ++vi, ++ni)
    {
      assignments.push_back(data::pp(*vi) + " := " + data::pp(*ni));
    }
    out << "(" << smd.condition << ") -> " << core::detail::print_list(assignments) << std::endl;
  }
  out << "read = " << core::detail::print_list(x.read) << std::endl;
  out << "read parameters = " << core::detail::print_list(x.read_parameters) << std::endl;
  out << "write = " << core::detail::print_list(x.write) << std::endl;
  out << "write parameters = " << core::detail::print_list(x.write_parameters) << std::endl;
  out << "L = " << print_ldd(x.L) << std::endl;
  out << "Ir = " << print_ldd(x.Ir) << std::endl;
  out << "Ip = " << print_ldd(x.Ip) << std::endl;
  return out;
}

// A very inefficient implementation of relprod, that matches the specification closely
sylvan::ldds::ldd alternative_relprod(const sylvan::ldds::ldd& todo, const summand_group& R)
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
      if (y_[j] != relprod_ignore)
      {
        x[R.write[j]] = y_[j];
      }
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

// A very inefficient implementation of relprev, that matches the specification closely
sylvan::ldds::ldd alternative_relprev(const sylvan::ldds::ldd& Y, const summand_group& R, const sylvan::ldds::ldd& X)
{
  using namespace sylvan::ldds;

  ldd result = empty_set();
  for (const std::vector<std::uint32_t>& x_values: ldd_solutions(X))
  {
    ldd x = cube(x_values);
    ldd y = alternative_relprod(x, R);
    if (intersect(y, Y) != empty_set())
    {
      result = union_(result, x);
    }
  }
  return result;
}

std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < x.size(); i++)
  {
    if (x[i] == relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[i].value(x[i]));
    }
  }
  return result;
}

std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x, const std::vector<std::size_t>& used)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < used.size(); i++)
  {
    if (x[i] == relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[used[i]].value(x[i]));
    }
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

// Add operations on reals that are needed for the exploration.
inline
std::set<data::function_symbol> add_real_operators(std::set<data::function_symbol> s)
{
  std::set<data::function_symbol> result = std::move(s);
  result.insert(data::less_equal(data::sort_real::real_()));
  result.insert(data::greater_equal(data::sort_real::real_()));
  result.insert(data::sort_real::plus(data::sort_real::real_(), data::sort_real::real_()));
  return result;
}

inline
data::rewriter construct_rewriter(const data::data_specification& dataspec, data::rewrite_strategy rewrite_strategy, const std::set<data::function_symbol>& function_symbols, bool remove_unused_rewrite_rules)
{
  if (remove_unused_rewrite_rules)
  {
    return data::rewriter(dataspec,
                          data::used_data_equation_selector(dataspec, add_real_operators(function_symbols), std::set<data::variable>()),
                          rewrite_strategy);
  }
  else
  {
    return data::rewriter(dataspec, rewrite_strategy);
  }
}

template <typename EnumeratorElement>
void check_enumerator_solution(const EnumeratorElement& p, const summand_group& summand)
{
  if (p.expression() != data::sort_bool::true_())
  {
    // TODO: print the problematic expression, like it is done in lps2lts
    throw data::enumerator_error("Expression does not rewrite to true or false: " + data::pp(p.expression()) + " " + atermpp::pp(atermpp::aterm(p.expression())));
  }
}

template <typename Context>
void learn_successors_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{
  using namespace sylvan::ldds;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

  auto p = reinterpret_cast<Context*>(context);
  auto& algorithm = p->first;
  auto& group = p->second;
  auto& sigma = algorithm.m_sigma;
  const auto& rewr = algorithm.m_rewr;
  auto& data_index = algorithm.m_data_index;
  const auto& enumerator = algorithm.m_enumerator;
  std::size_t x_size = group.read.size();
  std::size_t y_size = group.write.size();
  std::size_t xy_size = x_size + y_size;
  std::uint32_t xy[xy_size]; // TODO: avoid this C99 construction

  // add the assignments corresponding to x to sigma
  // add x to the transition xy
  for (std::size_t j = 0; j < x_size; j++)
  {
    sigma[group.read_parameters[j]] = data_index[group.read[j]].value(x[j]);
    xy[group.read_pos[j]] = x[j];
  }

  for (const auto& smd: group.summands)
  {
    data::data_expression condition = rewr(smd.condition, sigma);
    if (!data::is_false(condition))
    {
      enumerator.enumerate(enumerator_element(smd.variables, condition),
                           sigma,
                           [&](const enumerator_element& p) {
                             check_enumerator_solution(p, group);
                             p.add_assignments(smd.variables, sigma, rewr);
                             for (std::size_t j = 0; j < y_size; j++)
                             {
                               data::data_expression value = rewr(smd.next_state[j], sigma);
                               xy[group.write_pos[j]] = data::is_variable(value) ? lps::relprod_ignore : data_index[group.write[j]].index(value);
                             }
                             mCRL2log(log::debug) << "  " << print_transition(data_index, xy, group.read, group.write) << std::endl;
                             group.Ldomain = union_cube(group.Ldomain, x, x_size);
                             group.L = algorithm.m_options.no_relprod ? union_cube(group.L, xy, xy_size) : union_cube_copy(group.L, xy, smd.copy.data(), xy_size);
                             return false;
                           },
                           data::is_false
      );
    }
    data::remove_assignments(sigma, smd.variables);
  }
  data::remove_assignments(sigma, group.read_parameters);
}

template <typename SummandGroup>
void test_successor_predecessor(const std::vector<SummandGroup>& R, const sylvan::ldds::ldd& V, bool no_relprod = false)
{
  using namespace sylvan::ldds;

  std::random_device rd;
  std::mt19937 mt(rd());
  
  // returns a random value n: low <= n <= high
  auto random = [&](std::uint32_t low, std::uint32_t high)
  {
    std::uniform_int_distribution<std::uint32_t> dist(low, high);
    return dist(mt);
  };
  
  auto succ = [&](const ldd& U)
  {
    ldd result = empty_set();
    for (std::size_t i = 0; i < R.size(); i++)
    {
      result = no_relprod ? union_(result, alternative_relprod(U, R[i])) : relprod_union(U, R[i].L, R[i].Ir, result);
    }
    return result;
  };

  // Returns { u in U | exists v in V: u -> v }
  auto pred = [&](const ldd& U, const ldd& V)
  {
    ldd result = empty_set();
    for (std::size_t i = 0; i < R.size(); i++)
    {
      result = no_relprod ? union_(result, alternative_relprev(V, R[i], U)) : union_(result, relprev(V, R[i].L, R[i].Ir, U));
    }
    return result;
  };

  std::vector<std::vector<std::uint32_t>> V_elements = ldd_solutions(V);
  std::size_t n = random(1, V_elements.size() - 1);
  std::vector<std::vector<std::uint32_t>> U_elements;
  std::sample(V_elements.begin(), V_elements.end(), std::back_inserter(U_elements), n, mt);
  ldd U = make_ldd(U_elements); // U is a random subset of V
  if (pred(U, succ(U)) != U)
  {
    std::cout << "U = " << print_ldd(U) << std::endl;
    std::cout << "succ(U) = " << print_ldd(succ(U)) << std::endl;
    std::cout << "pred(U, succ(U)) = " << print_ldd(pred(U, succ(U))) << std::endl;
    throw mcrl2::runtime_error("test_successor_predecessor failed!");
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SYMBOLIC_REACHABILITY_H
