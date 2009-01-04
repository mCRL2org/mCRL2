// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normalize.h
/// \brief Normalization of pbes expressions.

#ifndef MCRL2_PBES_NORMALIZE_H
#define MCRL2_PBES_NORMALIZE_H

#include "mcrl2/exception.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/pbes_equation.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
// \brief Visitor for checking if a pbes expression is normalized.
struct is_normalized_visitor : public pbes_expression_visitor<pbes_expression>
{
  bool result;

  is_normalized_visitor()
    : result(true)
  {}

  /// \brief Visit not node
  /// \return The result of visiting the node
  bool visit_not(const pbes_expression& /* e */, const pbes_expression& /* arg */)
  {
    result = false;
    return stop_recursion;
  }

  /// \brief Visit imp node
  /// \return The result of visiting the node
  bool visit_imp(const pbes_expression& /* e */, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    result = false;
    return stop_recursion;
  }
};
/// \endcond

/// \brief Checks if a pbes expression is normalized
/// \param t A PBES expression
/// \return True if the pbes expression is normalized
inline
bool is_normalized(pbes_expression t)
{
  is_normalized_visitor visitor;
  visitor.visit(t);
  return visitor.result;
}

/// \brief The function normalize brings a pbes expression into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param f A PBES expression
/// \return The result of the normalization.
inline
pbes_expression normalize(pbes_expression f)
{
  using namespace pbes_expr;
  using namespace accessors;

  if (is_pbes_not(f))
  {
    f = arg(f); // remove the not
    if (is_data(f)) {
      return data::data_expr::not_(f);
    } else if (is_pbes_true(f)) {
      return false_();
    } else if (is_pbes_false(f)) {
      return true_();
    } else if (is_pbes_not(f)) {
      return normalize(arg(f));
    } else if (is_pbes_and(f)) {
      return or_(normalize(not_(left(f))), normalize(not_(right(f))));
    } else if (is_pbes_or(f)) {
      return and_(normalize(not_(left(f))), normalize(not_(right(f))));
    } else if (is_pbes_imp(f)) {
      return and_(normalize(left(f)), normalize(not_(right(f))));
    } else if (is_pbes_forall(f)) {
      return exists(var(f), normalize(not_(arg(f))));
    } else if (is_pbes_exists(f)) {
      return forall(var(f), normalize(not_(arg(f))));
    } else if (is_propositional_variable_instantiation(f)) {
      throw mcrl2::runtime_error(std::string("normalize error: illegal argument ") + f.to_string());
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
      return and_(normalize(left(f)), normalize(right(f)));
    } else if (is_pbes_or(f)) {
      return or_(normalize(left(f)), normalize(right(f)));
    } else if (is_pbes_imp(f)) {
      return or_(normalize(not_(left(f))), normalize(right(f)));
    } else if (is_pbes_forall(f)) {
      return forall(var(f), normalize(arg(f)));
    } else if (is_pbes_exists(f)) {
      return exists(var(f), normalize(arg(f)));
    } else if (is_propositional_variable_instantiation(f)) {
      return f;
    }
  }
  throw mcrl2::runtime_error(std::string("normalize error: unknown argument ") + f.to_string());
  return pbes_expression();
}

/// \brief Applies normalization to the right hand side of the equation.
/// \param e A PBES equation
/// \return The result of the normalization.
inline
pbes_equation normalize(const pbes_equation& e)
{
  return pbes_equation(e.symbol(), e.variable(), normalize(e.formula()));
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_NORMALIZE_H
