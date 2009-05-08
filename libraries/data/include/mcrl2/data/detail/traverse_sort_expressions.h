// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/traverse_sort_expressions.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_TRAVERSE_SORT_EXPRESSIONS_H
#define MCRL2_DATA_DETAIL_TRAVERSE_SORT_EXPRESSIONS_H

namespace mcrl2 {

namespace data {

namespace detail {

  /// \brief Traverses the container, and calls traverse_sort_expressions
  /// for all its elements.
  template <typename Container, typename OutIter>
  void traverse_sort_expressions(const Container& c, OutIter dest)
  {
    for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
    {
      traverse_sort_expressions(*i, dest);
    }
  }

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_TRAVERSE_SORT_EXPRESSIONS_H
