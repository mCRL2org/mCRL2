// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/consistency.h
/// \brief This file contains some functions that are present in all libraries
/// except the data library. In the data library they have different names, or
/// are located in different namespaces. Note that this file can not be used
/// everywhere, in particular not when people expose the sort_bool namespace.

#ifndef MCRL2_DATA_CONSISTENCY_H
#define MCRL2_DATA_CONSISTENCY_H

#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace data {

/// \brief Test if x is true
/// \param x a data expression
inline
bool is_true(const data_expression& x)
{
  return sort_bool::is_true_function_symbol(x);
}

/// \brief Test if x is false
/// \param x a data expression
inline
bool is_false(const data_expression& x)
{
  return sort_bool::is_false_function_symbol(x);
}

/// \brief Test if x is a negation
/// \param x a data expression
inline
bool is_not(const data_expression& x)
{
  return sort_bool::is_not_application(x);
}

/// \brief Test if x is a disjunction
/// \param x a data expression
inline
bool is_or(const data_expression& x)
{
  return sort_bool::is_or_application(x);
}

/// \brief Test if x is a conjunction
/// \param x a data expression
inline
bool is_and(const data_expression& x)
{
  return sort_bool::is_and_application(x);
}

/// \brief Test if x is an implication
/// \param x a data expression
inline
bool is_imp(const data_expression& x)
{
  return sort_bool::is_implies_application(x);
}

/// \brief Test if x is an equality
/// \param x a data expression
inline
bool is_equal_to(const data_expression& x)
{
  return is_equal_to_application(x);
}

/// \brief Test if x is an inequality
/// \param x a data expression
inline
bool is_not_equal_to(const data_expression& x)
{
  return is_not_equal_to_application(x);
}

/// \return The expression true
inline
const data_expression& true_()
{
  return sort_bool::true_();
}

/// \return The expression false
inline
const data_expression& false_()
{
  return sort_bool::false_();
}

/// \return The negation of x
inline
data_expression not_(const data_expression& x)
{
  return sort_bool::not_(x);
}

/// \return The disjunction of x and y
inline
data_expression or_(const data_expression& x, const data_expression& y)
{
  return sort_bool::or_(x, y);
}

/// \return The conjunction of x and y
inline
data_expression and_(const data_expression& x, const data_expression& y)
{
  return sort_bool::and_(x, y);
}

/// \return The implication of x and y
inline
data_expression imp(const data_expression& x, const data_expression& y)
{
  return sort_bool::implies(x, y);
}

/// \return Test if x is the boolean sort
inline
bool is_bool(const sort_expression& x)
{
  return sort_bool::is_bool(x);
}

/// \return The boolean sort
inline
sort_expression bool_()
{
  return sort_bool::bool_();
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONSISTENCY_H
