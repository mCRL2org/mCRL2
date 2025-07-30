// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/if_rewrite.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_IF_REWRITE_H
#define MCRL2_LPS_IF_REWRITE_H

#include "mcrl2/data/rewriters/if_rewriter.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2::lps
{

namespace detail {

struct if_rewrite_builder: public lps::data_expression_builder<if_rewrite_builder>
{
  using super = lps::data_expression_builder<if_rewrite_builder>;
  using super::apply;
  using super::update;

  data::if_rewriter r;

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = r(x);
  }
};

} // namespace detail

/// \brief Applies the one point rule rewriter to all embedded data expressions in an object x
/// \param x an object containing data expressions
template <typename T>
void if_rewrite(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  detail::if_rewrite_builder f{};
  f.update(x);
}

/// \brief Applies the one point rule rewriter to all embedded data expressions in an object x
/// \param x an object containing data expressions
/// \return the rewrite result
template <typename T>
T if_rewrite(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  detail::if_rewrite_builder f{};
  f.apply(result, x);
  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_IF_REWRITE_H
