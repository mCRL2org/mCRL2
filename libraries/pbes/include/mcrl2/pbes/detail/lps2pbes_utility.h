// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H
#define MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
//
/// \brief Concatenates two sequences of PBES equations
/// \param p A sequence of PBES equations
/// \param q A sequence of PBES equations
/// \return The concatenation result
inline
std::vector<pbes_equation> operator+(const std::vector<pbes_equation>& p, const std::vector<pbes_equation>& q)
{
  std::vector<pbes_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}
/// \endcond

/// \cond INTERNAL_DOCS
//
/// \brief Appends a PBES equation to a sequence of PBES equations
/// \param p A sequence of PBES equations
/// \param e A PBES equation
/// \return The append result
inline
std::vector<pbes_equation> operator+(const std::vector<pbes_equation>& p, const pbes_equation& e)
{
  std::vector<pbes_equation> result(p);
  result.push_back(e);
  return result;
}
/// \endcond

namespace detail {

inline
data::variable_list lhs_variables(const data::assignment_list& l)
{
  data::variable_list result;
  for (auto i = l.begin(); i != l.end(); ++i)
  {
    result.push_front(i->lhs());
  }
  return atermpp::reverse(result);
}

inline
data::data_expression_list rhs_expressions(const data::assignment_list& l)
{
  data::data_expression_list result;
  for (auto i = l.begin(); i != l.end(); ++i)
  {
    result.push_front(i->rhs());
  }
  return atermpp::reverse(result);
}

inline
const core::identifier_string& mu_name(const state_formulas::state_formula& f)
{
  if (state_formulas::is_mu(f))
  {
    const state_formulas::mu& g = atermpp::down_cast<state_formulas::mu>(f);
    return g.name();
  }
  else
  {
    const state_formulas::nu& g = atermpp::down_cast<state_formulas::nu>(f);
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
    const state_formulas::mu& g = atermpp::down_cast<state_formulas::mu>(f);
    return lhs_variables(g.assignments());
  }
  else
  {
    const state_formulas::nu& g = atermpp::down_cast<state_formulas::nu>(f);
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
    const state_formulas::mu& g = atermpp::down_cast<state_formulas::mu>(f);
    return rhs_expressions(g.assignments());
  }
  else
  {
    const state_formulas::nu& g = atermpp::down_cast<state_formulas::nu>(f);
    return rhs_expressions(g.assignments());
  }
}

inline
std::string myprint(const std::vector<pbes_equation>& v)
{
  std::ostringstream out;
  out << "[";
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    out << "\n  " << pbes_system::pp(i->symbol()) << " " << pbes_system::pp(i->variable()) << " = " << pbes_system::pp(i->formula());
  }
  out << "\n]";
  return out.str();
}

/// \brief Generates a substitution that assigns fresh variables to the given sequence of variables.
/// The identifier generator is used to assign names to the fresh variables.
/// Caveat: the implementation is very inefficient.
/// \param update_context If true, then generated names are added to the context
inline
data::mutable_map_substitution<> make_fresh_variables(const data::variable_list& variables, data::set_identifier_generator& id_generator, bool add_to_context = true)
{
  data::mutable_map_substitution<> result;
  for (auto i = variables.begin(); i != variables.end(); ++i)
  {
    core::identifier_string name =  id_generator(std::string(i->name()));
    result[*i] = data::variable(name, i->sort());
    if (!add_to_context)
    {
      id_generator.remove_identifier(name);
    }
  }
  return result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H
