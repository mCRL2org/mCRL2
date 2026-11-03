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

/// \brief Compares two function symbols structurally.
/// \details Function symbols are ordered on their name, and then on their arity, which unlike operator< on function
///          symbols does not depend on their addresses. The undefined function symbol precedes all other function
///          symbols. Function symbols are unique for a name and arity, so two function symbols are only equal under
///          this order if they are the same function symbol.
/// \return std::strong_ordering::less if f precedes g, std::strong_ordering::equal if f and g are equal, and
///         std::strong_ordering::greater if g precedes f.
inline std::strong_ordering structural_compare(const function_symbol& f, const function_symbol& g)
{
  if (f == g)
  {
    return std::strong_ordering::equal;
  }
  if (!f.defined() || !g.defined())
  {
    return f.defined() ? std::strong_ordering::greater : std::strong_ordering::less;
  }
  const std::strong_ordering c = f.name() <=> g.name();
  if (c != 0)
  {
    return c;
  }
  // Function symbols with the same name and arity are equal.
  assert(f.arity() != g.arity());
  return f.arity() <=> g.arity();
}

/// \brief Compares two terms structurally.
/// \details The order on terms given by operator< (and operator<=>) compares the addresses of terms. It is efficient,
///          but differs between runs, so any choice based on it, such as taking the first element of a std::set of
///          terms, is nondeterministic. This order only depends on the structure of the terms: integers precede
///          applications and are ordered by value; applications are ordered on the name of the function symbol, then
///          on its arity, and then lexicographically on the arguments. The undefined term precedes all other terms.
///          The comparison is linear in the size of the terms, but stops at the first difference, and identical
///          subterms are recognised in constant time because terms are maximally shared.
///          Because terms are maximally shared and function symbols are unique for a name and arity, two terms are
///          only equal under this order if they are the same term, so the order is a std::strong_ordering.
/// \return std::strong_ordering::less if x precedes y, std::strong_ordering::equal if x and y are equal, and
///         std::strong_ordering::greater if y precedes x.
inline
std::strong_ordering structural_compare(const aterm& x, const aterm& y)
{
  const aterm* left = &x;
  const aterm* right = &y;

  // All arguments but the last are compared recursively, and the last argument iteratively by continuing the loop
  // with it. A list is a head followed by a tail that is again a list, so recursion on the tail would make the depth
  // of the recursion linear in the length of the list, and comparing lists of a million elements would exhaust the
  // stack (see test_long_lists in aterm_structural_order_test.cpp). Other terms that are deeply nested in an argument
  // other than the last one still lead to deep recursion.
  while (*left != *right)
  {
    if (!left->defined() || !right->defined())
    {
      return left->defined() ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    if (left->type_is_int() || right->type_is_int())
    {
      if (left->type_is_int() && right->type_is_int())
      {
        return down_cast<aterm_int>(*left).value() <=> down_cast<aterm_int>(*right).value();
      }
      return left->type_is_int() ? std::strong_ordering::less : std::strong_ordering::greater;
    }

    const function_symbol& f = left->function();
    const function_symbol& g = right->function();
    if (f != g)
    {
      return structural_compare(f, g);
    }

    const std::size_t arity = f.arity();
    for (std::size_t i = 0; i + 1 < arity; ++i)
    {
      const std::strong_ordering c = structural_compare((*left)[i], (*right)[i]);
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
  return std::strong_ordering::equal;
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
