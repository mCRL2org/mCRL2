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
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/pbes.h"

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

} // namespace algorithms

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
