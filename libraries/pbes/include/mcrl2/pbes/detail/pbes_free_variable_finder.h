// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_free_variable_finder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_FREE_VARIABLE_FINDER_H
#define MCRL2_PBES_DETAIL_PBES_FREE_VARIABLE_FINDER_H

#include <set>
#include "mcrl2/data/find.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Function object for traversing the variable expressions of PBES data types.
  template <typename OutIter>
  struct pbes_free_variable_finder
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

    pbes_free_variable_finder(OutIter dest_)
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

    /// \brief Traverses a pbes expression
    /// \param t A pbes expression
    void traverse(const pbes_expression& t)
    {
      detail::free_variable_visitor<pbes_expression> visitor;
      visitor.visit(t);
      for (std::set<data::variable>::iterator i = visitor.result.begin(); i != visitor.result.end(); ++i)
      {
        traverse(*i);
      }
    } 

    /// \brief Traverses a propositional variable declaration
    /// \param t A propositional variable declaration
    void traverse(const propositional_variable& v)
    {
      traverse_container(v.parameters());     
    } 

    /// \brief Traverses a propositional variable instantiation
    /// \param t A propositional variable instantiation
    void traverse(const propositional_variable_instantiation& v)
    {
      traverse_container(v.parameters());
    } 

    /// \brief Traverses a pbes equation
    /// \param e A pbes equation
    void traverse(const pbes_equation& e)
    {
      add_bound_variables(e.variable().parameters());
      traverse(e.formula());
      remove_bound_variables(e.variable().parameters());
    } 

    /// \brief Traverses a PBES
    /// \param spec A PBES specification
    template <typename Container>
    void traverse(const pbes<Container>& p)
    {
      add_bound_variables(p.global_variables());
      traverse_container(p.equations());
      traverse(p.initial_state());
      remove_bound_variables(p.global_variables());
    }

    template <typename Term>
    void operator()(const Term& t)
    {
      traverse(t);
    }
  };

  /// \brief Utility function to create an pbes_free_variable_finder.
  template <typename OutIter>
  pbes_free_variable_finder<OutIter> make_pbes_free_variable_finder(OutIter dest)
  {
    return pbes_free_variable_finder<OutIter>(dest);
  }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_FREE_VARIABLE_FINDER_H
