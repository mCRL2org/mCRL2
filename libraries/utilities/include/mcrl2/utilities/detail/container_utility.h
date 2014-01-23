// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/container_utility.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_DETAIL_CONTAINER_UTILITY_H
#define MCRL2_UTILITIES_DETAIL_CONTAINER_UTILITY_H

#include <sstream>
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace utilities {

namespace detail {

/// \brief Returns the value corresponding to the given key in the map m. If the key is not
/// present, an exception is thrown.
template <typename Map>
typename Map::mapped_type map_element(const Map& m, const typename Map::key_type& key)
{
  auto i = m.find(key);
  if (i == m.end())
  {
    std::ostringstream out;
    out << "map_element: key " << key << " not found!";
    throw mcrl2::runtime_error(out.str());
  }
  return i->second;
}

/// \brief Returns the value corresponding to the given key in the map m. If the key is not
/// present, an exception is thrown.
template <typename Container>
bool contains(const Container& c, const typename Container::value_type& v)
{
  return std::find(c.begin(), c.end(), v) != c.end();
}

// specialization
template <typename T>
bool contains(const std::set<T>& c, const typename std::set<T>::value_type& v)
{
  return c.find(v) != c.end();
}

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_CONTAINER_UTILITY_H
