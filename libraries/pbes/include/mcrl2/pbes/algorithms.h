// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/algorithms.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ALGORITHMS_H
#define MCRL2_PBES_ALGORITHMS_H

#include <set>
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/file_formats.h"

namespace mcrl2 {

namespace pbes_system {

namespace algorithms {

/// \brief Removes parameters from propositional variable instantiations in a pbes expression
/// \param x A PBES library object that does not derive from atermpp::aterm_appl
/// \param to_be_removed A set of parameters that are to be removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pbes<>& x, const std::set<data::variable>& to_be_removed);

/// \brief Removes parameters from propositional variable instantiations in a pbes expression
/// \param x A PBES library object that does not derive from atermpp::aterm_appl
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The expression \p x with parameters removed according to the mapping \p to_be_removed
void remove_parameters(pbes<>& x, const std::map<core::identifier_string, std::vector<size_t> >& to_be_removed);

/// \brief The function normalize brings (embedded) pbes expressions into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing pbes expressions
void normalize(pbes<>& x);

/// \brief Checks if a PBEs is normalized
/// \return True if the PBES is normalized
bool is_normalized(const pbes<>& x);

/// \brief Attempts to eliminate the free variables of a PBES, by substituting
/// a constant value for them. If no constant value is found for one of the variables,
/// an exception is thrown.
void instantiate_global_variables(pbes<>& p);

/// \brief Apply finite instantiation to the given PBES.
void pbesinst_finite(pbes<>& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection);

/// \brief Save a PBES in the format specified.
/// \param pbes_spec The pbes to be stored
/// \param outfilename The name of the file to which the output is stored.
/// \param output_format Determines the format in which the result is written.
/// \param aterm_ascii Determines, if output_format is pbes, whether the file
///        is written is ascii format.
void save_pbes(const pbes<>& pbes_spec, const std::string& outfilename, pbes_file_format output_format, bool aterm_ascii = false);

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
/// \param f The format that should be assumed for the file in infilename.
void load_pbes(pbes<>& p, const std::string& infilename, const pbes_file_format f);

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
///
/// The format of the file in infilename is guessed.
void load_pbes(pbes<>& p, const std::string& infilename);

/// \brief Returns true if a PBES is in BES form.
/// \param x a PBES
bool is_bes(const pbes<>& x);

/// \brief Print removed equations.
std::string print_removed_equations(const std::vector<propositional_variable>& removed);

/// \brief Removes equations that are not (syntactically) reachable from the initial state of a PBES.
/// \return The removed variables
std::vector<propositional_variable> remove_unreachable_variables(pbes<>& p);

/// \brief Returns the significant variables of a pbes expression.
std::set<data::variable> significant_variables(const pbes_expression& x);

} // namespace algorithms

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
