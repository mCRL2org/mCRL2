// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/index_traits.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_INDEX_TRAITS_H
#define MCRL2_DATA_INDEX_TRAITS_H

#include <iostream>
#include <map>
#include <stack>
#include <stdexcept>

#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace data {

// prototype declaration
template <typename T> std::string pp(const T&);

//--- very naive implementation of index mapping for variables ---//
template <typename Variable>
std::map<Variable, std::size_t>& variable_index_map()
{
  static std::map<Variable, std::size_t> m;
  return m;
}

template <typename Variable>
std::stack<std::size_t>& variable_map_free_numbers()
{
  static std::stack<std::size_t> s;
  return s;
}

/// \brief For several expressions in mCRL2 an implicit mapping of these expressions
/// to integers is available. This is done for efficiency reasons. Examples are:
///
/// Variable, process::process_identifier
///
/// The class index_traits is used to implement this mapping. By this, the public
/// interface of the expression classes is not polluted with.
template <class Variable>
struct index_traits
{
  /// \brief Returns the index of the variable.
  static inline
  std::size_t index(const Variable& x)
  {
    auto& m = variable_index_map<Variable>();
    auto i = m.find(x);
    if (i == m.end())
    {
      throw std::runtime_error("error: could not find element " + data::pp(x));
    }
    return i->second;
  }

  /// \brief Returns the largest index of a variable that is currently in use.
  static inline
  std::size_t max_index()
  {
    auto& m = variable_index_map<Variable>();
    if (m.begin() == m.end())
    {
      throw std::runtime_error("error: empty map");
    }
    size_t max=0;
    for (auto j=m.begin(); j!=m.end(); ++j)
    {
      max=std::max(max,j->second);
    }
    return max;
  }

  /// \brief Note: intended for internal use only!
  /// Returns the index of the variable. If the variable was not already in the map, it is added.
  static inline
  std::size_t insert(const Variable& x)
  {
    mCRL2log(log::debug) << "insert " << x << std::endl;
    auto& m = variable_index_map<Variable>();
    auto i = m.find(x);
    if (i == m.end())
    {
      auto& s = variable_map_free_numbers<Variable>();
      std::size_t value;
      if (s.empty())
      {
        value = m.size();
      }
      else
      {
        value = s.top();
        s.pop();
      }
      m[x] = value;
      return value;
    }
    return i->second;
  }

  /// \brief Note: intended for internal use only!
  /// Removes the variable from the index map.
  static inline
  void erase(const Variable& x)
  {
    mCRL2log(log::debug) << "erase " << x << std::endl;
    auto& m = variable_index_map<Variable>();
    auto& s = variable_map_free_numbers<Variable>();
    auto i = m.find(x);
    assert(i != m.end());
    s.push(i->second);
    m.erase(i);
  }

  /// \brief Note: intended for internal use only!
  /// Provides the number of elements in the index. Note that there might be elements
  /// with higher values in the index map.
  static inline
  std::size_t size()
  {
    auto& m = variable_index_map<Variable>();
    return m.size();
  }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INDEX_TRAITS_H
