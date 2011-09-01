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
#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include <iterator>
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
#endif

/// \brief Create the finite set { x }, with x a data expression.
inline
data_expression create_finite_set(const data_expression& x)
{
  data_expression result = data::sort_fset::fset_empty(x.sort());
  result = data::sort_fset::fsetinsert(x.sort(), x, result);
  result = data::sort_set::setfset(x.sort(), result);

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
  std::clog << "<checking finite set>" << data::pp(x) << " : " << data::pp(x.sort()) << " " << x << std::endl;
  std::string text = data::pp(result);
  std::set<variable> v = data::find_free_variables(x);
  data_expression result1 = data::parse_data_expression(text, v.begin(), v.end(), get_data_specification());
  if (result != result1)
  {
    std::cout << "ERROR: in construction of " << text << std::endl;
    std::cout << " 1) " << result << std::endl;
    std::cout << " 2) " << result1 << std::endl;
  }
#endif

  return result;
}

/// \brief Create the set { x | phi }, with phi a predicate that may depend on the variable x.
inline
data_expression create_set_comprehension(const variable& x, const data_expression& phi)
{
  assert(sort_bool::is_bool(phi.sort()));
  data_expression result = sort_set::setconstructor(x.sort(), lambda(x, phi), sort_fset::fset_empty(x.sort()));

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
  std::clog << "<checking create_set_comprehension> x = " << data::pp(x) << ": " << data::pp(x.sort()) << x << " phi = " << data::pp(phi) << ": " << data::pp(phi.sort()) << std::endl;
  std::string text = data::pp(result);
  std::set<variable> v = data::find_free_variables(x);
  data::find_free_variables(phi, std::inserter(v, v.end()));
  data_expression result1 = data::parse_data_expression(text, v.begin(), v.end(), get_data_specification());
  if (result != result1)
  {
    std::cout << "ERROR: in construction of " << text << std::endl;
    std::cout << " 1) " << result << std::endl;
    std::cout << " 2) " << result1 << std::endl;
  }
#endif

  return result;
}

/// \brief Create the predicate 'x in X', with X a set.
inline
data_expression create_set_in(const variable& x, const data_expression& X)
{
  data_expression result = sort_set::setin(x.sort(), x, X);

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
  std::clog << "<checking create_set_in> x = " << data::pp(x) << ": " << data::pp(x.sort()) << x << " X = " << data::pp(X) << ": " << data::pp(X.sort()) << std::endl;
  std::string text = data::pp(result);
  std::set<variable> v = data::find_free_variables(x);
  data::find_free_variables(X, std::inserter(v, v.end()));
  data_expression result1 = data::parse_data_expression(text, v.begin(), v.end(), get_data_specification());
  if (result != result1)
  {
    std::cout << "ERROR: in construction of " << text << std::endl;
    std::cout << " 1) " << result << std::endl;
    std::cout << " 2) " << result1 << std::endl;
  }
#endif

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
