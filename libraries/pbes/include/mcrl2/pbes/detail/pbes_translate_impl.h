// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_translate_impl.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
#define MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <map>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Returns the free variables of a pbes expression
/// \param e A PBES expression
/// \return The free variables of a pbes expression
inline
std::set<data::variable> compute_free_pbes_expression_variables(const pbes_expression& e)
{
  free_variable_visitor<pbes_expression> visitor;
  visitor.visit(e);
  return visitor.result;
}

/// \brief Visitor that collects the names of propositional variables + instantiations
struct prop_var_visitor
{
  std::set<core::identifier_string>& m_identifiers;

  prop_var_visitor(std::set<core::identifier_string>& identifiers)
    : m_identifiers(identifiers)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  bool operator()(atermpp::aterm_appl t)
  {
    bool result = true;
    if (is_propositional_variable(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable(t).name());
    }
    if (is_propositional_variable_instantiation(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable_instantiation(t).name());
    }
    return result;
  }
};

/// \brief Returns the variables corresponding to ass(f)
/// \param f A modal formula
/// \return The variables corresponding to ass(f)
inline
data::variable_list mu_variables(state_formulas::state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  data::assignment_list l = state_formulas::detail::accessors::ass(f);
  data::variable_list result;
  for (data::assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = atermpp::push_front(result, i->lhs());
  }
  return atermpp::reverse(result);
}

/// \brief Returns the data expressions corresponding to ass(f)
/// \param f A modal formula
/// \return The data expressions corresponding to ass(f)
inline
data::data_expression_list mu_expressions(state_formulas::state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  data::assignment_list l = state_formulas::detail::accessors::ass(f);
  data::data_expression_list result;
  for (data::assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = atermpp::push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
