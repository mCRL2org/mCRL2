// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H
#define MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H

#include "mcrl2/data/detail/free_variables.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace lps {

namespace detail {

struct free_variable_visitor: public pbes_expression_visitor
{
  data_variable_list bound_variables;
  std::vector<data_variable> quantifier_stack;
  std::vector<int> quantifier_stack_sizes;
  std::set<data_variable> result;

  // add the elements of v to the quantifier_stack
  void push(const data_variable_list& v)
  {
    quantifier_stack_sizes.push_back(v.size());
    std::copy(v.begin(), v.end(), std::back_inserter(quantifier_stack));
  }

  // remove the elements from the quantifier_stack that were most recently added by calling push()
  void pop()
  {
    int n = quantifier_stack_sizes.back();
    quantifier_stack_sizes.pop_back();
    quantifier_stack.erase(quantifier_stack.end() - n, quantifier_stack.end());
  }

  bool visit_forall(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return true;
  }

  void leave_forall()
  {
    pop();
  }

  bool visit_exists(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return true;
  }

  void leave_exists()
  {
    pop();
  }

  bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& v)
  {
    data_expression_list l = v.parameters();

    // collect all free variables from the parameters of the propositional variable
    for (data_expression_list::iterator i = l.begin(); i != l.end(); ++i)
      atermpp::for_each(*i, make_data_variable_collector(bound_variables.begin(),
                                                         bound_variables.end(),
                                                         quantifier_stack.begin(),
                                                         quantifier_stack.end(),
                                                         std::inserter(result, result.end()))
                                                        );
    return true;
  }

  bool visit_data_expression(const pbes_expression& e, const data_expression& d)
  {
    atermpp::for_each(d, make_data_variable_collector(bound_variables.begin(),
                                                      bound_variables.end(),
                                                      quantifier_stack.begin(),
                                                      quantifier_stack.end(),
                                                      std::inserter(result, result.end()))
                                                     );
    return true;
  }
};  

} // namespace detail

} // namespace lps

#endif // MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H

