// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_free_variable_finder.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_FREE_VARIABLE_FINDER_H
#define MCRL2_LPS_DETAIL_LPS_FREE_VARIABLE_FINDER_H

#include <set>
#include "mcrl2/data/find.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for traversing the variable expressions of  LPS data types.
  template <typename OutIter>
  struct lps_free_variable_finder
  {
    OutIter dest;
    std::multiset<data::variable> bound_variables;

    bool is_free(const data::variable& v) const
    {
      return bound_variables.find(v) == bound_variables.end();
    }

    template <typename Container>
    void add_bound_variables(const Container& c)
    {
      bound_variables.insert(c.begin(), c.end());
    }

    template <typename Container>
    void remove_bound_variables(const Container& c)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        bound_variables.erase(bound_variables.find(*i));
      }
    }

    lps_free_variable_finder(OutIter dest_)
      : dest(dest_)
    {}

    /// \brief Traverses the elements of a container
    template <typename Container>
    void traverse_container(const Container& c)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        traverse(*i);
      }
    }

    /// \brief Traverses a variable
    /// \param d A variable
    void traverse(const data::variable& d)
    {
      if (is_free(d))
      {
        *dest++ = d;
      }
    }

    /// \brief Traverses a data expression
    /// \param d A data expression
    void traverse(const data::data_expression& d)
    {
      // TODO: replace this with a direct call to a suitable find function
      std::set<data::variable> v = data::find_free_variables(d);
      for (std::set<data::variable>::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (is_free(*i))
        {
          *dest++ = *i;
        }
      }
    }

    /// \brief Traverses an assignment
    /// \param a An assignment
    void traverse(const data::assignment& a)
    {
      traverse(a.rhs());
    }

    /// \brief Traverses an action
    /// \param a An action
    void traverse(const action& a)
    {
      traverse_container(a.arguments());
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void traverse(const deadlock& d)
    {
      if (d.has_time())
      {
        traverse(d.time());
      }
    }

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void traverse(const multi_action& a)
    {
      if (a.has_time())
      {
        traverse(a.time());
      }
      traverse_container(a.actions());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void traverse(const action_summand& s)
    {
      add_bound_variables(s.summation_variables());
      traverse(s.condition());
      traverse(s.multi_action());
      traverse_container(s.assignments());
      remove_bound_variables(s.summation_variables());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void traverse(const deadlock_summand& s)
    {
      add_bound_variables(s.summation_variables());
      traverse(s.condition());
      traverse(s.deadlock());
      remove_bound_variables(s.summation_variables());
    }

    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void traverse(const process_initializer& i)
    {
      traverse_container(i.assignments());
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void traverse(const linear_process& p)
    {
      add_bound_variables(p.process_parameters());
      traverse_container(p.action_summands());
      traverse_container(p.deadlock_summands());
      remove_bound_variables(p.process_parameters());
    }

    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void traverse(const specification& spec)
    {
      traverse(spec.process());
      traverse(spec.initial_process());
    }

    template <typename Term>
    void operator()(const Term& t)
    {
      traverse(t);
    }
  };

  /// \brief Utility function to create an lps_free_variable_finder.
  template <typename OutIter>
  lps_free_variable_finder<OutIter> make_lps_free_variable_finder(OutIter dest)
  {
    return lps_free_variable_finder<OutIter>(dest);
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_FREE_VARIABLE_FINDER_H
