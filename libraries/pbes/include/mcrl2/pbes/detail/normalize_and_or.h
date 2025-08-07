// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/normalize_and_or.h
/// \brief Function to normalize 'and' and 'or' sub expressions.

#ifndef MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H
#define MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H

#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/join.h"

namespace mcrl2::pbes_system::detail
{

// Simplifying PBES rewriter.
template <typename Derived>
struct normalize_and_or_builder: public pbes_expression_builder<Derived>
{
  using super = pbes_expression_builder<Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  /// \brief Splits a disjunction into a sequence of operands
  /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
  /// function symbol.
  /// \param expr A PBES expression
  /// \return A sequence of operands
  std::multiset<pbes_expression> split_or(const pbes_expression& expr)
  {
    using namespace accessors;
    std::multiset<pbes_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::multiset<pbes_expression> >(result, result.begin()), is_or, left, right);
    return result;
  }

  /// \brief Splits a conjunction into a sequence of operands
  /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
  /// function symbol.
  /// \param expr A PBES expression
  /// \return A sequence of operands
  std::multiset<pbes_expression> split_and(const pbes_expression& expr)
  {
    using namespace accessors;
    std::multiset<pbes_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::multiset<pbes_expression> >(result, result.begin()), is_and, left, right);
    return result;
  }

  pbes_expression normalize(const pbes_expression& x)
  {
    if (is_and(x))
    {
      std::multiset<pbes_expression> s = split_and(x);
      return join_and(s.begin(), s.end());
    }
    else if (is_or(x))
    {
      std::multiset<pbes_expression> s = split_or(x);
      return join_or(s.begin(), s.end());
    }
    return x;
  }

  // to prevent default operator() being called
  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = x;
  }

  template <class T>
  void apply(T& result, const pbes_expression& x)
  {
    super::apply(result, x);
    result = normalize(result);
  }
};

template <typename T>
T normalize_and_or(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_apply_builder<normalize_and_or_builder>().apply(result, x);
  return result;
}

template <typename T>
void normalize_and_or(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_apply_builder<normalize_and_or_builder>().update(x);
}

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_DETAIL_NORMALIZE_AND_OR_H
