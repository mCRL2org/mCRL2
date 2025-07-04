// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
#define MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_symbol.h"





namespace mcrl2::data::detail
{

template <typename Term>
struct compare_term
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
struct compare_sort
{
  sort_expression s;

  compare_sort(sort_expression s_)
    : s(s_)
  {}

  bool operator()(const atermpp::aterm& t) const
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

/// \brief Function object that returns the name of a data variable
struct variable_name
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  core::identifier_string operator()(const variable& v) const
  {
    return v.name();
  }
};

/// \brief Function object that returns the sort of a data expression
template < typename Expression >
struct sort_of_expression
{
  /// \brief Function call operator
  /// \param v A data variable
  /// \return The function result
  sort_expression operator()(const Expression& e) const
  {
    return e.sort();
  }
};

/// \brief Function object that returns the sort of a data variable
using sort_of_variable = sort_of_expression<variable>;

struct sort_has_name
{
  std::string m_name;

  sort_has_name(std::string const& name)
    : m_name(name)
  {}

  /// \brief Function call operator
  /// \param s A sort expression
  /// \return The function result
  bool operator()(const sort_expression& s) const
  {
    return is_basic_sort(s) && std::string(basic_sort(s).name()) == m_name;
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
  bool operator()(const function_symbol& c) const
  {
    return std::string(c.name()) == m_name;
  }
};

} // namespace mcrl2::data::detail





#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
