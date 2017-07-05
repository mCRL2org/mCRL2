// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite_statistics.h
/// \brief Global variable for collecting rewrite statistics.

#ifndef MCRL2_DATA_DETAIL_REWRITE_STATISTICS_H
#define MCRL2_DATA_DETAIL_REWRITE_STATISTICS_H

#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

template <class T> // note, T is only a dummy
struct rewrite_statistics
{
  static std::size_t rewrite_count;
};

template <class T>
std::size_t rewrite_statistics<T>::rewrite_count = 0;

inline
std::size_t rewrite_count()
{
  return rewrite_statistics<int>::rewrite_count;
}

inline
void display_rewrite_statistics()
{
  mCRL2log(log::verbose) << "rewrite count = " << rewrite_count() << std::endl;
}

inline
void increment_rewrite_count()
{
  rewrite_statistics<int>::rewrite_count++;
  if (rewrite_count() % 10000 == 0)
  {
    display_rewrite_statistics();
  }
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_REWRITE_STATISTICS_H
