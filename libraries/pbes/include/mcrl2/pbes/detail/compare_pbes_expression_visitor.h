// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/compare_pbes_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_COMPARE_PBES_EXPRESSION_VISITOR_H
#define MCRL2_PBES_DETAIL_COMPARE_PBES_EXPRESSION_VISITOR_H

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <typename Term>
  struct compare_pbes_expression_visitor: public global_variable_visitor<Term>
  {
    typedef global_variable_visitor<Term> super;
    typedef typename super::term_type term_type;
    typedef typename pbes_expression_visitor<term_type>::variable_sequence_type variable_sequence_type;
    typedef typename pbes_expression_visitor<term_type>::propositional_variable_type propositional_variable_type;
                        
    bool has_quantifiers;      
    bool has_predicate_variables;
    
    compare_pbes_expression_visitor()
      : has_quantifiers(false),
        has_predicate_variables(false)
    {}
                                   
    /// \brief Visit forall node
    /// \param e A term
    /// \param v A sequence of data variables
    /// \return The result of visiting the node
    bool visit_forall(const term_type& e, const variable_sequence_type& v, const term_type& x)
    {
      has_quantifiers = true;
      return super::visit_forall(e, v, x);
    }               

    /// \brief Visit exists node
    /// \param e A term
    /// \param v A sequence of data variables
    /// \return The result of visiting the node
    bool visit_exists(const term_type& e, const variable_sequence_type& v, const term_type& x)
    {
      has_quantifiers = true;
      return super::visit_exists(e, v, x); 
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \param v A propositional variable instantiation
    /// \return The result of visiting the node
    bool visit_propositional_variable(const term_type& e, const propositional_variable_type& v)
    {
      has_predicate_variables = true;
      return super::visit_propositional_variable(e, v);
    }
    
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_COMPARE_PBES_EXPRESSION_VISITOR_H
