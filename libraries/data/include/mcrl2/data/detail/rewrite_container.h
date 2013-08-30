// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite_container.h
/// \brief Functions for applying a data rewriter to a container of data expressions.

#ifndef MCRL2_DATA_DETAIL_REWRITE_CONTAINER_H
#define MCRL2_DATA_DETAIL_REWRITE_CONTAINER_H

#include <vector>
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

template <typename ExpressionContainer, typename Rewriter>
void rewrite_container(ExpressionContainer& v, const Rewriter& rewr)
{
  for (typename ExpressionContainer::iterator i = v.begin(); i != v.end(); ++i)
  {
    *i = rewr(*i);
  }
}

template <typename ExpressionContainer, typename Rewriter, typename Substitution>
void rewrite_container(ExpressionContainer& v, const Rewriter& rewr, const Substitution& sigma)
{
  for (typename ExpressionContainer::iterator i = v.begin(); i != v.end(); ++i)
  {
    *i = rewr(*i, sigma);
  }
}

template <typename Rewriter>
void rewrite_container(data::data_expression_list& v, const Rewriter& rewr)
{
  std::vector<data_expression> tmp(v.begin(), v.end());
  rewrite_container(tmp, rewr);
  v = data::data_expression_list(tmp.begin(),tmp.end());
}

template <typename Rewriter, typename Substitution>
void rewrite_container(data::data_expression_list& v, const Rewriter& rewr, const Substitution& sigma)
{
  std::vector<data_expression> tmp(v.begin(), v.end());
  rewrite_container(tmp, rewr, sigma);
  v = data::data_expression_list(tmp.begin(),tmp.end());
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_REWRITE_CONTAINER_H
