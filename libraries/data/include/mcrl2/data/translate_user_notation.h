// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TRANSLATE_USER_NOTATION_H
#define MCRL2_DATA_TRANSLATE_USER_NOTATION_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/standard_container_utility.h"
#include "mcrl2/data/standard_utility.h"

namespace mcrl2::data
{

namespace detail
{

template <typename Derived>
class translate_user_notation_builder: public data_expression_builder<Derived>
{
public:
  using super = data_expression_builder<Derived>;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  inline
  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <class T>
  void apply(T& result, const abstraction& x)
  {
    const variable_list& bound_variables = x.variables();

    if (is_set_comprehension(x))
    {
      sort_expression element_sort(x.variables().begin()->sort());
      data_expression body;
      derived().apply(body, x.body());
      result = sort_set::constructor(element_sort, lambda(bound_variables, body),
                                   sort_fset::empty(element_sort));
      return;
    }
    else if (is_bag_comprehension(x))
    {
      sort_expression element_sort(x.variables().begin()->sort());
      data_expression body;
      derived().apply(body, x.body());
      result = sort_bag::constructor(element_sort, lambda(bound_variables, body),
                                   sort_fbag::empty(element_sort));
      return;
    }
    data_expression body;
    derived().apply(body, x.body());
    result = abstraction(x.binding_operator(), bound_variables, body);
  }

  template <class T>
  void apply(T& result, const function_symbol& x)
  {
    derived().enter(x);
    const std::string& name(x.name());
    if (is_system_defined(x.sort()) && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
    {
      result = number(x.sort(), name);
    }
    else 
    {
      result=x;
    }
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const application& x)
  {
    derived().enter(x);
    if (is_function_symbol(x.head()))
    {
      function_symbol head(x.head());

      if (head.name() == sort_list::list_enumeration_name())
      {
        // convert to snoc list
        sort_expression element_sort(*function_sort(head.sort()).domain().begin());
        
        // result = sort_list::list(element_sort, derived().apply(data_expression_list(x.begin(), x.end())));
        result = sort_list::list(element_sort, 
                                 data_expression_list(
                                           x.begin(), 
                                           x.end(),  
                                           [&](const data_expression& t) { data_expression r;  derived().apply(r, t); return r;} ));
        return;
      }
      else if (head.name() == sort_set::set_enumeration_name())
      {
        // convert to finite set
        sort_expression element_sort(*function_sort(head.sort()).domain().begin());
        result = sort_fset::fset(element_sort, 
                                 data_expression_list(
                                           x.begin(), 
                                           x.end(),  
                                           [&](const data_expression& t) { data_expression r;  derived().apply(r, t); return r;} ));
        return;
      }
      else if (head.name() == sort_bag::bag_enumeration_name())
      {
        // convert to finite bag
        sort_expression element_sort(*function_sort(head.sort()).domain().begin());
        result = sort_fbag::fbag(element_sort,
                                 data_expression_list(
                                           x.begin(), 
                                           x.end(),  
                                           [&](const data_expression& t) { data_expression r;  derived().apply(r, t); return r;} ));
        return;
      }
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      else if (head.name() == sort_nat::pos2nat_name())
      {
        // convert pos2nat(number) to a natural number. 
        data_expression n;
        derived().apply(n, x[0]);
        assert(n.sort()==sort_pos::pos());
        result=sort_nat::transform_positive_number_to_nat(n);
      }
#endif
    }
   
    make_application(result,
       x.head(),
       x.begin(),
       x.end(),
       [&](data_expression& r, const data_expression& t){ return derived().apply(r, t); }
    ); 
    derived().leave(x);
  }
};

struct translate_user_notation_function
{
  using argument_type = data_expression;
  using result_type = data_expression;

  data_expression operator()(const data_expression& x) const
  {
    data_expression result;
    core::make_apply_builder<translate_user_notation_builder>().apply(result, x);
    return result;
  }
};

} // namespace detail

template <typename T>
void translate_user_notation(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<data::data_expression_builder>(detail::translate_user_notation_function()).update(x);
}

template <typename T>
T translate_user_notation(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<data::data_expression_builder>(detail::translate_user_notation_function()).apply(result, x);
  return result;
}

namespace detail
{
// added to avoid circular header problems in data_specification.h
inline
data_equation translate_user_notation_data_equation(const data_equation& x)
{
  return translate_user_notation(x);
}
} // namespace detail

} // namespace mcrl2::data



#endif // MCRL2_DATA_TRANSLATE_USER_NOTATION_H
