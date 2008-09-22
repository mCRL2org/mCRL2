// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_with_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
#define MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H

#include <set>
#include "mcrl2/core/detail/join.h"
#include "mcrl2/data/term_traits.h"
#include "mcrl2/data/data_expression_with_variables.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

  /// A pbes expression that stores a list of variables. The
  /// intended use case is to store the free variables of the
  /// expression.
  class pbes_expression_with_variables: public pbes_expression
  {
    protected:
      data::data_variable_list m_variables;
      
    public:
      /// Constructor.
      ///             
      pbes_expression_with_variables()
      {}
  
      /// Constructor.
      ///             
      pbes_expression_with_variables(atermpp::aterm_appl term)
        : pbes_expression(term)
      {
        pbes_system::detail::free_variable_visitor<pbes_expression> visitor;
        visitor.visit(term);
        m_variables = data::data_variable_list(visitor.result.begin(), visitor.result.end());
      }
  
      /// Constructor.
      ///             
      pbes_expression_with_variables(ATermAppl term)
        : pbes_expression(term)
      {
        pbes_system::detail::free_variable_visitor<pbes_expression> visitor;
        visitor.visit(pbes_expression(term));
        m_variables = data::data_variable_list(visitor.result.begin(), visitor.result.end());
      }

      /// Constructor.
      ///             
      pbes_expression_with_variables(pbes_expression expression, data::data_variable_list variables)
        : pbes_expression(expression), m_variables(variables)
      {}
      
      /// Return the variables.
      ///
      data::data_variable_list variables() const
      {
        return m_variables;
      }
      
      /// Return the variables.
      ///
      data::data_variable_list& variables()
      {
        return m_variables;
      }
      
      /// Returns true if the expression has no free variables.
      bool is_closed() const
      {
        return m_variables.empty();
      }
  }; 

} // namespace pbes_system

namespace core {

  template <>
  struct term_traits<pbes_system::pbes_expression_with_variables>
  {
    typedef pbes_system::pbes_expression_with_variables term_type;
    typedef data::data_variable variable_type;
    typedef data::data_expression_with_variables data_term_type;
    typedef data::data_expression_list data_term_sequence_type;
    typedef data::data_variable data_variable_type;
    typedef data::data_variable_list variable_sequence_type;
    typedef pbes_system::propositional_variable propositional_variable_decl_type;   
    typedef pbes_system::propositional_variable_instantiation propositional_variable_type;
    typedef core::identifier_string string_type;

    static inline
    bool is_constant(const term_type& t) { return t.variables().empty(); }

    static inline
    bool is_true(term_type t) { return pbes_system::pbes_expr::is_true(t); }
    
    static inline 
    bool is_false(term_type t) { return pbes_system::pbes_expr::is_false(t); }
    
    static inline 
    bool is_not(term_type t) { return pbes_system::pbes_expr::is_not(t); }
    
    static inline 
    bool is_and(term_type t) { return pbes_system::pbes_expr::is_and(t); }
    
    static inline 
    bool is_or(term_type t) { return pbes_system::pbes_expr::is_or(t); }
    
    static inline 
    bool is_imp(term_type t) { return pbes_system::pbes_expr::is_imp(t); }
    
    static inline 
    bool is_forall(term_type t) { return pbes_system::pbes_expr::is_forall(t); }
    
    static inline 
    bool is_exists(term_type t) { return pbes_system::pbes_expr::is_exists(t); }
    
    static inline 
    bool is_data(term_type t) { return pbes_system::pbes_expr::is_data(t); }
    
    static inline 
    bool is_prop_var(term_type t) { return pbes_system::pbes_expr::is_propositional_variable_instantiation(t); }

    static inline
    term_type true_() { return term_type(pbes_system::pbes_expr_optimized::true_(), variable_sequence_type()); }
    
    static inline
    term_type false_() { return term_type(pbes_system::pbes_expr_optimized::false_(), variable_sequence_type()); }
    
    static inline
    term_type not_(term_type p) { return term_type(pbes_system::pbes_expr_optimized::not_(p), p.variables()); }
    
    static inline
    term_type and_(term_type p, term_type q)
    {
      return term_type(pbes_system::pbes_expr::and_(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type or_(term_type p, term_type q)
    {
      return term_type(pbes_system::pbes_expr::or_(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type imp(term_type p, term_type q)
    {
      return term_type(pbes_system::pbes_expr::imp(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type forall(variable_sequence_type l, term_type p)
    {
      return term_type(pbes_system::pbes_expr::forall(l, p), data::data_variable_list_difference(p.variables(), l));
    }
    
    static inline
    term_type exists(variable_sequence_type l, term_type p)
    {
      return term_type(pbes_system::pbes_expr::exists(l, p), data::data_variable_list_difference(p.variables(), l));
    }

    static inline
    propositional_variable_type prop_var(const string_type& name, const data_term_sequence_type& parameters)
    {
      return propositional_variable_type(name, parameters);
    }

    static inline
    data_term_type val(term_type t) { return pbes_system::accessors::val(t); }  

    static inline
    term_type arg(term_type t) { return pbes_system::accessors::arg(t); }
    
    static inline
    term_type left(term_type t) { return pbes_system::accessors::left(t); }
    
    static inline
    term_type right(term_type t) { return pbes_system::accessors::right(t); }
    
    static inline
    variable_sequence_type var(term_type t) { return pbes_system::accessors::var(t); }
    
    static inline
    string_type name(term_type t) { return pbes_system::accessors::name(t); }
    
    static inline
    data_term_sequence_type param(term_type t) { return pbes_system::accessors::param(t); }

    static inline
    propositional_variable_type prop_var(term_type t) { return propositional_variable_type(t); }

    static inline
    term_type variable2term(variable_type v)
    {
      return term_type(v, atermpp::make_list(v));
    }
  };

} // namespace core

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::pbes_system::pbes_expression_with_variables)
/// \endcond

#endif // MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
