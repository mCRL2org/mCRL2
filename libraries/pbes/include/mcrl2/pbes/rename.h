// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rename.h
/// \brief Rename function for quantifiers in pbes expressions.

#ifndef MCRL2_PBES_RENAME_H
#define MCRL2_PBES_RENAME_H

#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/quantifier_rename_builder.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Renames quantifier variables in the expression p, such that within the scope
/// of a quantifier variable, no other quantifier variables or free variables
/// with the same name occur.
/// \param p A PBES expression
/// \param free_variables A sequence of data variables
/// \return The rename result
inline
pbes_expression rename_quantifier_variables(const pbes_expression& p, const data::variable_list& free_variables)
{
  data::multiset_identifier_generator generator;
  generator.add_identifiers(data::find_identifiers(free_variables));
  return detail::make_quantifier_rename_builder(generator).visit(p);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_RENAME_H
