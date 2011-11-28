// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_construction.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_CONSTRUCTION_H
#define MCRL2_DATA_DETAIL_DATA_CONSTRUCTION_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/standard.h"

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
#include "mcrl2/data/detail/print_parse_check.h"
#endif

namespace mcrl2 {

namespace data {

namespace detail {

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
template <class T> // note, T is only a dummy
struct data_specification_container
{
  static const data_specification* dataspec;
};

template <class T>
const data_specification* data_specification_container<T>::dataspec = 0;

inline
void set_data_specification(const data_specification& dataspec)
{
  data_specification_container<int>::dataspec = &dataspec;
}

inline
const data_specification& get_data_specification()
{
  return *data_specification_container<int>::dataspec;
}

template <typename T>
inline void data_construction_check_expression(const T& x)
{
  data::detail::print_parse_check(x, get_data_specification());
}
#else
template <typename T>
inline void data_construction_check_expression(const T&)
{}
#endif

/// \brief Create the finite set { x }, with x a data expression.
inline
data_expression create_finite_set(const data_expression& x)
{
  data_expression result = data::sort_fset::fset_empty(x.sort());
  result = data::sort_fset::fsetinsert(x.sort(), x, result);
  result = data::sort_set::setfset(x.sort(), result);
  data_construction_check_expression(result);
  return result;
}

/// \brief Create the set { x | phi }, with phi a predicate that may depend on the variable x.
inline
data_expression create_set_comprehension(const variable& x, const data_expression& phi)
{
  assert(sort_bool::is_bool(phi.sort()));
  data_expression result = sort_set::setconstructor(x.sort(), lambda(x, phi), sort_fset::fset_empty(x.sort()));
  data_construction_check_expression(result);
  return result;
}

/// \brief Create the predicate 'x in X', with X a set.
inline
data_expression create_set_in(const data_expression& x, const data_expression& X)
{
  data_expression result = sort_set::setin(x.sort(), x, X);
  data_construction_check_expression(result);
  return result;
}

/// \brief Returns the sort s of Set(s).
/// \param x A set expression
inline
sort_expression get_set_sort(const container_sort& x)
{
  return x.element_sort();
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_CONSTRUCTION_H
