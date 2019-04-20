// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef MCRL2_UTILITIES_UNORDERED_MAP_IMPLEMENTATION_H
#define MCRL2_UTILITIES_UNORDERED_MAP_IMPLEMENTATION_H
#pragma once

#include "mcrl2/utilities/unordered_map.h"

#define MCRL2_UNORDERED_MAP_TEMPLATES template<typename Key, typename T, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
#define MCRL2_UNORDERED_MAP_CLASS unordered_map<Key, T, Hash, Equals, Allocator, ThreadSafe>

namespace mcrl2
{
namespace utilities
{

MCRL2_UNORDERED_MAP_TEMPLATES
int MCRL2_UNORDERED_MAP_CLASS::count(const Key& key)
{
  return m_set.find(key) != m_set.end();
}

MCRL2_UNORDERED_MAP_TEMPLATES
T& MCRL2_UNORDERED_MAP_CLASS::operator[](const Key& key)
{
  // Insert a new object and return a reference to it;
  auto pair = m_set.emplace(std::make_pair(key, T()));
  return (*pair.first).second;
}

MCRL2_UNORDERED_MAP_TEMPLATES
const T& MCRL2_UNORDERED_MAP_CLASS::at(const Key& key) const
{
  auto it = m_set.find(key);
  assert(it != m_set.end());
  return (*it).second;
}

#undef MCRL2_UNORDERED_MAP_TEMPLATES
#undef MCRL2_UNORDERED_MAP_CLASS

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNORDERED_MAP_IMPLEMENTATION_H
