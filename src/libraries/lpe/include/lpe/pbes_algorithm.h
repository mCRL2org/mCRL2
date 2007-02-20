///////////////////////////////////////////////////////////////////////////////
/// \file pbes_algorithm.h
/// Contains pbes algorithms.

#ifndef PBES_ALGORITHM_H
#define PBES_ALGORITHM_H

#include <string>
#include "lpe/pbes.h"
#include "lpe/pbes_init.h"

namespace lpe {

/// Returns true if the pbes expression t is a bes expression.
inline
bool is_bes(pbes_expression t)
{
  using namespace pbes_init;
  using namespace pbes_expr;

  if (is_and(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if (is_or(t)) {
    return is_bes(lhs(t)) && is_bes(rhs(t));
  }
  else if (is_forall(t)) {
    return false;
  }
  else if (is_exists(t)) {
    return false;
  }
  else if (is_propositional_variable_instantiation(t)) {
    return propositional_variable_instantiation(t).parameters().empty();
  }
  else if (is_true(t)) {
    return true;
  }
  else if (is_false(t)) {
    return true;
  }

  return false;
}

} // namespace lpe

#endif // PBES_ALGORITHM_H
