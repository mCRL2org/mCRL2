// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/old_data/data_operators.h
/// \brief Some operators for list concatenation.

#ifndef MCRL2_OLD_DATA_DATA_OPERATORS_H
#define MCRL2_OLD_DATA_DATA_OPERATORS_H

#include "mcrl2/old_data/data.h"

namespace mcrl2 {

namespace old_data {

/// \brief Returns the concatenation of the lists l and m
inline
data_expression_list operator+(data_expression_list l, data_variable_list m)
{ return data_expression_list(ATconcat(l, m)); }

/// \brief Returns the concatenation of the lists l and m.
inline
data_expression_list operator+(data_variable_list l, data_expression_list m)
{ return data_expression_list(ATconcat(l, m)); }

/// \brief Returns the concatenation of [v] and the list l.
inline
data_expression_list operator+(data_variable v, data_expression_list l)
{
  return data_expression(atermpp::aterm_appl(v)) + l;
}

/// \brief Returns the concatenation of the list l and [v].
inline
data_expression_list operator+(data_expression_list l, data_variable v)
{
  return l + data_expression(atermpp::aterm_appl(v));
}

} // namespace old_data

} // namespace mcrl2

#endif // MCRL2_OLD_DATA_DATA_OPERATORS_H
