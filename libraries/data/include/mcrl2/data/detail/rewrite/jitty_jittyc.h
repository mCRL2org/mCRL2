// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jitty_jittyc.h

#ifndef __REWR_JITTY_JITTYC_COMMON_H
#define __REWR_JITTY_JITTYC_COMMON_H

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

inline variable_list get_vars(const data_expression &a)
{
  if (is_variable(a))
  {
    return make_list(atermpp::aterm_cast<variable>(a));
  }
  else
  {
    variable_list l;
    for(data_expression::const_iterator arg=a.begin(); arg!=a.end(); ++arg)
    {
      if (!is_function_symbol(atermpp::aterm_cast<const data_expression>(*arg)))
      {
        l= get_vars(atermpp::aterm_cast<const data_expression>(*arg))+l;
      }
    }
    return l;
  }
}

inline sort_expression residual_sort(const sort_expression &s, size_t no_initial_arguments)
{
  // Remove no_initial_arguments sort from sort s.

  sort_expression result=s;
  for( ;  no_initial_arguments>0 ; )
  {
    assert(is_function_sort(result));
    const function_sort& sf(result);
    result=sf.codomain();
    assert(sf.domain().size()<=no_initial_arguments);
    no_initial_arguments=no_initial_arguments-sf.domain().size();
  }

  return result;

}

inline bool get_argument_of_higher_order_term_helper(const data_expression& t, size_t& i, data_expression& result)
{
  // t has the shape t #REWR#(....)
  if (is_function_symbol(atermpp::aterm_cast<const data_expression>(t[0])))
  {
    const size_t arity = t.function().arity();
    if (arity>i)
    {
      result=atermpp::aterm_cast<data_expression>(t[i]);
      return true;
    }
    // arity <=i
    i=i-arity+1;
    return false;
  }
  if (get_argument_of_higher_order_term_helper(atermpp::aterm_cast<data_expression>(t[0]),i,result))
  {
    return true;
  }
  const size_t arity = t.function().arity();
  if (arity>i)
  {
    result=atermpp::aterm_cast<data_expression>(t[i]);
    return true;
  }
  // arity <=i
  i=i-arity+1;
  return false;
}

inline data_expression get_argument_of_higher_order_term(const data_expression& t, size_t i)
{
  // t is a aterm of the shape #REWR#(#REWR#(...#REWR(f,t1,...tn),tn+1....),tm...). 
  // Return the i-th argument t_i. NOTE: The first argument has index 1.
  
  assert(!is_function_symbol(t));
  data_expression result;
  bool b=get_argument_of_higher_order_term_helper(t,i,result);
  assert(b);
  return result;
}

inline size_t recursive_number_of_args(const data_expression &t)
{
  if (is_function_symbol(t))
  {
    return 0;
  }

  const data_expression& t0(t[0]);
  const size_t result=t.function().arity()+recursive_number_of_args(t0)-1;
  return result;
}


// Assume that the expression t is an application, and return its leading function symbol.
inline const function_symbol& get_function_symbol_of_head(const data_expression &t)
{
  if (is_function_symbol(t))
  {
    return atermpp::aterm_cast<function_symbol>(t);
  }
  assert(t.type_is_appl());

  return get_function_symbol_of_head(atermpp::aterm_cast<data_expression>(t[0]));
}

// Assume that the expression t is an application, and return its leading variable.
inline const variable& get_variable_of_head(const data_expression &t)
{
  if (is_variable(t))
  {
    return atermpp::aterm_cast<variable>(t);
  }
  assert(t.type_is_appl());

  return get_variable_of_head(atermpp::aterm_cast<data_expression>(t[0]));
}

inline bool head_is_variable(const data_expression& t)
{
  assert(!is_where_clause(t));

  if (is_function_symbol(t))
  {
    return false;
  }
  if (is_variable(t))
  {
    // v=atermpp::aterm_cast<variable>(t);
    return true;
  }
  if (is_abstraction(t))
  {
    return false;
  }
  // shape is #REWR#(t1,...,tn)
  const application& ta(t);
  return head_is_variable(ta.head());
}


inline bool head_is_function_symbol(const data_expression& t, function_symbol& head)
{
  assert(!is_where_clause(t));

  if (is_function_symbol(t))
  {
    head=atermpp::aterm_cast<function_symbol>(t);
    return true;
  }
  if (is_variable(t))
  {
    return false;
  }
  if (is_abstraction(t))
  {
    return false;
  }
  // shape is #REWR#(t1,...,tn)
  const application& ta(t);
  return head_is_function_symbol(ta.head(),head);
}

}
}
}

#endif // REWRITE_JITTY_JITTYC_COMMON_H
