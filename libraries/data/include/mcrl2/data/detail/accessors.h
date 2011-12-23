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
data::data_expression left(const data::data_expression& x)
{
  return data::application(x).left();
}

inline
data::data_expression right(const data::data_expression& x)
{
  return data::application(x).right();
}

inline
data::data_expression arg(const data::data_expression& x)
{
  return data::application(x).arguments().front();
}

} // namespace data_accessors

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ACCESSORS_H
