// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/merge_identical_equations.h
/// \brief Merges process equations with the same formal parameters and right hand side, as in the lineariser.

#ifndef MCRL2_PROCESS_MERGE_IDENTICAL_EQUATIONS_H
#define MCRL2_PROCESS_MERGE_IDENTICAL_EQUATIONS_H

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "mcrl2/process/process_equation.h"
#include "mcrl2/process/replace.h"

namespace mcrl2::process
{

namespace detail
{

/// \brief A map from process identifiers to the process identifiers they are identified with. It represents the
///        equivalence classes of the identified process identifiers: following the map from an identifier leads to
///        the representative of its class, which does not occur as a key.
using identifier_map = std::map<process_identifier, process_identifier>;

/// \brief The representative of id in m.
inline process_identifier representative(const process_identifier& id, const identifier_map& m)
{
  process_identifier result = id;
  for (identifier_map::const_iterator i = m.find(result); i != m.end(); i = m.find(result))
  {
    result = i->second;
  }
  return result;
}

/// \brief Identifies id1 and id2 in m. The representative of the class of id1 is mapped to that of id2, unless it
///        is initial_process, which must remain a representative.
inline void identify(identifier_map& m,
  const process_identifier& id1,
  const process_identifier& id2,
  const process_identifier& initial_process)
{
  assert(id1 != id2);
  process_identifier representative1 = representative(id1, m);
  process_identifier representative2 = representative(id2, m);
  if (representative1 == initial_process)
  {
    representative1.swap(representative2);
  }
  if (representative1 != representative2)
  {
    m[representative1] = representative2;
  }
}

/// \brief Maps every key of m directly to its representative.
inline identifier_map flatten(const identifier_map& m)
{
  identifier_map result;
  for (const std::pair<const process_identifier, process_identifier>& p: m)
  {
    result[p.first] = representative(p.second, m);
  }
#ifndef NDEBUG
  for (const std::pair<const process_identifier, process_identifier>& p: result)
  {
    assert(result.find(p.second) == result.end());
  }
#endif
  return result;
}

/// \brief The substitution on process identifiers that maps the keys of a flattened identifier map to their
///        representatives.
struct identifier_map_substitution
{
  using argument_type = process_identifier;
  using result_type = process_identifier;

  const identifier_map& m;

  process_identifier operator()(const process_identifier& id) const
  {
    const identifier_map::const_iterator i = m.find(id);
    return i == m.end() ? id : i->second;
  }
};

} // namespace detail

/// \brief Merges the equations with the same formal parameters and right hand side.
/// \details Of equations with the same formal parameters and right hand side only the first in equations is kept,
///          unless a later one is the equation of initial_process, which is always kept. Then the merged process
///          identifiers are replaced in the right hand sides by the identifiers of the kept equations, which can
///          make more right hand sides equal; this is repeated until no more equations are merged. Example:
///          P1 = a.P2, P3 = a.P4, P2 = b.Q, P4 = b.Q reduces to P1 = a.P2, P2 = b.Q. Equations that only become
///          equal under the assumption that they are merged, such as P = a.P and Q = a.Q, are not merged.
/// \param equations Process equations with distinct identifiers.
/// \param initial_process The identifier of one of the equations.
/// \return The remaining equations, in the order of equations, with the merged process identifiers replaced in
///         their right hand sides. The equation of initial_process is among them.
inline std::vector<process_equation> merge_identical_equations(const std::vector<process_equation>& equations,
  const process_identifier& initial_process)
{
  using key_type = std::pair<data::variable_list, process_expression>;

  // The keys of mapping in the order of insertion, which is the order in which mapping is traversed; the order of
  // the map itself depends on the addresses of the terms.
  std::map<key_type, process_identifier> mapping;
  std::vector<key_type> keys;
  detail::identifier_map identified;
  for (const process_equation& equation: equations)
  {
    const key_type key(equation.formal_parameters(), equation.expression());
    const std::map<key_type, process_identifier>::const_iterator i = mapping.find(key);
    if (i == mapping.end())
    {
      mapping[key] = equation.identifier();
      keys.push_back(key);
    }
    else
    {
      detail::identify(identified, equation.identifier(), i->second, initial_process);
    }
  }
  identified = detail::flatten(identified);

  while (!identified.empty())
  {
    // Replace the identified process identifiers in the right hand sides, which may make more equations equal.
    const detail::identifier_map_substitution sigma{identified};
    std::map<key_type, process_identifier> new_mapping;
    std::vector<key_type> new_keys;
    detail::identifier_map new_identified;
    for (const key_type& key: keys)
    {
      const process_identifier& id = mapping.at(key);
      const key_type new_key(key.first, replace_process_identifiers(key.second, sigma));
      const std::map<key_type, process_identifier>::const_iterator i = new_mapping.find(new_key);
      if (i == new_mapping.end())
      {
        new_mapping[new_key] = sigma(id);
        new_keys.push_back(new_key);
      }
      else
      {
        detail::identify(new_identified, sigma(id), sigma(i->second), initial_process);
      }
    }
    identified = detail::flatten(new_identified);
    mapping.swap(new_mapping);
    keys.swap(new_keys);
  }

  std::vector<process_equation> result;
  for (const key_type& key: keys)
  {
    result.emplace_back(mapping.at(key), key.first, key.second);
  }
  assert(std::any_of(result.begin(),
    result.end(),
    [&](const process_equation& equation) { return equation.identifier() == initial_process; }));
  return result;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_MERGE_IDENTICAL_EQUATIONS_H
