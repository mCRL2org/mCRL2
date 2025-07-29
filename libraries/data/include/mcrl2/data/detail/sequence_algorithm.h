// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/sequence_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_SEQUENCE_ALGORITHM_H
#define MCRL2_DATA_DETAIL_SEQUENCE_ALGORITHM_H

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

namespace mcrl2::data::detail
{

/// \brief Returns true if the sequence [first, last) contains duplicates.
/// \param first Start of a sequence
/// \param last End of a sequence
/// \return True if the sequence [first, last) contains duplicates.
template <typename Iterator>
bool sequence_contains_duplicates(Iterator first, Iterator last)
{
  // TODO: this implementation is not particularly efficient
  std::set<typename std::iterator_traits<Iterator>::value_type> s(first, last);
  return s.size() < static_cast <std::size_t>(std::distance(first, last));
}

/// \brief Returns true if the two sequences [first1, last1) and [first2, last2) have a non empty intersection.
/// \param first1 Start of a sequence
/// \param last1 End of a sequence
/// \param first2 Start of a sequence
/// \param last2 End of a sequence
/// \return True if the two sequences [first1, last1) and [first2, last2) have a non empty intersection.
template <typename Iterator1, typename Iterator2>
bool sequences_do_overlap(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2)
{
  using value_type = typename std::iterator_traits<Iterator1>::value_type;
  std::set<value_type> s1(first1, last1);
  std::set<value_type> s2(first2, last2);
  std::vector<value_type> intersection;
  std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(intersection));
  return !intersection.empty();
}

/// \brief Returns true if all elements of the range [first, last) are element of the set s.
/// \param first Start of a sequence
/// \param last End of a sequence
/// \param s A set
/// \return True if all elements of the range [first, last) are element of the set s.
template <typename Iterator, typename T>
bool sequence_is_subset_of_set(Iterator first, Iterator last, const std::set<T>& s)
{
  for (Iterator i = first; i != last; ++i)
  {
    if (s.find(*i) == s.end())
    {
      return false;
    }
  }
  return true;
}

/// \brief Makes a set of the given container.
/// \param c A container
/// \return A set containing the elements of the container
template <class Container>
std::set<typename Container::value_type> make_set(const Container& c)
{
  std::set<typename Container::value_type> result;
  std::copy(c.begin(), c.end(), std::inserter(result, result.begin()));
  return result;
}

/// \brief Determines if the unordered sequences s1 and s2 have an empty intersection
/// \param s1 A sequence
/// \param s2 A sequence
/// \return True if the intersection of s1 and s2 is empty
template <typename Sequence>
bool sequence_empty_intersection(Sequence s1, Sequence s2)
{
  for (typename Sequence::const_iterator i = s1.begin(); i != s1.end(); ++i)
  {
    if (std::find(s2.begin(), s2.end(), *i) != s2.end())
    {
      return false;
    }
  }
  return true;
}

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_SEQUENCE_ALGORITHM_H
