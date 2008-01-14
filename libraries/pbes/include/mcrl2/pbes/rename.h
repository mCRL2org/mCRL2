// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rename.h
/// \brief Rename function for quantifiers in pbes expressions.

#ifndef MCRL2_PBES_RENAME_H
#define MCRL2_PBES_RENAME_H

#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace lps {

/// Renames quantifier variables in the expression p, such that within the scope
/// of a quantifier variable, no other quantifier variables or free variables
/// with the same name occur.
inline
pbes_expression rename_quantifier_variables(const pbes_expression& p, const data_variable_list& free_variables)
{
  multiset_identifier_generator generator(free_variables);
  return detail::make_quantifier_rename_builder(generator).visit(p);
}

} // namespace lps

#endif // MCRL2_PBES_RENAME_H
