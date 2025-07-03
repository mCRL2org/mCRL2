// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/algorithms.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ALGORITHMS_H
#define MCRL2_PBES_ALGORITHMS_H

#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system::algorithms
{

/// \brief Removes parameters from propositional variable instantiations in a pbes expression
/// \param x A PBES library object that does not derive from atermpp::aterm
/// \param to_be_removed A set of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pbes& x, const std::set<data::variable>& to_be_removed);

/// \brief Removes parameters from propositional variable instantiations in a pbes expression
/// \param x A PBES library object that does not derive from atermpp::aterm
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pbes& x, const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed);

/// \brief The function normalize brings (embedded) pbes expressions into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing pbes expressions
void normalize(pbes& x);

/// \brief Checks if a PBEs is normalized
/// \return True if the PBES is normalized
bool is_normalized(const pbes& x);

/// \brief Attempts to eliminate the free variables of a PBES, by substituting
/// a constant value for them. If no constant value is found for one of the variables,
/// an exception is thrown.
void instantiate_global_variables(pbes& p);

/// \brief Apply finite instantiation to the given PBES.
void pbesinst_finite(pbes& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection);

/// \brief Returns true if a PBES is in BES form.
/// \param x a PBES
bool is_bes(const pbes& x);

/// \brief Print removed equations.
std::string print_removed_equations(const std::vector<propositional_variable>& removed);

/// \brief Removes equations that are not (syntactically) reachable from the initial state of a PBES.
/// \return The removed variables
std::vector<propositional_variable> remove_unreachable_variables(pbes& p);

/// \brief Returns the significant variables of a pbes expression.
std::set<data::variable> significant_variables(const pbes_expression& x);

} // namespace mcrl2::pbes_system::algorithms

#endif // MCRL2_PBES_ALGORITHMS_H
