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
#include <stdexcept>
#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace data {

/// \brief For several expressions in mCRL2 an implicit mapping of these expressions
/// to integers is available. This is done for efficiency reasons. Examples are:
///
/// data::variable, process::process_identifier
///
/// The class index_traits is used to implement this mapping. By this, the public
/// interface of the expression classes is not polluted with.
template <typename T>
struct index_traits
{
};

//--- very naive implementation of index mapping for data variables ---//
inline
std::map<data::variable, std::size_t>& data_variable_index_map()
{
  static std::map<data::variable, std::size_t> m;
  return m;
}

template <>
struct index_traits<data::variable>
{
  static std::map<data::variable, std::size_t> m;

  /// \brief Returns the index of the variable.
  static inline
  std::size_t index(const data::variable& x)
  {
    auto& m = data_variable_index_map();
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
    auto& m = data_variable_index_map();
    auto i = std::min_element(m.begin(), m.end(), m.value_comp());
    if (i == m.end())
    {
      throw std::runtime_error("error: empty map");
    }
    return i->second;
  }

  /// \brief Note: intended for internal use only!
  /// Returns the index of the variable. If the variable was not already in the map, it is added.
  static inline
  std::size_t insert(const data::variable& x)
  {
    auto& m = data_variable_index_map();
    auto i = m.find(x);
    if (i == m.end())
    {
      std::size_t value = m.size();
      m[x] = value;
      return value;
    }
    return i->second;
  }

  /// \brief Note: intended for internal use only!
  /// Removes the variable from the index map.
  static inline
  void erase(const data::variable& x)
  {
    auto& m = data_variable_index_map();
    auto i = m.find(x);
    if (i == m.end())
    {
      throw std::runtime_error("error: could not find element " + data::pp(x));
    }
    m.erase(i);
  }

  /// \brief Note: intended for internal use only!
  /// Removes the variable from the index map.
  static inline
  std::size_t size()
  {
    auto m = data_variable_index_map();
    return m.size();
  }

  /// \brief Note: intended for internal use only!
  /// Removes the variable from the index map.
  static inline
  void print(const std::string& msg)
  {
    std::cout << msg << std::endl;
    auto m = data_variable_index_map();
    for (auto i = m.begin(); i != m.end(); ++i)
    {
      std::cout << data::pp(i->first) << " -> " << i->second << std::endl;
    }
  }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INDEX_TRAITS_H
