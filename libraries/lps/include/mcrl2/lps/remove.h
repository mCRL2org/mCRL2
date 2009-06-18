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

#include "mcrl2/lps/detail/lps_parameter_remover.h"
#include "mcrl2/lps/substitute.h"

#ifndef MCRL2_LPS_REMOVE_H
#define MCRL2_LPS_REMOVE_H

namespace mcrl2 {

namespace lps {

namespace detail {

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
      data::data_specification::constructors_const_range c = m_data_spec.constructors(s);
      if (boost::distance(c) != 1)
      {
        return false;
      }
      data::function_symbol f = *c.begin();
      return !f.sort().is_function_sort();
    }
  };

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
  inline
  void remove_trivial_summands(specification& spec)
  {
    action_summand_vector& v = spec.process().action_summands();
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
    for (data::variable_list::const_iterator i = p.begin(); i != p.end(); ++i)
    {
      if (lps::detail::is_singleton_sort(spec.data())(i->sort()))
      {
        sigma[*i] = *spec.data().constructors(i->sort()).begin();
        to_be_removed.insert(*i);
      }
    }
    lps::substitute(spec, sigma);
    lps::remove_parameters(spec, to_be_removed);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REMOVE_H
