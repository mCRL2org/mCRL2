// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_PBES_FIND_H
#define MCRL2_PBES_FIND_H

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/pbes_functional.h"

namespace mcrl2 {

namespace pbes_system {

using atermpp::aterm;
using atermpp::aterm_traits;

/// Returns true if the term has a given variable as subterm.
template <typename Term>
bool find_propositional_variable(Term t, const propositional_variable_instantiation& v)
{
  return atermpp::partial_find_if(t,
                                  detail::compare_propositional_variable_instantiation(v),
                                  is_propositional_variable_instantiation
                                 ) != atermpp::aterm();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_H
