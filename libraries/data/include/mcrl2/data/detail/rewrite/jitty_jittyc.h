// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_DATA_DETAIL_REWRITE_JITTY_JITTYC_H
#define MCRL2_DATA_DETAIL_REWRITE_JITTY_JITTYC_H

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/function_update.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

template <template <class> class Traverser>
struct double_variable_traverser : public Traverser<double_variable_traverser<Traverser> >
{
  typedef Traverser<double_variable_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<variable> m_seen;
  std::set<variable> m_doubles;

  void apply(const variable& v)
  {
    if (!m_seen.insert(v).second)
    {
      m_doubles.insert(v);
    }
  }

  const std::set<variable>& result()
  {
    return m_doubles;
  }
};


inline variable_list get_free_vars(const data_expression& a)
{
  const std::set<variable> s = find_free_variables(a);
  return variable_list(s.begin(), s.end());
}

inline sort_expression residual_sort(const sort_expression& s, std::size_t no_of_initial_arguments)
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

// This function returns <b, arg_i> where arg_i is the i-th argument of the application t and
// b is a boolean that indicates whether this argument exists. 
// TODO: use the optional class of c++17 when it can be used. The current solution is somewhat ugly. 
// This function yields a pointer to argument_i if it exists. If not it yields the nullptr;
inline const data_expression* get_argument_of_higher_order_term_helper(const application& t, std::size_t& i)
{
  // t has the shape t application(....)
  if (!is_application(t.head()))
  {
    const std::size_t arity = t.size();
    if (arity>i)
    {
      return &t[i];
    }
    // arity <=i
    i=i-arity;
    return nullptr;
  }
  const data_expression* p=get_argument_of_higher_order_term_helper(atermpp::down_cast<application>(t.head()),i);
  if (p!=nullptr)
  {
    return p;
  }
  const std::size_t arity = t.size();
  if (arity>i)
  {
    return &t[i];
  }
  // arity <=i
  i=i-arity;
  return nullptr;
}

// This function returns the i-th argument t_i. NOTE: The first argument has index 1.
// t is an applicatoin of the shape application(application(...application(f,t1,...tn),tn+1....),tm...).
// i must be a valid index of an argument. 
inline const data_expression& get_argument_of_higher_order_term(const application& t, std::size_t i)
{
  // This first case applies to the majority of cases. Therefore this cheap check is done first, before
  // going into a recursive algorithm. Furthermore checking that t is a function_symbol, etc. is cheaper
  // than checking that t is not an application by !is_application(t.head()). 
  const data_expression& head=t.head();
  if (is_function_symbol(head) ||
      is_variable(head) ||
      is_where_clause(head) ||
      is_abstraction(head))
  {
    assert(t.size()>i);
    return t[i];
  }

  const data_expression* p=get_argument_of_higher_order_term_helper(t,i);
  assert(p!=nullptr);
  return *p;
}

inline std::size_t recursive_number_of_args(const data_expression& t)
{
  // Checking these cases is together more efficient than
  // checking whether t is an application. 
  if (is_function_symbol(t) ||
      is_variable(t) ||
      is_where_clause(t) ||
      is_abstraction(t))
  {
    return 0;
  }
 
  assert(is_application(t));
  
  const application& ta = atermpp::down_cast<application>(t);
  return ta.size()+recursive_number_of_args(ta.head());
}

// Return the head symbol, nested within applications.
// This helper function is used to allow inlining of the function
// get_nested_head.
inline const data_expression& get_nested_head_helper(const application& t)
{
  if (is_application(t.head()))
  {
    return get_nested_head_helper(atermpp::down_cast<application>(t.head()));
  }

  return t.head();
}

// Return the head symbol, nested within applications.
inline const data_expression& get_nested_head(const data_expression& t)
{
  if (is_function_symbol(t) ||
      is_variable(t) ||
      is_where_clause(t) ||
      is_abstraction(t))
  {
    return t;
  }

  const application& ta = atermpp::down_cast<application>(t);
  const data_expression& head=ta.head();

  if (is_function_symbol(head) ||
      is_variable(head) ||
      is_where_clause(head) ||
      is_abstraction(head))
  {
    return head;
  }
  return get_nested_head_helper(atermpp::down_cast<application>(head));
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

}
}
}

#endif // REWRITE_JITTY_JITTYC_COMMON_H
