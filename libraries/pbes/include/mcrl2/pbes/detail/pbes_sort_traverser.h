// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_SORT_TRAVERSER_H
#define MCRL2_PBES_DETAIL_PBES_SORT_TRAVERSER_H

#include <vector>
#include "mcrl2/data/find.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_sort_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template <typename OutIter>
  struct pbes_sort_traverser
  {
    OutIter dest;
  
    pbes_sort_traverser(OutIter dest_)
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

    /// \brief Traverses a sort expression
    /// \param d A sort expression
    void traverse(const data::sort_expression& d)    
    {                                         
      data::detail::make_find_helper< data::sort_expression, data::detail::sort_traverser >(dest)(d);
    } 
  
    /// \brief Traverses a variable
    /// \param d A variable
    void traverse(const data::variable& d)    
    {                                         
      data::detail::make_find_helper< data::sort_expression, data::detail::sort_traverser >(dest)(d);
    } 
  
    /// \brief Traverses a data expression
    /// \param d A data expression
    void traverse(const data::data_expression& d)    
    {                                         
      data::detail::make_find_helper< data::sort_expression, data::detail::sort_traverser >(dest)(d);
    } 
  
    /// \brief Traverses an assignment
    /// \param a An assignment
    void traverse(const data::assignment& a)
    {
      data::detail::make_find_helper< data::sort_expression, data::detail::sort_traverser >(dest)(a);
    } 

    /// \brief Traverses a pbes expression
    /// \param t A pbes expression
    void traverse(const pbes_expression& t)
    {
      detail::pbes_sort_expression_visitor<pbes_expression> visitor;
      visitor.visit(t);
      for (std::set<data::sort_expression>::iterator i = visitor.result.begin(); i != visitor.result.end(); ++i)
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
    /// \param d A deadlock
    void traverse(const pbes_equation& e)
    {
      traverse(e.variable());
      traverse(e.formula());
    } 

    /// \brief Traverses a PBES
    /// \param spec A PBES specification
    template <typename Container>
    void traverse(const pbes<Container>& p)
    {
      traverse_container(p.global_variables());
      traverse_container(p.equations());
      traverse(p.initial_state());
    }

    template <typename Term>
    void operator()(const Term& t)
    {
      traverse(t);
    }
  };

  /// \brief Utility function to create an pbes_sort_traverser.
  template <typename OutIter>    
  pbes_sort_traverser<OutIter> make_pbes_sort_traverser(OutIter dest)        
  {
    return pbes_sort_traverser<OutIter>(dest);
  } 

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_SORT_TRAVERSER_H
