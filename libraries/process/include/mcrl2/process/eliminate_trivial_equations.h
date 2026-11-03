// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/eliminate_trivial_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H
#define MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H

#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/print.h"

namespace mcrl2::process
{

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
  return process_instance(x.identifier(), data::replace_variables_capture_avoiding(x.actual_parameters(), sigma));
}

struct process_instance_replace_builder: public process_expression_builder<process_instance_replace_builder>
{
  using super = process_expression_builder<process_instance_replace_builder>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const std::map<process_identifier, process_instance>& substitutions;

  explicit process_instance_replace_builder(const std::map<process_identifier, process_instance>& substitutions_)
    : substitutions(substitutions_)
  {}

  template <class T>
  void apply(T& result, const process::process_instance& x)
  {
    auto i = substitutions.find(x.identifier());
    if (i == substitutions.end())
    {
      result = x;
      return;
    }
    else
    {
      data::mutable_map_substitution<> sigma = make_process_instance_substitution(x);
      result = apply_substitution(i->second, sigma);
      return;
    }
  }

  template <class T>
  void apply(T& result, const process::process_instance_assignment& x)
  {
    (*this).apply(result, make_process_instance(x));
  }
};

} // namespace detail

inline
process_expression process_instance_replace(const process_expression& x, const std::map<process_identifier, process_instance>& substitutions)
{
  detail::process_instance_replace_builder f(substitutions);
  process_expression result;
  f.apply(result, x);
  return result;
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
    equations.emplace_back(eqn.identifier(), eqn.formal_parameters(), process_instance_replace(eqn.expression(), substitutions));
  }
  procspec.init() = process_instance_replace(procspec.init(), substitutions);
  procspec.equations() = equations;
}

// Removes trivial equations of the form P1() = P2().
// Throws an exception if trivial equations form a loop P1() = P2() = ... = P1().
struct eliminate_trivial_equations_algorithm
{
  process_specification& procspec;
  std::map<process_identifier, process_identifier> edges;
  std::map<process_identifier, process_instance> substitutions;

  explicit eliminate_trivial_equations_algorithm(process_specification& procspec_)
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

  void print_substitutions() const
  {
    mCRL2log(log::log_level_t::verbose) << "--- substitutions ---" << std::endl;
    for (const auto& i: substitutions)
    {
      mCRL2log(log::log_level_t::verbose) << process_instance(i.first, detail::make_data_expression_list(i.first.variables())) << " -> " << i.second << std::endl;
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

  // Replaces the instance in the trivial equation of P by the instance at the end of its chain of trivial equations,
  // in terms of the parameters of P. The processes in visited are on the chain from which P is reached.
  void
  resolve(const process_identifier& P, std::set<process_identifier>& resolved, std::set<process_identifier>& visited)
  {
    if (resolved.find(P) != resolved.end())
    {
      return;
    }
    if (!visited.insert(P).second)
    {
      throw mcrl2::runtime_error("The trivial equations of " + process::pp(P) + " form a loop.");
    }
    const std::map<process_identifier, process_identifier>::const_iterator i = edges.find(P);
    if (substitutions.find(i->second) != substitutions.end())
    {
      resolve(i->second, resolved, visited);
      data::mutable_map_substitution<> sigma = detail::make_process_instance_substitution(substitutions[P]);
      substitutions[P] = detail::apply_substitution(substitutions[i->second], sigma);
    }
    resolved.insert(P);
  }

  void compute_substitutions()
  {
    std::set<process_identifier> resolved;
    for (const std::pair<const process_identifier, process_identifier>& edge: edges)
    {
      std::set<process_identifier> visited;
      resolve(edge.first, resolved, visited);
    }
  }

  void run()
  {
    compute_edges();
    compute_substitutions();
    print_substitutions();
    process_instance_replace(procspec, substitutions);
  }
};

/// \brief Eliminates trivial equations, that have a process instance as the right hand side.
inline
void eliminate_trivial_equations(process_specification& procspec)
{
  eliminate_trivial_equations_algorithm algorithm(procspec);
  algorithm.run();
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ELIMINATE_TRIVIAL_EQUATIONS_H
