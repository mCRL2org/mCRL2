// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/remove_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REMOVE_EQUATIONS_H
#define MCRL2_PROCESS_REMOVE_EQUATIONS_H

#include <algorithm>

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/replace.h"

namespace mcrl2::process
{

namespace detail {

// Uses a bisimulation algorithm to find duplicate equations. For example in the following
// list of equations
//
// S(b: Bool) = sum d: D. r1(d) . T(d, b);
// T(d: D, b: Bool) = s2(d, b) . (r6(b) . S(!b) + (r6(!b) + r6(e)) . T(d, b));
// R(b: Bool) = sum d: D. r3(d, b) . s4(d) . s5(b) . R(!b) + (sum d: D. r3(d, !b) + r3(e)) . s5(!b) . R(b);
// K = sum d: D, b: Bool. r2(d, b) . (i . s3(d, b) + i . s3(e)) . K;
// L = sum b: Bool. r5(b) . (i . s6(b) + i . s6(e)) . L;
// S2(b: Bool) = sum d: D. r1(d) . T1(d, b);
// T1(d: D, b: Bool) = s2(d, b) . (r6(b) . S2(!b) + (r6(!b) + r6(e)) . T1(d, b));
//
// the equations for S2 and T1 will be removed
struct duplicate_equation_removal
{
  using iterator = std::vector<process_equation>::const_iterator;
  using group = std::set<iterator>;
  using substitution = data::mutable_map_substitution<std::map<process_identifier, process_identifier>>;

  const std::vector<process_equation>& equations;
  data::set_identifier_generator generator;
  std::vector<group> groups;

  std::string print_group(const group& g)
  {
    std::ostringstream out;
    for (const auto& i: g)
    {
      const process_equation& eq = *i;

      // use a custom print for equations, since by default they are printed on multiple lines...
      bool print_parens = !eq.formal_parameters().empty();
      out << eq.identifier().name();
      if (print_parens)
      {
        out << "(";
      }
      out << data::pp(eq.formal_parameters());
      if (print_parens)
      {
        out << ")";
      }
      out << " = " << eq.expression() << ";\n";
    }
    return out.str();
  }

  std::string print_groups()
  {
    std::ostringstream out;
    std::size_t index = 0;
    for (auto& group: groups)
    {
      out << "--- group " << index++ << " ---\n" << print_group(group) << std::endl;
    }
    return out.str();
  }

  // identifiers contains the identifiers that the generated process identifiers must differ from
  duplicate_equation_removal(const std::vector<process_equation>& equations_,
    const std::set<core::identifier_string>& identifiers)
    : equations(equations_)
  {
    generator.add_identifiers(identifiers);
    std::map<data::variable_list, group> s;
    for (auto i = equations.begin(); i != equations.end(); ++i)
    {
      s[i->formal_parameters()].insert(i);
    }
    for (auto& i: s)
    {
      groups.push_back(i.second);
    }
    mCRL2log(log::log_level_t::debug) << "==========================================================\n" << print_groups() << std::endl;
  }

  // assigns a unique process identifier to each process identifier within a group
  substitution make_substitution()
  {
    substitution result;
    for (const group& g: groups)
    {
      const process_equation& first_equation = **g.begin();
      process_identifier id(generator("X"), first_equation.formal_parameters());
      for (const iterator& i: g)
      {
        const process_equation& eq = *i;
        result[eq.identifier()] = id;
      }
    }
    return result;
  }

  // splits the group g into multiple groups, and appends them to result
  void split_group(const group& g, const substitution& sigma, std::vector<group>& result)
  {
    std::map<process_expression, group> m;
    for (const auto& i: g)
    {
      const process_equation& eq = *i;
      process_expression expr = replace_process_identifiers(eq.expression(), sigma);
      m[expr].insert(i);
    }
    for (auto &i : m)
    {
      result.push_back(i.second);
    }
  }

  // splits all groups
  // returns true if at least one of the groups was split into multiple groups
  bool split_groups()
  {
    substitution sigma = make_substitution();
    std::vector<group> new_groups;
    for (auto& group: groups)
    {
      split_group(group, sigma, new_groups);
    }
    bool result = new_groups.size() > groups.size();
    groups = new_groups;
    return result;
  }

  // Returns the remaining equations, in the order of equations, and puts the identifiers of the removed equations
  // in sigma, mapped to those of the remaining ones. Of every group the equation of initial_process is kept if it
  // is in the group, otherwise the one with the lowest index in equations.
  std::vector<process_equation> run(const process_identifier& initial_process, substitution& sigma)
  {
    // Compute groups of equal equations
    for (;;)
    {
      if (!split_groups())
      {
        break;
      }
      mCRL2log(log::log_level_t::debug) << "==========================================================\n" << print_groups() << std::endl;
    }

    // Choose one equation per group, and prepare a substitution sigma that removes the others
    std::vector<iterator> kept;
    for (auto& i : groups)
    {
      // optimization for the case of only one equation in a group
      if (i.size() == 1)
      {
        kept.push_back(*i.begin());
        continue;
      }

      group::const_iterator j
        = std::find_if(i.begin(), i.end(), [&](const iterator& k) { return k->identifier() == initial_process; });
      if (j == i.end())
      {
        j = std::min_element(i.begin(), i.end());
      }
      kept.push_back(*j);
      for (group::const_iterator k = i.begin(); k != i.end(); ++k)
      {
        if (k != j)
        {
          sigma[(**k).identifier()] = (**j).identifier();
        }
      }
    }

    // The order of the groups depends on the addresses of terms, so keep the order of equations
    std::sort(kept.begin(), kept.end());

    // Apply sigma to the remaining equations
    std::vector<process_equation> result;
    for (const iterator& k: kept)
    {
      result.emplace_back(k->identifier(), k->formal_parameters(), replace_process_identifiers(k->expression(), sigma));
    }
    return result;
  }
};

} // namespace detail

/// \brief Removes duplicate equations, using a bisimulation algorithm
/// \param equations Process equations with distinct identifiers
/// \param initial_process A process identifier; its equation (if any) is kept
/// \return The remaining equations, in the order of equations, with the identifiers of the removed equations
///         replaced by those of the remaining ones
inline std::vector<process_equation> remove_duplicate_equations(const std::vector<process_equation>& equations,
  const process_identifier& initial_process)
{
  detail::duplicate_equation_removal f(equations, process::find_identifiers(equations));
  detail::duplicate_equation_removal::substitution sigma;
  return f.run(initial_process, sigma);
}

inline
/// \brief Removes duplicate equations from a process specification, using a bisimulation algorithm
void remove_duplicate_equations(process_specification& procspec)
{
  std::set<core::identifier_string> identifiers = process::find_identifiers(procspec);
  const std::set<core::identifier_string> data_identifiers = data::function_and_mapping_identifiers(procspec.data());
  identifiers.insert(data_identifiers.begin(), data_identifiers.end());
  detail::duplicate_equation_removal f(procspec.equations(), identifiers);
  detail::duplicate_equation_removal::substitution sigma;
  std::vector<process_equation> new_equations = f.run(process_identifier(), sigma);
  procspec.init() = replace_process_identifiers(procspec.init(), sigma);
  procspec.equations() = new_equations;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REMOVE_EQUATIONS_H
