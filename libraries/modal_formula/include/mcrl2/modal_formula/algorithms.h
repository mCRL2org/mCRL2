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

#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula_specification.h"
#include <iostream>

namespace mcrl2 {

namespace state_formulas {

namespace algorithms {

/// \brief Parses a state formula from an input stream
// spec may be updated as the data implementation of the state formula
// may cause internal names to change.
/// \param in A stream from which can be read
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

/// \brief Parses a state formula specification from an input stream
state_formula_specification parse_state_formula_specification(std::istream& in);

/// \brief Parses a state formula specification from text
state_formula_specification parse_state_formula_specification(const std::string& text);

/// \brief Parses a state formula specification from an input stream
state_formula_specification parse_state_formula_specification(std::istream& in, lps::specification& lpsspec);

/// \brief Parses a state formula specification from text
state_formula_specification parse_state_formula_specification(const std::string& text, lps::specification& lpsspec);

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

} // namespace algorithms

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_ALGORITHMS_H
