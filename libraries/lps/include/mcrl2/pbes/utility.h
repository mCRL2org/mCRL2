// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file mcrl2/pbes/pbes_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_PBES_UTILITY_H
#define MCRL2_PBES_PBES_UTILITY_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/lps/mucalculus.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/data.h"
#include "mcrl2/lps/data_utility.h"
#include "mcrl2/lps/sort.h"
#include "atermpp/algorithm.h"
#include "dataimpl.h" // implement_data_data_expr

namespace lps {

/// \brief Returns all propositional variables that occur in the term t
template <typename Term>
std::set<propositional_variable> find_propositional_variables(Term t)
{
  std::set<propositional_variable> variables;
  atermpp::find_all_if(t, is_propositional_variable, std::inserter(variables, variables.end()));
  return variables;
}

/// \brief Returns all propositional variable instantiations that occur in the term t
template <typename Term>
std::set<propositional_variable_instantiation> find_propositional_variable_instantiations(Term t)
{
  std::set<propositional_variable_instantiation> variables;
  atermpp::find_all_if(t, is_propositional_variable_instantiation, std::inserter(variables, variables.end()));
  return variables;
}

/// Converts a pbes expression to a data expression. Note that the specification
/// spec may change as a result of this.
inline
data_expression pbes2data(const pbes_expression& p, specification& spec)
{
  using namespace pbes_expr;
  namespace d = lps::data_expr;
  namespace s = lps::sort_expr;
  
  if (is_data(p)) {
    return val(p);
  } else if (is_true(p)) {
    return d::true_();
  } else if(is_false(p)) {
    return d::false_();
  } else if (is_and(p)) {
    return d::and_(pbes2data(lhs(p), spec), pbes2data(rhs(p), spec));
  } else if (is_or(p)) {
    return d::or_(pbes2data(lhs(p), spec), pbes2data(rhs(p), spec));
  } else if (is_forall(p)) {
      aterm_appl x = gsMakeBinder(gsMakeForall(), quant_vars(p), pbes2data(quant_expr(p), spec));
      return implement_data_data_expr(x, spec);
  } else if (is_exists(p)) {
      aterm_appl x = gsMakeBinder(gsMakeExists(), quant_vars(p), pbes2data(quant_expr(p), spec));
      return implement_data_data_expr(x, spec);
  } else if (is_propositional_variable_instantiation(p)) {
    identifier_string vname = var_name(p);
    data_expression_list parameters = var_val(p);
    sort_list sorts = apply(parameters, gsGetSort);
    // In order to use gsMakeSortArrow sorts must be non-empty
    // else an extra case should be added to just make vsort == s::bool_();
    assert(!sorts.empty());
    lps::sort vsort = gsMakeSortArrow(sorts, s::bool_());
    data_variable v(gsMakeDataVarId(vname, vsort));
    return gsMakeDataApplList(v, parameters);
  }
  throw std::runtime_error(std::string("pbes2data error: unknown pbes_variable_instantiation ") + p.to_string());
  return data_expression(); // to prevent compiler warnings
}

/// Converts a data expression to a pbes expression.
inline
pbes_expression data2pbes(data_expression q)
{
  namespace d = lps::data_expr;
  namespace p = lps::pbes_expr;
  namespace s = lps::sort_expr;

  data_expression head = q.head();
  data_expression_list arguments = q.arguments();
  if (is_data_variable(head))
  {
    return propositional_variable_instantiation(data_variable(head).name(), arguments);
  }
  else // head must be an operation id
  {
    assert(is_data_operation(head));
    if (d::is_true(head)) {
      return p::true_();
    } else if (d::is_false(head)) {
      return p::false_();
    } else if (d::is_and(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    } else if (d::is_or(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    }
    throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
    return pbes_expression();
  }
  throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
  return pbes_expression();
}

} // namespace lps

#endif // MCRL2_PBES_PBES_UTILITY_H
