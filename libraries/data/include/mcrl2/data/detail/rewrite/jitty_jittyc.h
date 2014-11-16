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

inline variable_list get_vars(const data_expression& a)
{
  const std::set<variable> s=find_free_variables(a);
  return variable_list(s.begin(),s.end());
}

inline sort_expression residual_sort(const sort_expression& s, size_t no_of_initial_arguments)
{
  // Remove no_of_initial_arguments sort from sort s.

  sort_expression result=s;
  while (no_of_initial_arguments > 0)
  {
    assert(is_function_sort(result));
    const function_sort& sf = atermpp::down_cast<function_sort>(result);
    assert(sf.domain().size()<=no_of_initial_arguments);
    no_of_initial_arguments=no_of_initial_arguments-sf.domain().size();
    result=sf.codomain();
  }

  return result;
}

inline bool get_argument_of_higher_order_term_helper(const application& t, size_t& i, data_expression& result)
{
  // t has the shape t application(....)
  if (!is_application(t.head()))
  {
    const size_t arity = t.size();
    if (arity>i)
    {
      result=t[i];
      return true;
    }
    // arity <=i
    i=i-arity;
    return false;
  }
  if (get_argument_of_higher_order_term_helper(atermpp::down_cast<application>(t.head()),i,result))
  {
    return true;
  }
  const size_t arity = t.size();
  if (arity>i)
  {
    result=t[i];
    return true;
  }
  // arity <=i
  i=i-arity;
  return false;
}

inline data_expression get_argument_of_higher_order_term(const data_expression& t, size_t i)
{
  // t is a aterm of the shape application(application(...application(f,t1,...tn),tn+1....),tm...).
  // Return the i-th argument t_i. NOTE: The first argument has index 1.

  data_expression result;
#ifndef NDEBUG // avoid a warning.
  bool b=
#endif
          get_argument_of_higher_order_term_helper(atermpp::down_cast<application>(t),i,result);
  assert(b);
  return result;
}

inline size_t recursive_number_of_args(const data_expression& t)
{
  if (is_function_symbol(t))
  {
    return 0;
  }

  if (is_variable(t))
  {
    return 0;
  }
 
  if (is_where_clause(t))
  {
    return 0;
  }

  if (is_abstraction(t))
  {
    return 0;
  }

  const application& ta = atermpp::down_cast<application>(t);
  const size_t result=ta.size()+recursive_number_of_args(ta.head());
  return result;
}

// Assume that the expression t is an application, and return its leading function symbol.
inline const function_symbol& get_function_symbol_of_head(const data_expression& t)
{
  if (is_function_symbol(t))
  {
    return atermpp::down_cast<function_symbol>(t);
  }
  assert(t.type_is_appl());

  const application& ta = atermpp::down_cast<application>(t);
  return get_function_symbol_of_head(ta.head());
}

// Return the head symbol, nested within applications.
inline const data_expression& get_nested_head(const data_expression& t)
{
  if (is_application(t))
  {
    const application& ta = atermpp::down_cast<application>(t);
    return get_nested_head(ta.head());
  }

  return t;
}

// Replace the recursively nested head symbol in t by head.
inline const data_expression replace_nested_head(const data_expression& t, const data_expression& head)
{
  if (is_application(t))
  {
    const application& ta = atermpp::down_cast<application>(t);
    return application(replace_nested_head(ta.head(),head),ta.begin(),ta.end());
  }

  return head;
}

template <class ARGUMENT_REWRITER>
inline const data_expression rewrite_all_arguments(const application& t, const ARGUMENT_REWRITER rewriter)
{
  return application(t.head(),t.begin(),t.end(),rewriter);
}

// Assume that the expression t is an application, and return its leading variable.
inline const variable& get_variable_of_head(const data_expression& t)
{
  if (is_variable(t))
  {
    return atermpp::down_cast<variable>(t);
  }
  assert(t.type_is_appl());

  return get_variable_of_head(atermpp::down_cast<data_expression>(t[0]));
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
    // v=atermpp::down_cast<variable>(t);
    return true;
  }
  if (is_abstraction(t))
  {
    return false;
  }
  // shape is application(t1,...,tn)
  const application& ta = atermpp::down_cast<application>(t);
  return head_is_variable(ta.head());
}


inline bool head_is_function_symbol(const data_expression& t, function_symbol& head)
{
  if (is_application(t))
  {
    const application& ta = atermpp::down_cast<application>(t);
    return head_is_function_symbol(ta.head(),head);
  }

  if (is_function_symbol(t))
  {
    head=atermpp::down_cast<function_symbol>(t);
    return true;
  }

  return false;
}

}
}
}

#endif // REWRITE_JITTY_JITTYC_COMMON_H
