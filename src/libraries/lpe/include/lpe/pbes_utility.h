///////////////////////////////////////////////////////////////////////////////
/// \file pbes_utility.h

#ifndef LPE_PBES_UTILITY_H
#define LPE_PBES_UTILITY_H

#include "lpe/pbes.h"
#include "atermpp/algorithm.h"

namespace lpe {

/// Returns all propositional variables that occur in the term t.
template <typename Term>
std::set<propositional_variable> find_propositional_variable(Term t)
{
  std::set<propositional_variable> variables;
  atermpp::find_all_if(t, is_propositional_variable, std::inserter(variables, variables.end()));
  return variables;
}

/// Returns all propositional variable instantiations that occur in the term t.
template <typename Term>
std::set<propositional_variable_instantiation> find_propositional_variable_instantiations(Term t)
{
  std::set<propositional_variable_instantiation> variables;
  atermpp::find_all_if(t, is_propositional_variable_instantiation, std::inserter(variables, variables.end()));
  return variables;
}

} // namespace lpe

#endif // LPE_PBES_UTILITY_H
