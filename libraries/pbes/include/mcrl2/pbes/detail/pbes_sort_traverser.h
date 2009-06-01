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

#include "mcrl2/data/traverse.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template <typename OutIter>
  struct pbes_sort_traverser: pbes_expression_visitor<pbes_expression>
  {
    OutIter dest;

    typedef pbes_expression_visitor<pbes_expression> super;
    
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
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses a variable
    /// \param d A variable
    void traverse(const data::variable& d)    
    {                                         
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses a data expression
    /// \param d A data expression
    void traverse(const data::data_expression& d)    
    {                                         
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses an assignment
    /// \param a An assignment
    void traverse(const data::assignment& a)
    {
      dest = data::traverse_sort_expressions(a, dest);
    } 

    /// \brief Traverses a pbes expression
    /// \param d A pbes expression
    void traverse(const pbes_expression& p)    
    {
      super::visit(p);
    } 
  
    void traverse(const propositional_variable& v)
    {
      traverse_container(v.parameters());
    }
    
    void traverse(const propositional_variable_instantiation& v)
    {
      traverse_container(v.parameters());
    }
    
    void traverse(const pbes_equation& eq)
    {
      traverse(eq.variable());
      traverse(eq.formula());
    }

    template <typename Container>
    void traverse(const pbes<Container>& p)
    {
      traverse_container(p.free_variables());
      traverse_container(p.equations());
      traverse(p.initial_state());
    }

    template <typename Term>
    void operator()(const Term& t)
    {
      traverse(t);
    }

    //---------------------------------------------------------------//
    //                visit functions
    //---------------------------------------------------------------//

    /// \brief Visit data_expression node
    /// \param e A term
    /// \param d A data term
    /// \return The result of visiting the node
    bool visit_data_expression(const pbes_expression& e, const data::data_expression& d)
    {
      traverse(d);
      return true;
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& v)
    {
      traverse(v);
      return continue_recursion;
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
