// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/sequence.h
/// \brief Functions for sequences.

#ifndef MCRL2_CORE_SEQUENCE_H
#define MCRL2_CORE_SEQUENCE_H

#include <iterator>
#include <vector>
#include <boost/utility.hpp> // next()

namespace mcrl2 {
  
namespace core {

namespace detail {

template <typename Iter1, typename Iter2, typename SequenceFunction>
void foreach_sequence_impl(Iter1 first, Iter1 last, Iter2 i, SequenceFunction f)
{
  if (first == last)
  {
    f();
  }
  else
  {
    for (typename std::iterator_traits<Iter1>::value_type::const_iterator j = first->begin(); j != first->end(); ++j)
    {
      *i = *j;
      foreach_sequence_impl(boost::next(first), last, boost::next(i), f);
    }
  }
}

} // namespace detail

/// Given a sequence [X1, ..., Xn], where each element Xi is a sequence
/// as well, this function generates all sequences [x1, ..., xn], where
/// xi is an element of Xi for all i = 1 ... n. For each of these sequences
/// the function f is called.
template <typename SequenceContainer, typename OutIter, typename SequenceFunction>
void foreach_sequence(const SequenceContainer& X, OutIter i, SequenceFunction f)
{
  typedef typename SequenceContainer::value_type::value_type value_type;
  detail::foreach_sequence_impl(X.begin(),
                                X.end(),
                                i,
                                f
                               );
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_SEQUENCE_H
