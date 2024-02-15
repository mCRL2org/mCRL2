// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace_constants_by_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REPLACE_CONSTANTS_BY_VARIABLES_H
#define MCRL2_PBES_REPLACE_CONSTANTS_BY_VARIABLES_H

#include "mcrl2/data/replace_constants_by_variables.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct replace_constants_by_variables_builder: public data::detail::replace_constants_by_variables_builder<pbes_system::data_expression_builder>
{
  typedef data::detail::replace_constants_by_variables_builder<pbes_system::data_expression_builder> super;
  using super::apply;
  using super::update;

  replace_constants_by_variables_builder(const data::rewriter& r, data::mutable_indexed_substitution<>& sigma)
    : super(r, sigma)
  {}
};

} // namespace detail

/// \brief Replace each constant data application c in x by a fresh variable v, and add extend the substitution sigma
/// with the assignment v := r(c). This can be used in rewriting, to avoid that c is rewritten by the rewriter
/// multiple times.
template <typename T>
void replace_constants_by_variables(T& x,
                                    const data::rewriter& r,
                                    data::mutable_indexed_substitution<>& sigma,
                                    typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                                   )
{
  detail::replace_constants_by_variables_builder f(r, sigma);
  f.update(x);
}

/// \brief Replace each constant data application c in x by a fresh variable v, and add extend the substitution sigma
/// with the assignment v := r(c). This can be used in rewriting, to avoid that c is rewritten by the rewriter
/// multiple times.
template <typename T>
T replace_constants_by_variables(const T& x,
                                 const data::rewriter& r,
                                 data::mutable_indexed_substitution<>& sigma,
                                 typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
                                )
{
  T result;
  detail::replace_constants_by_variables_builder f(r, sigma);
  f.apply(result, x);
  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REPLACE_CONSTANTS_BY_VARIABLES_H
