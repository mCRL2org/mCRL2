// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/preprocess_state_formula.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H
#define MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"

namespace mcrl2
{

namespace state_formulas
{

/// \brief Renames data variables and predicate variables in the formula \p f, and
/// wraps the formula inside a 'nu' if needed. This is needed as a preprocessing
/// step for the algorithm.
/// \param formula A modal formula
/// \param spec A linear process specification
/// \return The preprocessed formula
inline
state_formulas::state_formula preprocess_state_formula(const state_formulas::state_formula& formula, const lps::specification& spec)
{
  using namespace state_formulas::detail::accessors;
  namespace s = state_formulas;

  state_formulas::state_formula f = formula;
  std::set<core::identifier_string> formula_variable_names = data::detail::variable_names(state_formulas::find_variables(formula));
  std::set<core::identifier_string> spec_variable_names = data::detail::variable_names(lps::find_variables(spec));
  std::set<core::identifier_string> spec_names = lps::find_identifiers(spec);

  // rename data variables in f, to prevent name clashes with data variables in spec
  f = state_formulas::rename_variables(f, spec_variable_names);

  // rename predicate variables in f, to prevent name clashes
  data::xyz_identifier_generator xyz_generator;
  xyz_generator.add_identifiers(spec_names);
  xyz_generator.add_identifiers(formula_variable_names);
  f = rename_predicate_variables(f, xyz_generator);

  // wrap the formula inside a 'nu' if needed
  if (!s::is_mu(f) && !s::is_nu(f))
  {
    data::set_identifier_generator generator;
    generator.add_identifiers(state_formulas::find_identifiers(f));
    generator.add_identifiers(lps::find_identifiers(spec));
    core::identifier_string X = generator("X");
    f = s::nu(X, data::assignment_list(), f);
  }

  return f;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H
