// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_structural_order.h
/// \brief A total order on terms that does not depend on their addresses.

#ifndef MCRL2_ATERMPP_ATERM_STRUCTURAL_ORDER_H
#define MCRL2_ATERMPP_ATERM_STRUCTURAL_ORDER_H

#include <compare>

#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

/// \brief Compares two terms structurally.
/// \details The order on terms given by operator< (and operator<=>) compares the addresses of terms. It is efficient,
///          but differs between runs, so any choice based on it, such as taking the first element of a std::set of
///          terms, is nondeterministic. This order only depends on the structure of the terms: integers precede
///          applications and are ordered by value; applications are ordered on the name of the function symbol, then
///          on its arity, and then lexicographically on the arguments. The undefined term precedes all other terms.
///          The comparison is linear in the size of the terms, but stops at the first difference, and identical
///          subterms are recognised in constant time because terms are maximally shared.
///          The interface is that of operator<=> on terms, so the result can be used in the same way, e.g. with
///          std::is_lt.
/// \return std::weak_ordering::less if x precedes y, std::weak_ordering::equivalent if x and y are equal, and
///         std::weak_ordering::greater if y precedes x.
inline
std::weak_ordering structural_compare(const aterm& x, const aterm& y)
{
  const aterm* left = &x;
  const aterm* right = &y;

  // The last argument is handled iteratively, such that long lists do not lead to deep recursion.
  while (*left != *right)
  {
    if (!left->defined() || !right->defined())
    {
      return left->defined() ? std::weak_ordering::greater : std::weak_ordering::less;
    }
    if (left->type_is_int() || right->type_is_int())
    {
      if (left->type_is_int() && right->type_is_int())
      {
        return down_cast<aterm_int>(*left).value() <=> down_cast<aterm_int>(*right).value();
      }
      return left->type_is_int() ? std::weak_ordering::less : std::weak_ordering::greater;
    }

    const function_symbol& f = left->function();
    const function_symbol& g = right->function();
    if (f != g)
    {
      const std::weak_ordering c = f.name() <=> g.name();
      if (c != 0)
      {
        return c;
      }
      // Function symbols with the same name and arity are equal.
      return f.arity() <=> g.arity();
    }

    const std::size_t arity = f.arity();
    for (std::size_t i = 0; i + 1 < arity; ++i)
    {
      const std::weak_ordering c = structural_compare((*left)[i], (*right)[i]);
      if (c != 0)
      {
        return c;
      }
    }
    // Distinct terms with the same function symbol differ in an argument, so the arity is at least one here.
    assert(arity > 0);
    left = &(*left)[arity - 1];
    right = &(*right)[arity - 1];
  }
  return std::weak_ordering::equivalent;
}

/// \brief Function object that orders terms structurally; see structural_compare.
/// \details Can be used as the comparison of ordered containers, e.g. std::set<T, structural_less>, to make the
///          iteration order the same in every run.
struct structural_less
{
  bool operator()(const aterm& x, const aterm& y) const
  {
    return std::is_lt(structural_compare(x, y));
  }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_STRUCTURAL_ORDER_H
