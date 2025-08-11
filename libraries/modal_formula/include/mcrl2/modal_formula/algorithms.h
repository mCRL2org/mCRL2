// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/algorithms.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_ALGORITHMS_H
#define MCRL2_MODAL_FORMULA_ALGORITHMS_H

#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/modal_formula/state_formula_specification.h"

namespace mcrl2::state_formulas::algorithms
{

/// \brief Parses a state formula from an input stream
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param in A stream from which can be read
/// \param spec A linear process specification
/// \param formula_is_quantitative True if the formula is interpreted as a quantitative formula, instead of a classic boolean modal formula.
/// \return The converted modal formula
state_formula parse_state_formula(std::istream& in, lps::stochastic_specification& spec, bool formula_is_quantitative);

/// \brief Parses a state formula from text
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_text A string
/// \param spec A linear process specification
/// \param formula_is_quantitative True if the formula is interpreted as a quantitative formula, instead of a classic boolean modal formula.
/// \return The converted modal formula
state_formula
parse_state_formula(const std::string& formula_text, lps::stochastic_specification& spec, bool formula_is_quantitative);

/// \brief Parses a state formula specification from an input stream
state_formula_specification parse_state_formula_specification(std::istream& in, bool formula_is_quantitative);

/// \brief Parses a state formula specification from text
state_formula_specification parse_state_formula_specification(const std::string& text, bool formula_is_quantitative);

/// \brief Parses a state formula specification from an input stream
state_formula_specification parse_state_formula_specification(std::istream& in,
  lps::stochastic_specification& lpsspec,
  bool formula_is_quantitative);

/// \brief Parses a state formula specification from text
state_formula_specification parse_state_formula_specification(const std::string& text,
  lps::stochastic_specification& lpsspec,
  bool formula_is_quantitative);

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
bool is_monotonous(const state_formula& f);

/// \brief Normalizes a state formula, i.e. removes any occurrences of ! or =>.
state_formula normalize(const state_formula& x);

/// \brief Checks if a state formula is normalized
/// \return True if the state formula is normalized
bool is_normalized(const state_formula& x);

/// \brief Returns the names of the state variables that occur in x.
std::set<core::identifier_string> find_state_variable_names(const state_formula& x);

} // namespace mcrl2::state_formulas::algorithms

#endif // MCRL2_MODAL_FORMULA_ALGORITHMS_H
