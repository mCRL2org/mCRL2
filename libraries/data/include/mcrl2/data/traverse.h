// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/traverse.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TRAVERSE_H
#define MCRL2_DATA_TRAVERSE_H

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/find.h"

namespace mcrl2 {

namespace data {

/// \brief Traverses the sort expression, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const sort_expression& s, OutIter dest)
{
  *dest++ = s;
  return dest;
}

/// \brief Traverses the variable, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const variable& v, OutIter dest)
{
  find_sort_expressions(v, dest);
  return dest;
}

/// \brief Traverses the data expression, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const data_expression& d, OutIter dest)
{
  find_sort_expressions(d, dest);
  return dest;
}

/// \brief Traverses the assignment, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const assignment& a, OutIter dest)
{
  find_sort_expressions(a.lhs(), dest);
  find_sort_expressions(a.rhs(), dest); // TODO: can this be removed?
  return dest;
}

/// \brief Traverses the variable, and writes all variables
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_variables(const variable& v, OutIter dest)
{
  *dest++ = v;
  return dest;
}

/// \brief Traverses the data expression, and writes all variables
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_variables(const data_expression& d, OutIter dest)
{
  data::find_variables(d, dest);
  return dest;
}

/// \brief Traverses the assignment, and writes all variables
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_variables(const assignment& a, OutIter dest)
{
  dest = traverse_variables(a.lhs(), dest);
  dest = traverse_variables(a.rhs(), dest);
  return dest;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TRAVERSE_H
