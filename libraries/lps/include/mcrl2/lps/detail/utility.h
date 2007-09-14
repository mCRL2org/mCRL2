// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utility.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_UTILITY_H
#define MCRL2_LPS_DETAIL_UTILITY_H

#include <algorithm>
#include <iterator>
#include <set>

namespace lps {

namespace detail {

/// Returns true if the sequence [first, last[ contains duplicates.
// TODO: this implementation is not particularly efficient
template <typename Iterator>
bool contains_duplicates(Iterator first, Iterator last)
{
  std::set<typename std::iterator_traits<Iterator>::value_type> s(first, last);
  int ssize = s.size();
  return ssize < std::distance(first, last);
}

/// Returns true if all elements of the range [first, last[ are element of s.
template <typename Iterator, typename T>
bool is_subset_of(Iterator first, Iterator last, const std::set<T>& s)
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

/// Makes a set of the given container.
template <class Container>
std::set<typename Container::value_type> make_set(const Container& c)
{
  std::set<typename Container::value_type> result;
  std::copy(c.begin(), c.end(), std::inserter(result, result.begin()));
  return result;
}

template <typename T>
std::set<T> set_union(const std::set<T>& x, const std::set<T>& y)
{
  std::set<T> result;
  std::set_union(x.begin(), x.end(), y.begin(), y.end(), std::inserter(result, result.begin()));
  return result;
}

template <typename T>
std::set<T> set_difference(const std::set<T>& x, const std::set<T>& y)
{
  std::set<T> result;
  std::set_difference(x.begin(), x.end(), y.begin(), y.end(), std::inserter(result, result.begin()));
  return result;
}

} // namespace detail

} // namespace lps

#endif // MCRL2_LPS_DETAIL_UTILITY_H
