// Author(s): Jan Friso Groote. Based on lps2pbes_utility.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lps2pres_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_LPS2PRES_UTILITY_H
#define MCRL2_PRES_DETAIL_LPS2PRES_UTILITY_H

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pres/pres.h"



namespace mcrl2::pres_system {

/// \cond INTERNAL_DOCS
//
/// \brief Concatenates two sequences of PRES equations
/// \param p A sequence of PRES equations
/// \param q A sequence of PRES equations
/// \return The concatenation result
inline
std::vector<pres_equation> operator+(const std::vector<pres_equation>& p, const std::vector<pres_equation>& q)
{
  std::vector<pres_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}
/// \endcond

/// \cond INTERNAL_DOCS
//
/// \brief Appends a PRES equation to a sequence of PRES equations
/// \param p A sequence of PRES equations
/// \param e A PRES equation
/// \return The append result
inline
std::vector<pres_equation> operator+(const std::vector<pres_equation>& p, const pres_equation& e)
{
  std::vector<pres_equation> result(p);
  result.push_back(e);
  return result;
}
/// \endcond

namespace detail {

inline
data::variable_list lhs_variables(const data::assignment_list& l)
{
  data::variable_list result;
  for (const data::assignment& a: l)
  {
    result.push_front(a.lhs());
  }
  return atermpp::reverse(result);
}

inline
data::data_expression_list rhs_expressions(const data::assignment_list& l)
{
  data::data_expression_list result;
  for (const data::assignment& a: l)
  {
    result.push_front(a.rhs());
  }
  return atermpp::reverse(result);
}

inline
const core::identifier_string& mu_name(const state_formulas::state_formula& f)
{
  if (state_formulas::is_mu(f))
  {
    const auto& g = atermpp::down_cast<state_formulas::mu>(f);
    return g.name();
  }
  else
  {
    const auto& g = atermpp::down_cast<state_formulas::nu>(f);
    return g.name();
  }
}

/// \brief Returns the variables corresponding to ass(f)
/// \param f A modal formula
/// \return The variables corresponding to ass(f)
inline
data::variable_list mu_variables(const state_formulas::state_formula& f)
{
  assert(state_formulas::is_mu(f) || state_formulas::is_nu(f));
  if (state_formulas::is_mu(f))
  {
    const auto& g = atermpp::down_cast<state_formulas::mu>(f);
    return lhs_variables(g.assignments());
  }
  else
  {
    const auto& g = atermpp::down_cast<state_formulas::nu>(f);
    return lhs_variables(g.assignments());
  }
}

/// \brief Returns the data expressions corresponding to ass(f)
/// \param f A modal formula
/// \return The data expressions corresponding to ass(f)
inline
data::data_expression_list mu_expressions(const state_formulas::state_formula& f)
{
  assert(state_formulas::is_mu(f) || state_formulas::is_nu(f));
  if (state_formulas::is_mu(f))
  {
    const auto& g = atermpp::down_cast<state_formulas::mu>(f);
    return rhs_expressions(g.assignments());
  }
  else
  {
    const auto& g = atermpp::down_cast<state_formulas::nu>(f);
    return rhs_expressions(g.assignments());
  }
}

inline
std::string myprint(const std::vector<pres_equation>& v)
{
  std::ostringstream out;
  out << "[";
  for (const pres_equation& eqn: v)
  {
    out << "\n  " << pbes_system::pp(eqn.symbol()) << " " << pbes_system::pp(eqn.variable()) << " = " << pres_system::pp(eqn.formula());
  }
  out << "\n]";
  return out.str();
}

/// \brief Generates a substitution that assigns fresh variables to the given sequence of variables.
/// The identifier generator is used to assign names to the fresh variables.
/// Caveat: the implementation is very inefficient.
/// \param update_context If true, then generated names are added to the context
inline
data::mutable_map_substitution<> make_fresh_variable_substitution(const data::variable_list& variables, data::set_identifier_generator& id_generator, bool add_to_context = true)
{
  data::mutable_map_substitution<> result;
  for (const data::variable& v: variables)
  {
    core::identifier_string name =  id_generator(std::string(v.name()));
    result[v] = data::variable(name, v.sort());
    if (!add_to_context)
    {
      id_generator.remove_identifier(name);
    }
  }
  return result;
}

} // namespace detail

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_DETAIL_LPS2PRES_UTILITY_H
