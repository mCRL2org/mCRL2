// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/eliminate_trivial_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H
#define MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H

#include <algorithm>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

namespace detail {

/// \brief Convert data::variable_list to data::expression_list
inline
data::data_expression_list make_data_expression_list(const data::variable_list& x)
{
  return data::data_expression_list(x.begin(), x.end());
}

/// \brief Convert a process instance assignment to a process instance
inline
process_instance make_process_instance(const process_instance_assignment& x)
{
  std::vector<data::data_expression> actual_parameters;
  const data::assignment_list& assignments = x.assignments();
  for (const data::variable& v: x.identifier().variables())
  {
    auto i = std::find_if(assignments.begin(), assignments.end(), [&v](const data::assignment& a) { return a.lhs() == v; });
    if (i == assignments.end())
    {
      actual_parameters.push_back(v);
    }
    else
    {
      actual_parameters.push_back(i->rhs());
    }
  }
  return process_instance(x.identifier(), data::data_expression_list(actual_parameters.begin(), actual_parameters.end()));
}

/// Precondition: x is a process instance or a process instance assignment
inline
process_instance make_process_instance(const process_expression& x)
{
  if (is_process_instance_assignment(x))
  {
    return make_process_instance(atermpp::down_cast<process_instance_assignment>(x));
  }
  else
  {
    return atermpp::down_cast<process_instance>(x);
  }
}

/// \brief Given P(2, 3) it returns the substitution [x := 2, y := 3] for process identifier P(x, y).
inline
data::mutable_map_substitution<> make_process_instance_substitution(const process_instance& x)
{
  data::mutable_map_substitution<> sigma;
  const data::variable_list& v = x.identifier().variables();
  const data::data_expression_list& e = x.actual_parameters();
  auto i = v.begin();
  auto j = e.begin();
  for (; i != v.end(); ++i, ++j)
  {
    if (*i != *j)
    {
      sigma[*i] = *j;
    }
  }
  return sigma;
}

/// \brief Applies sigma to the arguments of x
inline
process_instance apply_substitution(const process_instance& x, data::mutable_map_substitution<>& sigma)
{
  return process_instance(x.identifier(), data::replace_variables_capture_avoiding(x.actual_parameters(), sigma, data::substitution_variables(sigma)));
}

struct process_instance_replace_builder: public process_expression_builder<process_instance_replace_builder>
{
  typedef process_expression_builder<process_instance_replace_builder> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const std::map<process_identifier, process_instance>& substitutions;

  process_instance_replace_builder(const std::map<process_identifier, process_instance>& substitutions_)
    : substitutions(substitutions_)
  {}

  process_expression apply(const process::process_instance& x)
  {
    auto i = substitutions.find(x.identifier());
    if (i == substitutions.end())
    {
      return x;
    }
    else
    {
      data::mutable_map_substitution<> sigma = make_process_instance_substitution(x);
      return apply_substitution(i->second, sigma);
    }
  }

  process_expression apply(const process::process_instance_assignment& x)
  {
    return (*this).apply(make_process_instance(x));
  }
};

} // namespace detail

inline
process_expression process_instance_replace(const process_expression& x, const std::map<process_identifier, process_instance>& substitutions)
{
  detail::process_instance_replace_builder f(substitutions);
  return f.apply(x);
}

inline
void process_instance_replace(process_specification& procspec, const std::map<process_identifier, process_instance>& substitutions)
{
  std::vector<process_equation> equations;
  for (const process_equation& eqn: procspec.equations())
  {
    // remove trivial equations
    if (substitutions.find(eqn.identifier()) != substitutions.end())
    {
      continue;
    }
    equations.push_back(process_equation(eqn.identifier(), eqn.formal_parameters(), process_instance_replace(eqn.expression(), substitutions)));
  }
  procspec.init() = process_instance_replace(procspec.init(), substitutions);
  procspec.equations() = equations;
}

// Removes trivial equations of the form P1() = P2().
// Precondition: there are no loops P1() = P2() = ... = P1().
struct eliminate_trivial_equations_algorithm
{
  process_specification& procspec;
  std::map<process_identifier, process_identifier> edges;
  std::vector<std::vector<process_identifier> > chains;
  std::map<process_identifier, process_instance> substitutions;

  eliminate_trivial_equations_algorithm(process_specification& procspec_)
    : procspec(procspec_)
  {}

  bool is_trivial(const process_equation& eqn) const
  {
    return is_process_instance(eqn.expression()) || is_process_instance_assignment(eqn.expression());
  }

  const process_identifier& instance_identifier(const process_expression& x) const
  {
    if (is_process_instance(x))
    {
      return atermpp::down_cast<process_instance>(x).identifier();
    }
    else
    {
      return atermpp::down_cast<process_instance_assignment>(x).identifier();
    }
  }

  void print_chains() const
  {
    mCRL2log(log::verbose) << "--- chains ---" << std::endl;
    for (const std::vector<process_identifier>& chain: chains)
    {
      mCRL2log(log::verbose) << core::detail::print_list(chain) << std::endl;
    }
  }

  void print_substitutions() const
  {
    mCRL2log(log::verbose) << "--- substitutions ---" << std::endl;
    for (const auto& i: substitutions)
    {
      mCRL2log(log::verbose) << process_instance(i.first, detail::make_data_expression_list(i.first.variables())) << " -> " << i.second << std::endl;
    }
  }

  void compute_edges()
  {
    for (const process_equation& eqn: procspec.equations())
    {
      if (is_trivial(eqn))
      {
        const process_identifier& src = eqn.identifier();
        const process_identifier& dest = instance_identifier(eqn.expression());
        edges[src] = dest;
        substitutions[src] = detail::make_process_instance(eqn.expression());
      }
    }
  }

  void compute_chains()
  {
    // compute sources of chains
    std::set<process_identifier> sources;
    for (const auto& p: edges)
    {
      sources.insert(p.first);
    }
    for (const auto& p: edges)
    {
      sources.erase(p.second);
    }


    // compute chains
    for (const process_identifier& source: sources)
    {
      std::vector<process_identifier> chain;
      chain.push_back(source);
      auto i = edges.find(source);
      while (i != edges.end())
      {
        const process_identifier& target = i->second;
        chain.push_back(target);
        i = edges.find(target);
      }
      chains.push_back(chain);
    }
  }

  void compute_substitutions()
  {
    for (const std::vector<process_identifier>& chain: chains)
    {
      auto last_end = --chain.end();
      for (auto last = ++chain.begin(); last != last_end; ++last)
      {
        const process_instance& Plast = substitutions[*last];
        for (auto first = chain.begin(); first != last; ++first)
        {
          process_instance& Pfirst = substitutions[*first];
          auto sigma = detail::make_process_instance_substitution(Pfirst);
          Pfirst = detail::apply_substitution(Plast, sigma);
        }
      }
    }
  }

  void run()
  {
    compute_edges();
    compute_chains();
    print_chains();
    compute_substitutions();
    print_substitutions();
    process_instance_replace(procspec, substitutions);
  }
};

/// \brief Eliminates trivial equations, that have a process instance as the right hand side.
void eliminate_trivial_equations(process_specification& procspec)
{
  eliminate_trivial_equations_algorithm algorithm(procspec);
  algorithm.run();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H
