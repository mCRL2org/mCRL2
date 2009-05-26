// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
#define MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/function_symbol.h"

namespace mcrl2 {

namespace data {

namespace detail {

inline bool is_basic_sort(atermpp::aterm_appl p) {
  return sort_expression(p).is_basic_sort();
}

inline bool is_function_sort(atermpp::aterm_appl p) {
  return sort_expression(p).is_function_sort();
}

inline bool is_container_sort(atermpp::aterm_appl p) {
  return sort_expression(p).is_container_sort();
}

inline bool is_structured_sort(atermpp::aterm_appl p) {
  return sort_expression(p).is_structured_sort();
}

inline bool is_variable(atermpp::aterm_appl p) {
  return data_expression(p).is_variable();
}

inline bool is_function_symbol(atermpp::aterm_appl p) {
  return data_expression(p).is_function_symbol();
}

template <typename Term>
struct compare_term: public std::unary_function<atermpp::aterm_appl, bool>
{
  const Term& term;

  compare_term(const Term& t)
   : term(t)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  template <typename Term2>
  bool operator()(Term2 t) const
  {
    return term == t;
  }
};

/// Tests if a term is a sort, and if it is equal to s
struct compare_sort : public std::unary_function< bool, atermpp::aterm_appl >
{
  sort_expression s;

  compare_sort(sort_expression s_)
    : s(s_)
  {}

  bool operator()(atermpp::aterm_appl t) const
  {
    return is_sort_expression(t) && s == t;
  }
};

/// \brief Function object that determines if a term is equal to a given data variable.
struct compare_variable: public compare_term<variable>
{
  compare_variable(const variable& v)
   : compare_term<variable>(v)
  {}
};

/// Function object that returns true if the expressions x and y have the same sort.
struct equal_data_expression_sort: public std::binary_function<data_expression, data_expression, bool>
{
  /// \brief Function call operator
  /// \param x A data expression
  /// \param y A data expression
  /// \return The function result
  bool operator()(const data_expression& x, const data_expression& y) const
  {
    return x.sort() == y.sort();
  }
};

/// \brief Function object that returns the name of a data variable
struct variable_name: public std::unary_function<variable, core::identifier_string>
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  core::identifier_string operator()(const variable& v) const
  {
    return v.name();
  }
};

/// \brief Function object that returns the name of a data expression
template < typename Expression >
struct sort_of_expression: public std::unary_function< Expression, sort_expression >
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  sort_expression operator()(const Expression& e) const
  {
    return e.sort();
  }
};

/// \brief Function object that returns the name of a data variable
typedef sort_of_expression< variable > sort_of_variable;

struct sort_has_name
{
  std::string m_name;

  sort_has_name(std::string const& name)
    : m_name(name)
  {}

  /// \brief Function call operator
  /// \param s A sort expression
  /// \return The function result
  bool operator()(sort_expression s) const
  {
    return s.is_basic_sort() && std::string(basic_sort(s).name()) == m_name;
  }
};

struct function_symbol_has_name
{
  std::string m_name;

  function_symbol_has_name(std::string const& name)
    : m_name(name)
  {}

  /// \brief Function call operator
  /// \param c A data operation
  /// \return The function result
  bool operator()(function_symbol c) const
  {
    return std::string(c.name()) == m_name;
  }
};

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
