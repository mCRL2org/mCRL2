// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/replace_constants_by_variables.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_REPLACE_CONSTANTS_BY_VARIABLES_H
#define MCRL2_LPS_REPLACE_CONSTANTS_BY_VARIABLES_H

#include "mcrl2/data/replace_constants_by_variables.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2::lps
{

namespace detail {

struct replace_constants_by_variables_builder: public data::detail::replace_constants_by_variables_builder<lps::data_expression_builder>
{
  using super = data::detail::replace_constants_by_variables_builder<lps::data_expression_builder>;
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
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
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
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  detail::replace_constants_by_variables_builder f(r, sigma);
  f.apply(result, x);
  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REPLACE_CONSTANTS_BY_VARIABLES_H
