// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/join.h
/// \brief Generic join and split functions.

#ifndef MCRL2_UTILITIES_DETAIL_JOIN_H
#define MCRL2_UTILITIES_DETAIL_JOIN_H

#include <iterator>





namespace mcrl2::utilities::detail
{

/// \brief Splits a binary tree T into a sequence, and writes the result to the output range
/// given by an output iterator.
/// \param t The tree that has to be split.
/// \param i The output iterator.
/// \param match If this functions returns true in a node, the node will be split.
/// \param lhs Function for getting the left subtree of a node.
/// \param rhs Function for getting the right subtree of a node.
template <typename T, typename OutputIterator, typename MatchFunction, typename AccessorFunction1, typename AccessorFunction2>
void split(const T& t, OutputIterator i, MatchFunction match, AccessorFunction1 lhs, AccessorFunction2 rhs)
{
  if (match(t))
  {
    split(lhs(t), i, match, lhs, rhs);
    split(rhs(t), i, match, lhs, rhs);
  }
  else
  {
    *i++ = t;
  }
}

/// \brief Given a sequence [t1, t2, ..., tn] of elements of type T, returns
/// op(t1, op(t2, ...), tn)))).
/// \param empty_sequence_result The value that is returned when the sequence is empty.
/// \param first [first, last) is the range of elements.
/// \param last
/// \param op An operator
/// \return The joined sequence
template <typename T, typename FwdIt, typename BinaryOperation>
T join(FwdIt first, FwdIt last, BinaryOperation op, T empty_sequence_result)
{
  if (first == last)
  {
    return empty_sequence_result;
  }
  T result = *first++;
  while (first != last)
  {
    result = op(result, *first++);
  }
  return result;
}

/// \brief Given a non-empty sequence [t1, t2, ..., tn] of elements of type T, returns
/// op(op(t1, op(t2, ...), tn)))). The height of the resulting expression tree is minimal.
/// \param first [first, last) is the range of elements.
/// \param last
/// \param op An operator
/// \return The joined sequence
template <typename T, typename RndIt, typename BinaryOperation>
T join_balanced(RndIt first, RndIt last, BinaryOperation op)
{
  auto n = std::distance(first, last);
  if (n == 1)
  {
    return *first;
  }
  if (n == 2)
  {
    return op(*first, *(first + 1));
  }
  auto d = n / 2;
  auto left = join_balanced<T, RndIt, BinaryOperation>(first, first + d, op);
  auto right = join_balanced<T, RndIt, BinaryOperation>(first + d, last, op);
  return op(left, right);
}

} // namespace mcrl2::utilities::detail





#endif // MCRL2_UTILITIES_DETAIL_JOIN_H
