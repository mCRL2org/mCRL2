// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/term_traits.h
/// \brief Contains term traits for data_expression and data_expression_with_variables.

#ifndef MCRL2_DATA_TERM_TRAITS_H
#define MCRL2_DATA_TERM_TRAITS_H

#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_expression_with_variables.h"
#include "mcrl2/data/data_variable.h"

namespace mcrl2 {

namespace core {

  template <>
  struct term_traits<data::data_expression>
  {
    typedef data::data_expression term_type;
    typedef data::data_variable variable_type;
    typedef data::data_variable_list data_variable_sequence_type;
    
    static inline
    term_type true_() { return data::data_expr::true_(); }
    
    static inline
    term_type false_() { return data::data_expr::false_(); }
    
    static inline
    term_type not_(term_type p) { return data::data_expr::not_(p); }
    
    static inline
    term_type and_(term_type p, term_type q) { return data::data_expr::and_(p, q); }
    
    static inline
    term_type or_(term_type p, term_type q) { return data::data_expr::or_(p, q); }
    
    // static inline
    // term_type imp(term_type p, term_type q) { return data::data_expr::imp(p, q); }
    
    // static inline
    // term_type forall(data_variable_sequence_type l, term_type p) { return data::data_expr::forall(l, p); }
    
    // static inline
    // term_type exists(data_variable_sequence_type l, term_type p) { return data::data_expr::exists(l, p); }

    static inline
    bool is_true(term_type t) { return data::data_expr::is_true(t); }
    
    static inline 
    bool is_false(term_type t) { return data::data_expr::is_false(t); }
    
    static inline 
    bool is_not(term_type t) { return data::data_expr::is_not(t); }
    
    static inline 
    bool is_and(term_type t) { return data::data_expr::is_and(t); }
    
    static inline 
    bool is_or(term_type t) { return data::data_expr::is_or(t); }
    
    static inline 
    bool is_imp(term_type t) { return data::data_expr::is_imp(t); }
    
    static inline 
    bool is_forall(term_type t) { return data::data_expr::is_forall(t); }
    
    static inline 
    bool is_exists(term_type t) { return data::data_expr::is_exists(t); }

    static inline
    term_type variable2term(variable_type v)
    {
      return v;
    }

/*    
    template <typename FwdIt>
    static
    term_type join_and(FwdIt first, FwdIt last)
    {
      return data::data_expr::join_and(first, last);
    }

    template <typename FwdIt>
    static
    term_type join_or(FwdIt first, FwdIt last)
    {
      return data::data_expr::join_or(first, last);
    }
*/    
  };

  template <>
  struct term_traits<data::data_expression_with_variables>
  {
    typedef data::data_expression_with_variables term_type;
    typedef data::data_variable variable_type;
    typedef data::data_variable_list data_variable_sequence_type;
    
    static inline
    term_type true_() { return data::data_expr::true_(); }
    
    static inline
    term_type false_() { return data::data_expr::false_(); }

    static inline
    term_type not_(term_type p) { return term_type(data::data_expr::not_(p), p.variables()); }
    
    static inline
    term_type and_(term_type p, term_type q)
    {
      return term_type(data::data_expr::and_(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    }
    
    static inline
    term_type or_(term_type p, term_type q)
    {
      return term_type(data::data_expr::or_(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    }
    
    // static inline
    // term_type imp(term_type p, term_type q)
    // {
    //   return term_type(data::data_expr::imp(p, q), data::data_variable_list_union(p.variables(), q.variables()));
    // }
    // 
    // static inline
    // term_type forall(data_variable_sequence_type l, term_type p)
    // {
    //   return term_type(data::data_expr::forall(l, p), data::data_variable_list_difference(p.variables(), l));
    // }
    // 
    // static inline
    // term_type exists(data_variable_sequence_type l, term_type p)
    // {
    //   return term_type(data::data_expr::exists(l, p), data::data_variable_list_difference(p.variables(), l));
    // }
    
    static inline
    bool is_true(term_type t) { return data::data_expr::is_true(t); }
    
    static inline 
    bool is_false(term_type t) { return data::data_expr::is_false(t); }
    
    static inline 
    bool is_not(term_type t) { return data::data_expr::is_not(t); }
    
    static inline 
    bool is_and(term_type t) { return data::data_expr::is_and(t); }
    
    static inline 
    bool is_or(term_type t) { return data::data_expr::is_or(t); }
    
    static inline 
    bool is_imp(term_type t) { return data::data_expr::is_imp(t); }
    
    static inline 
    bool is_forall(term_type t) { return data::data_expr::is_forall(t); }
    
    static inline 
    bool is_exists(term_type t) { return data::data_expr::is_exists(t); }

    static inline
    term_type variable2term(variable_type v)
    {
      return term_type(v, atermpp::make_list(v));
    }

    static inline
    bool is_constant(term_type d)
    {
      return d.variables().empty();
    }
/*    
    template <typename FwdIt>
    static
    term_type join_and(FwdIt first, FwdIt last)
    {
      return core::detail::join(first, last, and_, true_());
    }

    template <typename FwdIt>
    static
    term_type join_or(FwdIt first, FwdIt last)
    {
      return core::detail::join(first, last, or_, false_());
    }
*/    
  };
  
} // namespace core

} // namespace mcrl2

#endif // MCRL2_DATA_TERM_TRAITS_H
