// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/join.h
/// \brief Generic join and split functions.

#ifndef MCRL2_CORE_DETAIL_JOIN_H
#define MCRL2_CORE_DETAIL_JOIN_H

namespace mcrl2 {

namespace core {

namespace detail {

  /// \brief Splits a binary tree T into a sequence, and writes the result to the output range
  /// given by an output iterator.
  /// \param t The tree that has to be split.
  /// \param i The output iterator.
  /// \param match If this functions returns true in a node, the node will be split.
  /// \param lhs Function for getting the left subtree of a node.
  /// \param rhs Function for getting the right subtree of a node.
  template <typename T, typename OutputIterator, typename MatchFunction, typename AccessorFunction>
  void split(T t, OutputIterator i, MatchFunction match, AccessorFunction lhs, AccessorFunction rhs)
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
  /// \param first [first, last[ is the range of elements.
  /// \param last
  /// \param op An operator
  /// \return The joined sequence
  template <typename T, typename FwdIt, typename BinaryOperation>
  T join(FwdIt first, FwdIt last, BinaryOperation op, T empty_sequence_result)
  {
    if(first == last)
      return empty_sequence_result;
    T result = *first++;
    while(first != last)
    {
      result = op(result, *first++);
    }
    return result;
  }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_JOIN_H
