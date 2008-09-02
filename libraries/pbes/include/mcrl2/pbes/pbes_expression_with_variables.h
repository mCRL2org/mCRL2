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
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

  /// Returns the union of v and w.
  inline
  data::data_variable_list data_variable_list_union(data::data_variable_list v, data::data_variable_list w)
  {
    if (v.empty())
    {
      return w;
    }
    if (w.empty())
    {
      return v;
    }
    std::set<data::data_variable> result;
    result.insert(v.begin(), v.end());
    result.insert(w.begin(), w.end());
    return data::data_variable_list(result.begin(), result.end());
  }

  /// Returns v minus w.
  inline
  data::data_variable_list data_variable_list_difference(data::data_variable_list v, data::data_variable_list w)
  {
    if (w.empty())
    {
      return v;
    }
    if (v.empty())
    {
      return v;
    }
    std::set<data::data_variable> result;
    result.insert(v.begin(), v.end());
    for (data::data_variable_list::iterator i = w.begin(); i != w.end(); ++i)
    {
      result.erase(*i);
    }
    return data::data_variable_list(result.begin(), result.end());
  }

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
        pbes_system::detail::free_variable_visitor visitor;
        visitor.visit(term);
        m_variables = data::data_variable_list(visitor.result.begin(), visitor.result.end());
      }
  
      /// Constructor.
      ///             
      pbes_expression_with_variables(ATermAppl term)
        : pbes_expression(term)
      {
        pbes_system::detail::free_variable_visitor visitor;
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

  template <>
  struct term_traits<pbes_expression_with_variables>
  {
    static inline
    bool is_true(pbes_expression_with_variables t) { return pbes_expr::is_true(t); }
    
    static inline 
    bool is_false(pbes_expression_with_variables t) { return pbes_expr::is_false(t); }
    
    static inline 
    bool is_not(pbes_expression_with_variables t) { return pbes_expr::is_not(t); }
    
    static inline 
    bool is_and(pbes_expression_with_variables t) { return pbes_expr::is_and(t); }
    
    static inline 
    bool is_or(pbes_expression_with_variables t) { return pbes_expr::is_or(t); }
    
    static inline 
    bool is_imp(pbes_expression_with_variables t) { return pbes_expr::is_imp(t); }
    
    static inline 
    bool is_forall(pbes_expression_with_variables t) { return pbes_expr::is_forall(t); }
    
    static inline 
    bool is_exists(pbes_expression_with_variables t) { return pbes_expr::is_exists(t); }
    
    static inline 
    bool is_data(pbes_expression_with_variables t) { return pbes_expr::is_data(t); }
    
    static inline 
    bool is_prop_var(pbes_expression_with_variables t) { return pbes_expr::is_propositional_variable_instantiation(t); }

    static inline
    pbes_expression_with_variables true_() { return pbes_expression_with_variables(pbes_expr_optimized::true_(), data::data_variable_list()); }
    
    static inline
    pbes_expression_with_variables false_() { return pbes_expression_with_variables(pbes_expr_optimized::false_(), data::data_variable_list()); }
    
    static inline
    pbes_expression_with_variables not_(pbes_expression_with_variables p) { return pbes_expression_with_variables(pbes_expr_optimized::not_(p), p.variables()); }
    
    static inline
    pbes_expression_with_variables and_(pbes_expression_with_variables p, pbes_expression_with_variables q)
    {
      if(is_true(p))
        return q;
      else if(is_false(p))
        return false_();
      else if(is_true(q))
        return p;
      else if(is_false(q))
        return false_();
      else if (p == q)
        return p;
      else
        return pbes_expression_with_variables(pbes_expr::and_(p, q), data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    pbes_expression_with_variables or_(pbes_expression_with_variables p, pbes_expression_with_variables q)
    {
      if(is_true(p))
        return true_();
      else if(is_false(p))
        return q;
      else if(is_true(q))
        return true_();
      else if(is_false(q))
        return p;
      else if (p == q)
        return p;
      else
        return pbes_expression_with_variables(pbes_expr::or_(p, q), data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    pbes_expression_with_variables imp(pbes_expression_with_variables p, pbes_expression_with_variables q)
    {
      if(is_true(p))
        return q;
      else if(is_false(p))
        return true_();
      else if(is_true(q))
        return true_();
      else if(is_false(q))
        return not_(p);
      else if (p == q)
        return true_();
      else
        return pbes_expression_with_variables(pbes_expr::imp(p, q), data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    pbes_expression_with_variables forall(data::data_variable_list l, pbes_expression_with_variables p)
    {
      return pbes_expression_with_variables(pbes_expr_optimized::forall(l, p), data_variable_list_difference(p.variables(), l));
    }
    
    static inline
    pbes_expression_with_variables exists(data::data_variable_list l, pbes_expression_with_variables p)
    {
      return pbes_expression_with_variables(pbes_expr_optimized::exists(l, p), data_variable_list_difference(p.variables(), l));
    }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
