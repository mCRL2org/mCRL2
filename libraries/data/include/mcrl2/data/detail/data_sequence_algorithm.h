// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_sequence_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H
#define MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H

#include <set>
#include <vector>
#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace data {

namespace detail {

  /// \brief Removes elements from a sequence
  /// \param l A sequence of terms
  /// \param to_be_removed A set of terms
  /// \return The removal result
  template <typename Term>
  variable_list remove_elements(variable_list l, const std::set<Term>& to_be_removed)
  {
    variable_list result;
    for (variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
    {
      if (to_be_removed.find(*i) == to_be_removed.end())
      {
        result = push_front(result, *i);
      }
    }
    return reverse(result);
  }

  /// \brief Returns the difference of two unordered sets, that are stored in ATerm lists.
  /// \param x A sequence of data variables
  /// \param y A sequence of data variables
  /// \return The difference of two sets.
  inline
  variable_list set_difference(variable_list x, variable_list y)
  {
    if (x == y)
    {
      return x;
    }

    // We assume that in the majority of cases no variables are removed.
    // Therefore we only do the expensive ATerm list construction if it
    // is really needed.
    std::set<variable> to_be_removed;
    for (variable_list::iterator i = x.begin(); i != x.end(); ++i)
    {
      if (std::find(y.begin(), y.end(), *i) == y.end())
      {
        to_be_removed.insert(*i);
      }
    }
    if (to_be_removed.empty())
    {
      return x;
    }
    return remove_elements(x, to_be_removed);
  }

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_SEQUENCE_ALGORITHM_H
