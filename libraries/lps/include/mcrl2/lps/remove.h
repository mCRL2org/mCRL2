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

/// \brief Function object for removing parameters from LPS data types.
/// These parameters can be either process parameters or free variables.
/// Assignments to these parameters are removed as well.
template <typename SetContainer>
struct lps_parameter_remover
{
  const SetContainer& to_be_removed;

  lps_parameter_remover(const SetContainer& to_be_removed_)
    : to_be_removed(to_be_removed_)
  {}

  /// \brief Removes parameters from a set container.
  template <typename SetContainer1>
  void remove_set_container(SetContainer1& c) const
  {
    for (auto i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
    {
      c.erase(*i);
    }
  }

  /// \brief Removes parameters from a list of variables.
  data::variable_list remove_list_copy(const data::variable_list& l) const
  {
  	using utilities::detail::contains;

    std::vector<data::variable> result;
    for (auto i = l.begin(); i != l.end(); ++i)
    {
      if (!contains(to_be_removed, *i))
      {
        result.push_back(*i);
      }
    }
    return data::variable_list(result.begin(), result.end());
  }

  /// \brief Removes parameters from a list of assignments.
  /// Assignments to removed parameters are removed.
  data::assignment_list remove_list_copy(const data::assignment_list& l) const
  {
    // TODO: make this implementation more efficient
    std::vector<data::assignment> a(l.begin(), l.end());
    a.erase(std::remove_if(a.begin(), a.end(), data::detail::has_left_hand_side_in(to_be_removed)), a.end());
    return data::assignment_list(a.begin(), a.end());
  }

  /// \brief Removes parameters from the elements of a term list
  template <typename TermList>
  void remove_list(TermList& l) const
  {
    l = remove_list_copy(l);
  }

  /// \brief Removes parameters from the elements of a container
  template <typename Container>
  void remove_container(Container& c) const
  {
    for (auto i = c.begin(); i != c.end(); ++i)
    {
      remove(*i);
    }
  }

  /// \brief Removes parameters from a summand
  /// \param s A summand
  void remove(action_summand& s) const
  {
    remove_list(s.assignments());
  }

  /// \brief Removes parameters from a process_initializer
  /// \param s A process_initializer
  void remove(process_initializer& i) const
  {
    i = process_initializer(remove_list_copy(i.assignments()));
  }

  /// \brief Removes parameters from a linear_process
  /// \param s A linear_process
  void remove(linear_process& p) const
  {
    remove_list(p.process_parameters());
    remove_container(p.action_summands());
  }

  /// \brief Removes parameters from a linear process specification
  /// \param spec A linear process specification
  void remove(specification& spec) const
  {
    remove(spec.process());
    remove(spec.initial_process());
    remove_set_container(spec.global_variables());
  }

  template <typename Term>
  void operator()(Term& t)
  {
    remove(t);
  }
};

/// \brief Utility function to create an lps_parameter_remover.
template <typename SetContainer>
lps_parameter_remover<SetContainer> make_lps_parameter_remover(const SetContainer& to_be_removed)
{
  return lps_parameter_remover<SetContainer>(to_be_removed);
}

} // namespace detail

/// \brief Rewrites an LPS data type.
template <typename Object, typename SetContainer>
void remove_parameters(Object& o, const SetContainer& to_be_removed)
{
  lps::detail::lps_parameter_remover<SetContainer> r(to_be_removed);
  r(o);
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
inline
void remove_singleton_sorts(specification& spec)
{
  data::mutable_map_substitution<> sigma;
  std::set<data::variable> to_be_removed;
  const data::variable_list& p = spec.process().process_parameters();
  for (auto i = p.begin(); i != p.end(); ++i)
  {
    if (lps::detail::is_singleton_sort(spec.data())(i->sort()))
    {
      sigma[*i] = *spec.data().constructors(i->sort()).begin();
      to_be_removed.insert(*i);
    }
  }
  lps::replace_variables(spec, sigma);
  lps::remove_parameters(spec, to_be_removed);
}

/// \brief Removes assignments of the form x := x from v for variables x that are not contained in do_not_remove.
inline
data::assignment_list remove_redundant_assignments(const data::assignment_list& v, const data::variable_list& do_not_remove)
{
  using utilities::detail::contains;

  std::vector<data::assignment> result;
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    if (i->lhs() != i->rhs() || contains(do_not_remove, i->lhs()))
    {
      result.push_back(*i);
    }
  }
  return data::assignment_list(result.begin(), result.end());
}

/// \brief Removes redundant assignments of the form x = x from an LPS specification
/// \param spec A linear process specification
inline
void remove_redundant_assignments(specification& lpsspec)
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
