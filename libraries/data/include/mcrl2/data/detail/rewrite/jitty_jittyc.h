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

inline variable_list get_vars(const atermpp::aterm_appl &a)
{
  if (is_variable(a))
  {
    return make_list(atermpp::aterm_cast<variable>(a));
  }
  else
  {
    variable_list l;
    for(atermpp::aterm_appl::const_iterator arg=a.begin(); arg!=a.end(); ++arg)
    {
      if (!is_function_symbol(atermpp::aterm_cast<const atermpp::aterm_appl>(*arg)))
      {
        l= get_vars(atermpp::aterm_cast<const atermpp::aterm_appl>(*arg))+l;
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

inline bool get_argument_of_higher_order_term_helper(const atermpp::aterm_appl& t, size_t& i, atermpp::aterm_appl& result)
{
  // t has the shape t #REWR#(....)
  if (is_function_symbol(atermpp::aterm_cast<const atermpp::aterm_appl>(t[0])))
  {
    const size_t arity = t.function().arity();
    if (arity>i)
    {
      result=atermpp::aterm_cast<atermpp::aterm_appl>(t[i]);
      return true;
    }
    // arity <=i
    i=i-arity+1;
    return false;
  }
  if (get_argument_of_higher_order_term_helper(atermpp::aterm_cast<atermpp::aterm_appl>(t[0]),i,result))
  {
    return true;
  }
  const size_t arity = t.function().arity();
  if (arity>i)
  {
    result=atermpp::aterm_cast<atermpp::aterm_appl>(t[i]);
    return true;
  }
  // arity <=i
  i=i-arity+1;
  return false;
}

inline atermpp::aterm_appl get_argument_of_higher_order_term(const atermpp::aterm_appl& t, size_t i)
{
  // t is a aterm of the shape #REWR#(#REWR#(...#REWR(f,t1,...tn),tn+1....),tm...). 
  // Return the i-th argument t_i. NOTE: The first argument has index 1.
  
  assert(!is_function_symbol(t));
  atermpp::aterm_appl result;
  bool b=get_argument_of_higher_order_term_helper(t,i,result);
  assert(b);
  return result;
}

inline size_t recursive_number_of_args(const atermpp::aterm_appl &t)
{
  if (is_function_symbol(t))
  {
    return 0;
  }

  const atermpp::aterm_appl& t0(t[0]);
  const size_t result=t.function().arity()+recursive_number_of_args(t0)-1;
  return result;
}


// Assume that the expression t is an application, and return its leading function symbol.
inline const function_symbol& get_function_symbol_of_head(const atermpp::aterm_appl &t)
{
  if (is_function_symbol(t))
  {
    return atermpp::aterm_cast<function_symbol>(t);
  }
  assert(t.type_is_appl());

  return get_function_symbol_of_head(atermpp::aterm_cast<atermpp::aterm_appl>(t[0]));
}

// Assume that the expression t is an application, and return its leading variable.
inline const variable& get_variable_of_head(const atermpp::aterm_appl &t)
{
  if (is_variable(t))
  {
    return atermpp::aterm_cast<variable>(t);
  }
  assert(t.type_is_appl());

  return get_variable_of_head(atermpp::aterm_cast<atermpp::aterm_appl>(t[0]));
}

inline bool head_is_variable(const atermpp::aterm_appl& t)
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
  return head_is_variable(atermpp::aterm_cast<const atermpp::aterm_appl>(t[0]));
}


inline bool head_is_function_symbol(const atermpp::aterm_appl& t, function_symbol& head)
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
  return head_is_function_symbol(atermpp::aterm_cast<const atermpp::aterm_appl>(t[0]),head);
}

}
}
}

#endif // REWRITE_JITTY_JITTYC_COMMON_H
