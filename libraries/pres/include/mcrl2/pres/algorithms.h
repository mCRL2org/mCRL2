// Author(s): Jan Friso Groote. Based on pbes/algorithms.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/algorithms.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_ALGORITHMS_H
#define MCRL2_PRES_ALGORITHMS_H

#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/pres/pres.h"

namespace mcrl2::pres_system::algorithms {

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that does not derive from atermpp::aterm
/// \param to_be_removed A set of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pres& x, const std::set<data::variable>& to_be_removed);

/// \brief Removes parameters from propositional variable instantiations in a pres expression
/// \param x A PRES library object that does not derive from atermpp::aterm
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pres& x, const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed);

/// \brief The function normalize brings (embedded) pres expressions into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing pres expressions
void normalize(pres& x);

/// \brief Checks if a PRES is normalized
/// \return True if the PRES is normalized
bool is_normalized(const pres& x);

/// \brief Attempts to eliminate the free variables of a PRES, by substituting
/// a constant value for them. If no constant value is found for one of the variables,
/// an exception is thrown.
void instantiate_global_variables(pres& p);

/// \brief Apply finite instantiation to the given PRES.
void presinst_finite(pres& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection);

/// \brief Returns true if a PRES is in RES form.
/// \param x a PRES
bool is_res(const pres& x);


/// \brief Print removed equations.
std::string print_removed_equations(const std::vector<propositional_variable>& removed);

/// \brief Removes equations that are not (syntactically) reachable from the initial state of a PRES.
/// \return The removed variables
std::vector<propositional_variable> remove_unreachable_variables(pres& p);

/// \brief Returns the significant variables of a pres expression.
std::set<data::variable> significant_variables(const pres_expression& x);


} // namespace mcrl2::pres_system::algorithms





#endif // MCRL2_PRES_ALGORITHMS_H
