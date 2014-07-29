// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/sequence.h
/// \brief Functions for sequences.

#ifndef MCRL2_UTILITIES_SEQUENCE_H
#define MCRL2_UTILITIES_SEQUENCE_H

#include <iterator>
#include <vector>

namespace mcrl2
{

namespace utilities
{

/// \cond INTERNAL_DOCS
namespace detail
{

/// \brief Assignment function object
struct foreach_sequence_assign
{
  /// \brief Function call operator
  /// \param t1 An object
  /// \param t2 A value
  template <typename T1, typename T2>
  void operator()(T1& t1, const T2& t2) const
  {
    t1 = t2;
  }
};

/// \brief Implementation of the foreach_sequence algorithm
/// \param first Start of a sequence container
/// \param last End of a sequence container
/// \param i An output iterator to where the generated sequences are written.
/// \param f Function that is called for each generated sequence
/// \param assign Assignment operator for assigning a value to a sequence element
template <typename Iter1, typename Iter2, typename SequenceFunction, typename Assign>
void foreach_sequence_impl(Iter1 first, Iter1 last, Iter2 i, SequenceFunction f, Assign assign)
{
  if (first == last)
  {
    f();
  }
  else
  {
    for (auto j = first->begin(); j != first->end(); ++j)
    {
      assign(*i, *j);
      foreach_sequence_impl(std::next(first), last, std::next(i), f, assign);
    }
  }
}

} // namespace detail
/// \endcond

/// \brief Algorithm for generating sequences.
/// Given a sequence [X1, ..., Xn], where each element Xi is a sequence
/// as well, this function generates all sequences [x1, ..., xn], where
/// xi is an element of Xi for all i = 1 ... n. For each of these sequences
/// the function f is called. The assign parameter gives the user control
/// over how each sequence is built.
/// \param X A sequence.
/// \param i An output iterator to where the generated sequences are written.
/// \param f A function that is called for each generated sequence.
/// \param assign The assign operation is called to assign values to the generated sequence.
template <typename SequenceContainer,
         typename OutIter,
         typename SequenceFunction,
         typename Assign>
void foreach_sequence(const SequenceContainer& X, OutIter i, SequenceFunction f, Assign assign)
{
  detail::foreach_sequence_impl(X.begin(),
                                X.end(),
                                i,
                                f,
                                assign
                               );
}

/// \brief Algorithm for generating sequences.
/// Given a sequence [X1, ..., Xn], where each element Xi is a sequence
/// as well, this function generates all sequences [x1, ..., xn], where
/// xi is an element of Xi for all i = 1 ... n. For each of these sequences
/// the function f is called.
/// \param X A sequence.
/// \param i An output iterator to where the generated sequences are written.
/// \param f A function that is called for each generated sequence.
template <typename SequenceContainer,
         typename OutIter,
         typename SequenceFunction>
void foreach_sequence(const SequenceContainer& X, OutIter i, SequenceFunction f)
{
  foreach_sequence(X, i, f, detail::foreach_sequence_assign());
}

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_SEQUENCE_H
