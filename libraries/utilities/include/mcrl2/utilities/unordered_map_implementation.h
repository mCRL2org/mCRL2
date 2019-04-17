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
  auto it = m_set.find(key);
  if (it != m_set.end())
  {
    // Return a reference to the existing element.
    return (*it).value;
  }
  else
  {
    // Insert a new object and return a reference to it;
    auto& pair = m_set.emplace(key);
    return pair.value;
  }
}

MCRL2_UNORDERED_MAP_TEMPLATES
const T& MCRL2_UNORDERED_MAP_CLASS::at(const Key& key) const
{
  auto it = m_set.find(key);
  assert(it != m_set.end());
  return (*it).value;
}

#undef MCRL2_UNORDERED_MAP_TEMPLATES
#undef MCRL2_UNORDERED_MAP_CLASS

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNORDERED_MAP_IMPLEMENTATION_H
