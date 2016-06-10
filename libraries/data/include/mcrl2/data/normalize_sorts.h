// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/normalize_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_NORMALIZE_SORTS_H
#define MCRL2_DATA_NORMALIZE_SORTS_H

#include <functional>
#include "mcrl2/data/builder.h"
#include "mcrl2/data/sort_specification.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

struct normalize_sorts_function: public std::unary_function<data::sort_expression, data::sort_expression>
{
  /* const sort_specification& m_sort_spec; */
  const std::map< sort_expression, sort_expression >& m_normalised_aliases;

  normalize_sorts_function(const sort_specification& sort_spec)
    : m_normalised_aliases(sort_spec.sort_alias_map())
  {
  }

  ///\brief Normalise sorts.
  sort_expression operator()(const sort_expression& e) const
  {
    // This routine takes the map m_normalised_aliases which contains pairs of sort expressions
    // <A,B> and takes all these pairs as rewrite rules, which are applied to e using an innermost
    // strategy. Note that it is assumed that m_normalised_aliases contain rewrite rules <A,B>, such
    // that B is a normal form. This allows to check that if e matches A, then we can return B.

    const std::map< sort_expression, sort_expression >::const_iterator i1=m_normalised_aliases.find(e);
    if (i1!=m_normalised_aliases.end())
    {
      return i1->second;
    }

    sort_expression new_sort=e; // This will be a placeholder for the sort of which all
    // arguments will be normalised.

    // We do not have to do anything if e is a basic sort, as new_sort=e.
    if (is_function_sort(e))
    {
      // Rewrite the arguments into normal form.
      std::vector< sort_expression > new_domain;
      sort_expression_list e_domain(function_sort(e).domain());
      for (const sort_expression& sort: e_domain)
      {
        new_domain.push_back(this->operator()(sort));
      }
      new_sort=function_sort(new_domain, this->operator()(function_sort(e).codomain()));
    }
    else if (is_container_sort(e))
    {
      // Rewrite the argument of the container sort to normal form.
      new_sort=container_sort(
                 container_sort(e).container_name(),
                 this->operator()(container_sort(e).element_sort()));

    }
    else if (is_structured_sort(e))
    {
      // Rewrite the argument sorts to normal form.
      std::vector< structured_sort_constructor > new_constructors;
      const structured_sort_constructor_list& e_constructors(structured_sort(e).constructors());
      for (const structured_sort_constructor& e_constructor: e_constructors)
      {
        std::vector<structured_sort_constructor_argument> new_arguments;
        const structured_sort_constructor_argument_list& i_arguments(e_constructor.arguments());
        for (const structured_sort_constructor_argument& i_argument : i_arguments)
        {
          new_arguments.push_back(structured_sort_constructor_argument(
                                    i_argument.name(),
                                    this->operator()(i_argument.sort())));
        }
        new_constructors.push_back(structured_sort_constructor(e_constructor.name(), new_arguments, e_constructor.recogniser()));
      }
      new_sort=structured_sort(new_constructors);
    }

    // The arguments of new_sort are now in normal form.
    // Rewrite it to normal form.
    const std::map< sort_expression, sort_expression >::const_iterator i2=m_normalised_aliases.find(new_sort);
    if (i2!=m_normalised_aliases.end())
    {
      new_sort=this->operator()(i2->second); // rewrite the result until normal form.
    }
    return new_sort;
  }

};

} // namespace detail


template <typename T>
void normalize_sorts(T& x,
                     const data::sort_specification& sort_spec,
                     typename std::enable_if< !std::is_base_of<atermpp::aterm, T>::value >::type* = nullptr
                    )
{
  core::make_update_apply_builder<data::sort_expression_builder>
  (data::detail::normalize_sorts_function(sort_spec)).update(x);
} 

template <typename T>
T normalize_sorts(const T& x,
                  const data::sort_specification& sort_spec,
                  typename std::enable_if< std::is_base_of<atermpp::aterm, T>::value >::type* = nullptr
                 )
{
  return core::make_update_apply_builder<data::sort_expression_builder>
         (data::detail::normalize_sorts_function(sort_spec)).apply(x);
}

/* The functions below are defined as the function normalize_sorts
   above does not work on other sorts than sort expressions. 

inline sort_expression normalize_sorts(const basic_sort& x,
                                       const data::sort_specification& sort_spec)
{
  return normalize_sorts(static_cast<sort_expression>(x),sort_spec);
}

inline sort_expression normalize_sorts(const function_sort& x,
                                       const data::sort_specification& sort_spec)
{
  return normalize_sorts(static_cast<sort_expression>(x),sort_spec);
}

inline sort_expression normalize_sorts(const container_sort& x,
                                       const data::sort_specification& sort_spec)
{
  return normalize_sorts(static_cast<sort_expression>(x),sort_spec);
}

inline sort_expression normalize_sorts(const structured_sort& x,
                                       const data::sort_specification& sort_spec)
{
  return normalize_sorts(static_cast<sort_expression>(x),sort_spec);
}
*/

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NORMALIZE_SORTS_H
