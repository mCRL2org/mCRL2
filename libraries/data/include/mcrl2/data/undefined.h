// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/undefined.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNDEFINED_H
#define MCRL2_DATA_UNDEFINED_H

#ifdef MCRL2_ENABLE_MACHINENUMBERS
#include "mcrl2/data/real64.h"
#else
#include "mcrl2/data/real.h"
#endif



namespace mcrl2::data {

/// \brief Returns an index that corresponds to 'undefined'
inline
constexpr std::size_t undefined_index()
{
  return std::size_t(-1);
}

/// \brief Returns a data variable that corresponds to 'undefined'
inline
const data::variable& undefined_variable()
{
  static data::variable v("@undefined_variable", data::sort_expression());
  return v;
}

/// \brief Returns a data variable that corresponds to 'undefined'
inline
const data::variable& undefined_real_variable()
{
  static data::variable v("@undefined_real_variable", data::sort_real::real_());
  return v;
}

/// \brief Returns a sort expression that corresponds to 'undefined'
inline
const data::sort_expression& undefined_sort_expression()
{
  static data::basic_sort v("@undefined_sort_expression");
  return v;
}

/// \brief Returns a data expression that corresponds to 'undefined'
inline
const data::data_expression& undefined_data_expression()
{
  static data::variable v("@undefined_data_expression", data::sort_expression());
  return v;
}

/// \brief Returns a data expression of type Real that corresponds to 'undefined'
inline
const data::data_expression& undefined_real()
{
  static data::variable r("@undefined_real", data::sort_real::real_());
  return r;
}

} // namespace mcrl2::data



#endif // MCRL2_DATA_UNDEFINED_H
