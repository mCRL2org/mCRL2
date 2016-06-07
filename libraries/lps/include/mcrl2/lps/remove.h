// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/remove.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REMOVE_H
#define MCRL2_LPS_REMOVE_H

#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Function object that checks if a summand has a false condition
struct is_trivial_summand
{
  bool operator()(const summand_base& s) const
  {
    return s.condition() == data::sort_bool::false_();
  }
};

/// \brief Function object that checks if a sort is a singleton sort.
/// Note that it is an approximation, meaning that in some cases it
/// may return false whereas in reality the answer is true.
struct is_singleton_sort
{
  const data::data_specification& m_data_spec;

  is_singleton_sort(const data::data_specification& data_spec)
    : m_data_spec(data_spec)
  {}

  bool operator()(const data::sort_expression& s) const
  {
    auto const& constructors = m_data_spec.constructors(s);
    if (constructors.size() != 1)
    {
      return false;
    }
    return !is_function_sort(constructors.front().sort());
  }
};

/// \brief Traverser for removing parameters from LPS data types.
/// These parameters can be either process parameters or free variables.
/// Assignments to these parameters are removed as well.
struct remove_parameters_builder: public data_expression_builder<remove_parameters_builder>
{
  typedef data_expression_builder<remove_parameters_builder> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const std::set<data::variable>& to_be_removed;

  remove_parameters_builder(const std::set<data::variable>& to_be_removed_)
    : to_be_removed(to_be_removed_)
  {}

  /// \brief Removes parameters from a set container.
  void update(std::set<data::variable>& x)
  {
    for (const data::variable& v: to_be_removed)
    {
      x.erase(v);
    }
  }

  /// \brief Removes parameters from a list of variables.
  data::variable_list apply(const data::variable_list& x)
  {
  	using utilities::detail::contains;

    std::vector<data::variable> result;
    for (const data::variable& v: x)
    {
      if (!contains(to_be_removed, v))
      {
        result.push_back(v);
      }
    }
    return data::variable_list(result.begin(), result.end());
  }

  /// \brief Removes parameters from a list of assignments.
  /// Assignments to removed parameters are removed.
  data::assignment_list apply(const data::assignment_list& x)
  {
    // TODO: make this implementation more efficient
    std::vector<data::assignment> a(x.begin(), x.end());
    a.erase(std::remove_if(a.begin(), a.end(), data::detail::has_left_hand_side_in(to_be_removed)), a.end());
    return data::assignment_list(a.begin(), a.end());
  }

  /// \brief Removes parameters from a linear_process
  /// \param s A linear_process
  void update(linear_process& x)
  {
    super::update(x);
    x.process_parameters() = apply(x.process_parameters());
  }

  /// \brief Removes parameters from a linear_process
  /// \param s A linear_process
  void update(stochastic_linear_process& x)
  {
    super::update(x);
    x.process_parameters() = apply(x.process_parameters());
  }

  /// \brief Removes parameters from a linear process specification
  /// \param spec A linear process specification
  void update(specification& x)
  {
    super::update(x);
    update(x.global_variables());
  }

  /// \brief Removes parameters from a linear process specification
  /// \param spec A linear process specification
  void update(stochastic_specification& x)
  {
    super::update(x);
    update(x.global_variables());
  }
};

} // namespace detail

/// \brief Rewrites an LPS data type.
template <typename Object>
void remove_parameters(Object& x, const std::set<data::variable>& to_be_removed)
{
  detail::remove_parameters_builder f(to_be_removed);
  f.update(x);
}

/// \brief Removes summands with condition equal to false from a linear process specification
/// \param spec A linear process specification
template <typename Specification>
void remove_trivial_summands(Specification& spec)
{
  auto& v = spec.process().action_summands();
  v.erase(std::remove_if(v.begin(), v.end(), lps::detail::is_trivial_summand()), v.end());

  deadlock_summand_vector& w = spec.process().deadlock_summands();
  w.erase(std::remove_if(w.begin(), w.end(), lps::detail::is_trivial_summand()), w.end());
}

/// \brief Removes parameters with a singleton sort from a linear process specification
/// \param spec A linear process specification
template <typename Specification>
void remove_singleton_sorts(Specification& spec)
{
  data::mutable_map_substitution<> sigma;
  std::set<data::variable> to_be_removed;
  for (const data::variable& v: spec.process().process_parameters())
  {
    if (lps::detail::is_singleton_sort(spec.data())(v.sort()))
    {
      sigma[v] = *spec.data().constructors(v.sort()).begin();
      to_be_removed.insert(v);
    }
  }
  lps::replace_variables(spec, sigma);
  lps::remove_parameters(spec, to_be_removed);
}

/// \brief Removes assignments of the form x := x from v for variables x that are not contained in do_not_remove.
inline
data::assignment_list remove_redundant_assignments(const data::assignment_list& assignments, const data::variable_list& do_not_remove)
{
  using utilities::detail::contains;

  std::vector<data::assignment> result;
  for (const data::assignment& a: assignments)
  {
    if (a.lhs() != a.rhs() || contains(do_not_remove, a.lhs()))
    {
      result.push_back(a);
    }
  }
  return data::assignment_list(result.begin(), result.end());
}

/// \brief Removes redundant assignments of the form x = x from an LPS specification
/// \param spec A linear process specification
template <typename Specification>
void remove_redundant_assignments(Specification& lpsspec)
{
  auto& summands = lpsspec.process().action_summands();
  for (auto i = summands.begin(); i != summands.end(); ++i)
  {
    i->assignments() = remove_redundant_assignments(i->assignments(), i->summation_variables());
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REMOVE_H
