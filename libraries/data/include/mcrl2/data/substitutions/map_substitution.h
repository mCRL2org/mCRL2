// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::data
{

/// \brief Generic substitution function. The substitution is stored as a mapping
/// of variables to expressions.
template <typename AssociativeContainer>
struct map_substitution
{
  using variable_type = typename AssociativeContainer::key_type;
  using expression_type = typename AssociativeContainer::mapped_type;
  using argument_type = variable_type;
  using result_type = expression_type;

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
std::set<data::variable> substitution_variables(const map_substitution<AssociativeContainer>& sigma)
{
  std::set<data::variable> result;
  for (const auto& [key, value]: sigma.m_map)
  {
    data::find_free_variables(value, std::inserter(result, result.end()));
  }
  return result;
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

} // namespace mcrl2::data

#endif // MCRL2_DATA_SUBSTITUTIONS_MAP_SUBSTITUTION_H
