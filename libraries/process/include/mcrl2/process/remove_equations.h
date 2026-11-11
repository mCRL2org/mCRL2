// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/remove_equations.h
/// \brief Removes duplicate process equations.

#ifndef MCRL2_PROCESS_REMOVE_EQUATIONS_H
#define MCRL2_PROCESS_REMOVE_EQUATIONS_H

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/replace.h"

namespace mcrl2::process
{

namespace detail
{

/// \brief The substitution on process identifiers given by a map; identifiers that are not a key are unchanged.
struct process_identifier_map_substitution
{
  using argument_type = process_identifier;
  using result_type = process_identifier;

  const std::map<process_identifier, process_identifier>& m;

  process_identifier operator()(const process_identifier& id) const
  {
    const std::map<process_identifier, process_identifier>::const_iterator i = m.find(id);
    return i == m.end() ? id : i->second;
  }
};

/// \brief Numbers 0, ..., n - 1 by key: equal keys get the same number, and the numbers are assigned in the order of
///        the first occurrence of their key.
/// \param key A function that maps an index to its key; the keys are only compared, so their order (which may
///        depend on the addresses of terms) does not matter.
/// \param numbers Receives the number of every index.
/// \return The number of distinct keys.
template<typename Key, typename KeyFunction>
std::size_t number_by_key(std::size_t n, KeyFunction key, std::vector<std::size_t>& numbers)
{
  std::map<Key, std::size_t> index;
  numbers.resize(n);
  for (std::size_t i = 0; i < n; ++i)
  {
    numbers[i] = index.try_emplace(key(i), index.size()).first->second;
  }
  return index.size();
}

/// \brief Maps the identifiers of the equations that are not kept to the identifier of the equation that is kept
///        for their class: the equation of initial_process if it is in the class, otherwise the first one.
/// \param classes For every equation the index of its class, which is less than the number of equations.
inline std::map<process_identifier, process_identifier> class_representatives(
  const std::vector<process_equation>& equations,
  const std::vector<std::size_t>& classes,
  const process_identifier& initial_process)
{
  const std::size_t n = equations.size();
  std::vector<std::size_t> kept(n, n);
  for (std::size_t i = 0; i < n; ++i)
  {
    std::size_t& k = kept[classes[i]];
    if (k == n || equations[i].identifier() == initial_process)
    {
      k = i;
    }
  }

  std::map<process_identifier, process_identifier> result;
  for (std::size_t i = 0; i < n; ++i)
  {
    const std::size_t k = kept[classes[i]];
    if (k != i)
    {
      result[equations[i].identifier()] = equations[k].identifier();
    }
  }
  return result;
}

/// \brief Partitions equations into classes of duplicate equations, using a bisimulation algorithm.
/// \details Two equations are duplicates if they have the same formal parameters, and their right hand sides are
///          equal after replacing every process identifier by (an identifier for) its class. This is the coarsest
///          such partition (a greatest fixpoint), computed by partition refinement: start with one class per list
///          of formal parameters, and split classes until replacing identifiers by their classes gives equal right
///          hand sides within every class. For example in the following list of equations
///
///          S(b: Bool) = sum d: D. r1(d) . T(d, b);
///          T(d: D, b: Bool) = s2(d, b) . (r6(b) . S(!b) + (r6(!b) + r6(e)) . T(d, b));
///          S2(b: Bool) = sum d: D. r1(d) . T1(d, b);
///          T1(d: D, b: Bool) = s2(d, b) . (r6(b) . S2(!b) + (r6(!b) + r6(e)) . T1(d, b));
///
///          S and S2 form a class, and so do T and T1.
/// \param equations Process equations with distinct identifiers.
/// \return For every equation the index of its class. The classes are numbered in the order of their first
///         equation in equations, so the result does not depend on the addresses of terms.
inline std::vector<std::size_t> duplicate_equation_classes(const std::vector<process_equation>& equations)
{
  const std::size_t n = equations.size();

  // The initial partition: one class per list of formal parameters.
  std::vector<std::size_t> classes;
  std::size_t class_count
    = number_by_key<data::variable_list>(n, [&](std::size_t i) { return equations[i].formal_parameters(); }, classes);

  for (;;)
  {
    // Split the classes on the right hand sides after replacing every identifier by the representative of its
    // class. Splitting never joins classes, so the partition is stable when the number of classes stays the same.
    const std::map<process_identifier, process_identifier> representatives
      = class_representatives(equations, classes, process_identifier());
    const process_identifier_map_substitution sigma{representatives};
    std::vector<std::size_t> new_classes;
    const std::size_t new_class_count = number_by_key<std::pair<std::size_t, process_expression>>(
      n,
      [&](std::size_t i)
      { return std::make_pair(classes[i], replace_process_identifiers(equations[i].expression(), sigma)); },
      new_classes);
    if (new_class_count == class_count)
    {
      return classes;
    }
    classes.swap(new_classes);
    class_count = new_class_count;
  }
}

} // namespace detail

/// \brief Removes duplicate equations, using a bisimulation algorithm.
/// \details Two equations are duplicates if they have the same formal parameters, and their right hand sides are
///          equal after identifying all duplicate equations (the greatest such relation), see
///          detail::duplicate_equation_classes. So also P = a . P and Q = a . Q are duplicates. Of every class of
///          duplicates one equation is kept: that of initial_process if it is in the class, otherwise the first one
///          in equations. The result does not depend on the addresses of terms.
/// \param equations Process equations with distinct identifiers. Afterwards it contains the kept equations, in
///        their original order, with the identifiers of the removed equations replaced by those of the kept
///        equations in the right hand sides.
/// \param initial_process A process identifier, whose equation (if any) is kept.
/// \return The identifiers of the removed equations, mapped to those of the kept equations.
inline std::map<process_identifier, process_identifier> remove_duplicate_equations(
  std::vector<process_equation>& equations,
  const process_identifier& initial_process = process_identifier())
{
  std::map<process_identifier, process_identifier> sigma
    = detail::class_representatives(equations, detail::duplicate_equation_classes(equations), initial_process);
  if (sigma.empty())
  {
    return sigma;
  }

  std::vector<process_equation> result;
  const detail::process_identifier_map_substitution substitution{sigma};
  for (const process_equation& equation: equations)
  {
    if (sigma.find(equation.identifier()) == sigma.end())
    {
      result.emplace_back(equation.identifier(),
        equation.formal_parameters(),
        replace_process_identifiers(equation.expression(), substitution));
    }
  }
  equations.swap(result);
  return sigma;
}

/// \brief Removes duplicate equations from a process specification, using a bisimulation algorithm.
/// \details See remove_duplicate_equations on a vector of equations. The first equation of every class is kept,
///          and the identifiers of the removed equations are also replaced in the initial process.
inline void remove_duplicate_equations(process_specification& procspec)
{
  const std::map<process_identifier, process_identifier> sigma = remove_duplicate_equations(procspec.equations());
  if (!sigma.empty())
  {
    procspec.init() = replace_process_identifiers(procspec.init(), detail::process_identifier_map_substitution{sigma});
  }
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REMOVE_EQUATIONS_H
