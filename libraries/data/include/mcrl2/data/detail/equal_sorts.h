// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/equal_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_EQUAL_SORTS_H
#define MCRL2_DATA_DETAIL_EQUAL_SORTS_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// \brief Checks if the sorts of the variables/expressions in both lists are equal.
/// \param v A sequence of data variables
/// \param w A sequence of data expressions
/// \return True if the sorts match pairwise
inline
bool equal_sorts(const data::variable_list& v, const data::data_expression_list& w, const data::data_specification& data_spec)
{
  if (v.size() != w.size())
  {
    return false;
  }
  data::variable_list::iterator i = v.begin();
  data::data_expression_list::iterator j = w.begin();
  for (; i != v.end(); ++i, ++j)
  {
    if (!data_spec.equal_sorts(i->sort(), j->sort()))
    {
      return false;
    }
  }
  return true;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_EQUAL_SORTS_H
