// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/remove_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REMOVE_EQUATIONS_H
#define MCRL2_PROCESS_REMOVE_EQUATIONS_H

#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

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
  typedef std::vector<process_equation>::const_iterator iterator;
  typedef std::set<iterator> group;
  typedef data::mutable_map_substitution<std::map<process_identifier, process_identifier> > substitution;

  process_specification& procspec;
  data::set_identifier_generator generator;
  std::vector<group> groups;

  duplicate_equation_removal(process_specification& procspec_)
    : procspec(procspec_)
  {
    std::map<data::variable_list, group> s;
    for (auto i = procspec.equations().begin(); i != procspec.equations().end(); ++i)
    {
      generator.add_identifier(i->identifier().name());
      s[i->formal_parameters()].insert(i);
    }
    for (auto i = s.begin(); i != s.end(); ++i)
    {
      groups.push_back(i->second);
    }
  }

  data::sort_expression_list get_sorts(const data::variable_list& v)
  {
    data::sort_expression_vector s;
    for (auto i = v.begin(); i != v.end(); ++i)
    {
      s.push_back(i->sort());
    }
    return data::sort_expression_list(s.begin(), s.end());
  }

  // assigns a unique process identifier to each process identifier within a group
  substitution make_substitution()
  {
    substitution result;
    for (auto i = groups.begin(); i != groups.end(); ++i)
    {
      const group& g = *i;
      const process_equation& first_equation = **g.begin();
      process_identifier id(generator("X"), get_sorts(first_equation.formal_parameters()));
      for (auto j = i->begin(); j != i->end(); ++j)
      {
        const process_equation& eq = **j;
        result[eq.identifier()] = id;
      }
    }
    return result;
  }

  // splits the group g into multiple groups, and appends them to result
  void split_group(const group& g, const substitution& sigma, std::vector<group>& result)
  {
    std::map<process_expression, group> m;
    for (auto i = g.begin(); i != g.end(); ++i)
    {
      const process_equation& eq = **i;
      process_expression expr = replace_process_identifiers(eq.expression(), sigma);
      m[expr].insert(*i);
    }
    for (auto i = m.begin(); i != m.end(); ++i)
    {
      result.push_back(i->second);
    }
  }

  // splits all groups
  // returns true if at least one of the groups was split into multiple groups
  bool split_groups()
  {
    substitution sigma = make_substitution();
    std::vector<group> new_groups;
    for (auto i = groups.begin(); i != groups.end(); ++i)
    {
      split_group(*i, sigma, new_groups);
    }
    bool result = new_groups.size() > groups.size();
    groups = new_groups;
    return result;
  }

  void run()
  {
    // Compute groups of equal equations
    for (;;)
    {
      if (!split_groups())
      {
        break;
      }
    }

    // Choose one equation per group (the one with the lowest index in equations),
    // and prepare a substitution sigma that removes the others
    std::vector<process_equation> new_equations;
    substitution sigma;
    for (auto i = groups.begin(); i != groups.end(); ++i)
    {
      // optimization for the case of only one equation in a group
      if (i->size() == 1)
      {
        new_equations.push_back(**(i->begin()));
        continue;
      }

      group::const_iterator j = std::min_element(i->begin(), i->end());
      new_equations.push_back(**j);
      for (group::const_iterator k = i->begin(); k != i->end(); ++k)
      {
        if (k != j)
        {
          sigma[(**k).identifier()] = (**j).identifier();
        }
      }
    }

    // Apply sigma to the new equations and the initial state
    for (auto i = new_equations.begin(); i != new_equations.end(); ++i)
    {
      *i = process_equation(i->identifier(), i->formal_parameters(), replace_process_identifiers(i->expression(), sigma));
    }
    procspec.init() = replace_process_identifiers(procspec.init(), sigma);
    procspec.equations() = new_equations;
  }
};

} // namespace detail

inline
/// \brief Removes duplicate equations from a process specification
void remove_duplicate_equations(process_specification& procspec)
{
  detail::duplicate_equation_removal f(procspec);
  f.run();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_REMOVE_EQUATIONS_H
