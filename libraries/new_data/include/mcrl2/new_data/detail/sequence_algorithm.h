// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/sequence_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_NEW_DATA_DETAIL_SEQUENCE_ALGORITHM_H
#define MCRL2_NEW_DATA_DETAIL_SEQUENCE_ALGORITHM_H

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

namespace mcrl2 {

namespace new_data {

namespace detail {

/// \brief Returns true if the sequence [first, last) contains duplicates.
/// \param first Start of a sequence
/// \param last End of a sequence
/// \return True if the sequence [first, last) contains duplicates.
template <typename Iterator>
bool sequence_contains_duplicates(Iterator first, Iterator last)
{
  // TODO: this implementation is not particularly efficient
  std::set<typename std::iterator_traits<Iterator>::value_type> s(first, last);
  int ssize = s.size();
  return ssize < std::distance(first, last);
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
  typedef typename std::iterator_traits<Iterator1>::value_type value_type;
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

} // namespace detail

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_SEQUENCE_ALGORITHM_H
