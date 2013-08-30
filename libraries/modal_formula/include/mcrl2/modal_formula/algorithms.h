// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/algorithms.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_ALGORITHMS_H
#define MCRL2_MODAL_FORMULA_ALGORITHMS_H

#include <iostream>
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace state_formulas {

namespace algorithms {

/// \brief Parses a state formula from an input stream
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_stream A stream from which can be read
/// \param spec A linear process specification
/// \return The converted modal formula
state_formula parse_state_formula(std::istream& in, lps::specification& spec);

/// \brief Parses a state formula from text
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param formula_text A string
/// \param spec A linear process specification
/// \return The converted modal formula
state_formula parse_state_formula(const std::string& formula_text, lps::specification& spec);

/// \brief Renames data variables and predicate variables in the formula \p f, and
/// wraps the formula inside a 'nu' if needed. This is needed as a preprocessing
/// step for the algorithm.
/// \param formula A modal formula
/// \param spec A linear process specification
/// \return The preprocessed formula
state_formulas::state_formula preprocess_state_formula(const state_formulas::state_formula& formula, const lps::specification& spec);

/// \brief Returns true if the state formula is monotonous.
/// \param f A modal formula
/// \return True if the state formula is monotonous.
bool is_monotonous(state_formula f);

/// \brief Normalizes a state formula, i.e. removes any occurrences of ! or =>.
state_formula normalize(const state_formula& x);

/// \brief Checks if a state formula is normalized
/// \return True if the state formula is normalized
bool is_normalized(const state_formula& x);

/// \brief Returns the names of the state variables that occur in x.
std::set<core::identifier_string> find_state_variable_names(const state_formula& x);

}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_ALGORITHMS_H
