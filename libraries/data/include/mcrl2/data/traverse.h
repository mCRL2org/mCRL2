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

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/variable.h"

#ifndef MCRL2_DATA_TRAVERSE_H
#define MCRL2_DATA_TRAVERSE_H

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
  *dest++ = v.sort();
  return dest;
}

/// \brief Traverses the data expression, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const data_expression& d, OutIter dest)
{
  *dest++ = d.sort();     
  return dest;
}

/// \brief Traverses the assignment, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
OutIter traverse_sort_expressions(const assignment& a, OutIter dest)
{
  *dest++ = a.lhs().sort();
  *dest++ = a.rhs().sort(); // TODO: can this be removed?
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
  // TODO: make this implementation more efficient
  std::set<data::variable> v = data::find_all_variables(d);
  for (std::set<data::variable>::iterator i = v.begin(); i != v.end(); ++i)
  {
    *dest++ = *i;
  }
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
