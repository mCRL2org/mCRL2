// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/substitutions/map_substitution.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SUBSTITUTIONS_MAP_SUBSTITUTION_H
#define MCRL2_DATA_SUBSTITUTIONS_MAP_SUBSTITUTION_H

#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/utilities/exception.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace mcrl2 {

namespace data {

/// \brief Generic substitution function. The substitution is stored as a mapping
/// of variables to expressions.
template <typename AssociativeContainer>
struct map_substitution : public std::unary_function<typename AssociativeContainer::key_type, typename AssociativeContainer::mapped_type>
{
  typedef typename AssociativeContainer::key_type variable_type;
  typedef typename AssociativeContainer::mapped_type expression_type;

  const AssociativeContainer& m_map;

  map_substitution(const AssociativeContainer& m)
    : m_map(m)
  { }

  const expression_type operator()(const variable_type& v) const
  {
    typename AssociativeContainer::const_iterator i = m_map.find(v);
    if (i == m_map.end())
    {
      return v;
    }
    else
    {
      return i->second;
    }
    // N.B. This does not work!
    // return i == m_map.end() ? v : i->second;
  }

  template <typename Expression>
  expression_type operator()(const Expression&) const
  {
    throw mcrl2::runtime_error("data::map_substitution::operator(const Expression&) is a deprecated interface!");
    return data::undefined_data_expression();
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << "[";
    for (typename AssociativeContainer::const_iterator i = m_map.begin(); i != m_map.end(); ++i)
    {
      out << (i == m_map.begin() ? "" : "; ") << i->first << ":" << i->first.sort() << " := " << i->second;
    }
    out << "]";
    return out.str();
  }
};

/// \brief Utility function for creating a map_substitution.
template <typename AssociativeContainer>
map_substitution<AssociativeContainer>
make_map_substitution(const AssociativeContainer& m)
{
  return map_substitution<AssociativeContainer>(m);
}

template <typename AssociativeContainer>
bool is_simple_substitution(const map_substitution<AssociativeContainer>& sigma)
{
  for (auto i = sigma.m_map.begin(); i != sigma.m_map.end(); ++i)
  {
    if (!is_simple_substitution(i->first, i->second))
    {
      return false;
    }
  }
  return true;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SUBSTITUTIONS_MAP_SUBSTITUTION_H
