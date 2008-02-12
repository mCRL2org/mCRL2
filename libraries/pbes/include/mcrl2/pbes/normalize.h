// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normalize.h
/// \brief Normalization of pbes expressions.

#ifndef MCRL2_PBES_NORMALIZE_H
#define MCRL2_PBES_NORMALIZE_H

#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/pbes_equation.h"

namespace mcrl2 {

namespace pbes_system {

/// Visitor for checking if a pbes expression is normalized.
struct is_normalized_visitor : public pbes_expression_visitor
{
  bool result;
  
  is_normalized_visitor()
    : result(true)
  {}
  
  bool visit_not(const pbes_expression& /* e */, const pbes_expression& /* arg */)
  {
    result = false;
    return stop_recursion;
  }

  bool visit_imp(const pbes_expression& /* e */, const pbes_expression& /* left */, const pbes_expression& /* right */) 
  {
    result = false;
    return stop_recursion;
  }
};


/// The function normalize brings a pbes expression into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \return The result of the normalization.
inline
pbes_expression normalize(pbes_expression f)
{
  using namespace pbes_expr;
  using namespace accessors;

  if (is_pbes_not(f))
  {
    f = not_arg(f); // remove the not
    if (is_data(f)) {
      return data::data_expr::not_(f);
    } else if (is_pbes_true(f)) {
      return false_();
    } else if (is_pbes_false(f)) {
      return true_();
    } else if (is_pbes_not(f)) {
      return normalize(not_arg(f));
    } else if (is_pbes_and(f)) {
      return or_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_pbes_or(f)) {
      return and_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_pbes_imp(f)) {
      return and_(normalize(lhs(f)), normalize(not_(rhs(f))));
    } else if (is_pbes_forall(f)) {
      return exists(quant_vars(f), normalize(not_(quant_expr(f))));
    } else if (is_pbes_exists(f)) {
      return forall(quant_vars(f), normalize(not_(quant_expr(f))));
    } else if (is_propositional_variable_instantiation(f)) {
      throw std::runtime_error(std::string("normalize error: illegal argument ") + f.to_string());
    }
  }
  else // !is_pbes_not(f)
  {
    if (is_data(f)) {
      return f;
    } else if (is_pbes_true(f)) {
      return f;
    } else if (is_pbes_false(f)) {
      return f;
    //} else if (is_not(f)) {
    // ;
    } else if (is_pbes_and(f)) {
      return and_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_pbes_or(f)) {
      return or_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_pbes_imp(f)) {
      return or_(normalize(not_(lhs(f))), normalize(rhs(f)));
    } else if (is_pbes_forall(f)) {
      return forall(quant_vars(f), normalize(quant_expr(f)));
    } else if (is_pbes_exists(f)) {
      return exists(quant_vars(f), normalize(quant_expr(f)));
    } else if (is_propositional_variable_instantiation(f)) {
      return f;
    }
  }
  throw std::runtime_error(std::string("normalize error: unknown argument ") + f.to_string());
  return pbes_expression();
}

/// Applies normalization to the right hand side of the equation.
/// \return The result of the normalization.
/// 
inline
pbes_equation normalize(const pbes_equation& e)
{
  return pbes_equation(e.symbol(), e.variable(), normalize(e.formula()));
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_NORMALIZE_H
