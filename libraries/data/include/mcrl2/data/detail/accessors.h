// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/accessors.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_ACCESSORS_H
#define MCRL2_DATA_DETAIL_ACCESSORS_H

#include "mcrl2/data/application.h"

namespace mcrl2 {

namespace data {

namespace detail {

namespace data_accessors {

inline
const data::data_expression &left(const data::data_expression& x)
{
  return data::application(x).left();
}

inline
const data::data_expression &right(const data::data_expression& x)
{
  return data::application(x).right();
}

inline
const data::data_expression &arg(const data::data_expression& x)
{
  return *(data::application(x).begin());
}

inline
const data::data_expression &argument(const data::data_expression& x, const size_t n)
{
  const data::application a(x);
  data::application::const_iterator arg = a.begin();
  for(size_t i = 0; i < n; ++i, ++arg)
  {}
  return *arg;
}

} // namespace data_accessors

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ACCESSORS_H
