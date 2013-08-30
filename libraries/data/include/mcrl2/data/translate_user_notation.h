// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/translate_user_notation.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TRANSLATE_USER_NOTATION_H
#define MCRL2_DATA_TRANSLATE_USER_NOTATION_H

#include <functional>
#include "mcrl2/data/builder.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/standard_container_utility.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

template <typename Derived>
class translate_user_notation_builder: public data_expression_builder<Derived>
{
  public:
    typedef data_expression_builder<Derived> super;

    using super::enter;
    using super::leave;
    using super::operator();

    data_expression operator()(const abstraction& x)
    {
      //variable_list bound_variables = atermpp::convert<variable_list>((*this)(x.variables()));
      variable_list bound_variables = x.variables();

      if (atermpp::function_symbol(atermpp::arg1(x).function()).name() == "SetComp")
      {
        //sort_expression element_sort((*this)(x.variables().begin()->sort()));
        sort_expression element_sort(x.variables().begin()->sort());
        return sort_set::constructor(element_sort, lambda(bound_variables, static_cast<Derived&>(*this)(x.body())),sort_fset::empty(element_sort));
      }
      else if (atermpp::function_symbol(atermpp::arg1(x).function()).name() == "BagComp")
      {
        sort_expression element_sort(x.variables().begin()->sort());

        return sort_bag::constructor(element_sort, lambda(bound_variables, static_cast<Derived&>(*this)(x.body())), sort_fbag::empty(element_sort));
      }
      return abstraction(x.binding_operator(), bound_variables, static_cast<Derived&>(*this)(x.body()));
    }

    data_expression operator()(const function_symbol& x)
    {
      static_cast<Derived&>(*this).enter(x);
      data_expression result = x;
      std::string name(x.name());
      if (is_system_defined(x.sort()) && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
      {
        result = number(x.sort(), name);
      }
      static_cast<Derived&>(*this).leave(x);

      return result;
    }

    data_expression operator()(const application& x)
    {
      static_cast<Derived&>(*this).enter(x);
      if (is_function_symbol(x.head()))
      {
        function_symbol head(x.head());

        if (head.name() == sort_list::list_enumeration_name()) 
        {
          // convert to snoc list
          sort_expression element_sort(*function_sort(head.sort()).domain().begin());

          return sort_list::list(element_sort, static_cast<Derived&>(*this)(x.arguments()));
        }
        else if (head.name() == sort_set::set_enumeration_name())
        {
          // convert to finite set
          sort_expression element_sort(*function_sort(head.sort()).domain().begin());

          return sort_set::set_fset(element_sort, sort_fset::fset(element_sort, static_cast<Derived&>(*this)(x.arguments())));
        }
        else if (head.name() == sort_bag::bag_enumeration_name())
        {
          // convert to finite bag
          sort_expression element_sort(*function_sort(head.sort()).domain().begin());
          return sort_bag::bag_fbag(element_sort, sort_fbag::fbag(element_sort, static_cast<Derived&>(*this)(x.arguments())));
        }
      }
      data_expression result = application(static_cast<Derived&>(*this)(x.head()), static_cast<Derived&>(*this)(x.arguments()));
      static_cast<Derived&>(*this).leave(x);
      return result;
    }
};

struct translate_user_notation_function: public std::unary_function<data_expression, data_expression>
{
  data_expression operator()(const data_expression& x)
  {
    return core::make_apply_builder<translate_user_notation_builder>()(x);
  }
};

} // namespace detail

template <typename T>
void translate_user_notation(T& x,
                             typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                            )
{
  core::make_update_apply_builder<data::data_expression_builder>(detail::translate_user_notation_function())(x);
}

template <typename T>
T translate_user_notation(const T& x,
                          typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                         )
{
  T result = core::make_update_apply_builder<data::data_expression_builder>(detail::translate_user_notation_function())(x);
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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRANSLATE_USER_NOTATION_H
