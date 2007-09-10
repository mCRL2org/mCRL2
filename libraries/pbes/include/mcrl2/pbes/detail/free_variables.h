// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variables.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_FREE_VARIABLES_H
#define MCRL2_PBES_DETAIL_FREE_VARIABLES_H

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "print/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"

namespace lps {

namespace detail {

/// Computes the free variables in the pbes expression t.
///
inline
void collect_free_pbes_variables(pbes_expression t,
                                 const data_variable_list& bound_variables,
                                 std::vector<data_variable>& quantifier_stack,
                                 std::set<data_variable>& result
                                )
{
  using namespace lps::pbes_expr;

  if(is_and(t)) {
    collect_free_pbes_variables(lhs(t), bound_variables, quantifier_stack, result);
    collect_free_pbes_variables(rhs(t), bound_variables, quantifier_stack, result);
  }
  else if(is_or(t)) {
    collect_free_pbes_variables(lhs(t), bound_variables, quantifier_stack, result);
    collect_free_pbes_variables(rhs(t), bound_variables, quantifier_stack, result);
  }
  else if(is_forall(t)) {
    data_variable_list vars = quant_vars(t);   
    std::copy(vars.begin(), vars.end(), std::back_inserter(quantifier_stack));
    collect_free_pbes_variables(quant_expr(t), bound_variables, quantifier_stack, result);
    quantifier_stack.erase(quantifier_stack.end() - vars.size(), quantifier_stack.end());
  }
  else if(is_exists(t)) {
    data_variable_list vars = quant_vars(t);   
    std::copy(vars.begin(), vars.end(), std::back_inserter(quantifier_stack));
    collect_free_pbes_variables(quant_expr(t), bound_variables, quantifier_stack, result);
    quantifier_stack.erase(quantifier_stack.end() - vars.size(), quantifier_stack.end());
  }
  else if(is_propositional_variable_instantiation(t)) {
    data_expression_list l = propositional_variable_instantiation(t).parameters();
    for (data_expression_list::iterator i = l.begin(); i != l.end(); ++i)
      atermpp::for_each(*i, make_data_variable_collector(bound_variables.begin(),
                                                         bound_variables.end(),
                                                         quantifier_stack.begin(),
                                                         quantifier_stack.end(),
                                                         std::inserter(result, result.end()))
                                                        );
  }
  else if(is_true(t)) {
  }
  else if(is_false(t)) {
  }
  else if(is_data(t)) {
      atermpp::for_each(t, make_data_variable_collector(bound_variables.begin(),
                                                        bound_variables.end(),
                                                        quantifier_stack.begin(),
                                                        quantifier_stack.end(),
                                                        std::inserter(result, result.end()))
                                                       );
  }
}

/// Computes the free variables in the sequence of pbes equations [first, last[.
///
template <typename EquationIterator>
std::set<data_variable> free_pbes_variables(EquationIterator first, EquationIterator last)
{
  std::set<data_variable> result;
  for (EquationIterator i = first; i != last; ++i)
  {
    std::vector<data_variable> quantifier_stack;
    collect_free_pbes_variables(i->formula(), i->variable().parameters(), quantifier_stack, result);
  }
  return result;
}

} // namespace detail

} // namespace lps

#endif // MCRL2_PBES_DETAIL_FREE_VARIABLES_H
