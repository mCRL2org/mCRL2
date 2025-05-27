// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/include/mcrl2/pbes/pbes_functions.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_FUNCTIONS_H
#define MCRL2_PBES_PBES_FUNCTIONS_H

#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/detail/pbes_remove_counterexample_info.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Visitor for printing the root node of a PBES.
struct print_brief_traverser: public pbes_expression_traverser<print_brief_traverser>
{
  typedef pbes_expression_traverser<print_brief_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::string result;

  print_brief_traverser()
    : result("")
  {}

  void apply(const not_& /* x */)
  {
    result = "not";
  }

  void apply(const imp& /* x */)
  {
    result = "imp";
  }

  void apply(const forall& /* x */)
  {
    result = "forall";
  }

  void apply(const exists& /* x */)
  {
    result = "exists";
  }

  void apply(const propositional_variable_instantiation& x)
  {
    result = "propvar " + std::string(x.name());
  }

  void apply(const pbes_equation& x)
  {
    result = "equation " + std::string(x.variable().name());
  }
};

/// \brief Returns a string representation of the root node of a PBES.
/// \param x a PBES object
template <typename T>
std::string print_brief(const T& x)
{
  print_brief_traverser f;
  f.apply(x);
  return f.result;
}


/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is a simple pbes expression.
struct is_simple_expression_traverser: public pbes_expression_traverser<is_simple_expression_traverser>
{
  typedef pbes_expression_traverser<is_simple_expression_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;
  bool allow_counter_example_variables = false;

  is_simple_expression_traverser(bool allow_counter_example_variables)
    : result(true),
      allow_counter_example_variables(allow_counter_example_variables)
  {}

  void enter(const propositional_variable_instantiation& x)
  {
    if (!allow_counter_example_variables || !detail::is_counter_example_instantiation(x))
    {
      result = false;
    }
  }
};
/// \endcond

/// \brief Determines if an expression is a simple expression.
/// An expression is simple if it is free of propositional variables.
/// \param x a PBES object
/// \param allow_counter_example_propvar If true, counter example propositional variables are seen as simple expressions.
/// \return true if x is a simple expression.
template <typename T>
bool is_simple_expression(const T& x, bool allow_counter_example_propvar)
{
  is_simple_expression_traverser f(allow_counter_example_propvar);
  f.apply(x);
  return f.result;
}

/// \brief Test for a disjunction
/// \param t A PBES expression
/// \return True if it is a disjunction and not a simple expression
inline bool is_non_simple_disjunct(const pbes_expression& t)
{
  return is_pbes_or(t) && !is_simple_expression(t, false);
}

/// \brief Test for a conjunction
/// \param t A PBES expression
/// \return True if it is a conjunction and not a simple expression
inline bool is_non_simple_conjunct(const pbes_expression& t)
{
  return is_pbes_and(t) && !is_simple_expression(t, false);
}

/// \brief Splits a disjunction into a sequence of operands.
/// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression.
/// \param split_simple_expr If true, pbes disjuncts are split, even if
///        no proposition variables occur. If false, pbes disjuncts are only split
///        if a proposition variable occurs somewhere in \a expr.
/// \note This never splits data disjuncts.
/// \return A sequence of operands.
inline
std::vector<pbes_expression> split_disjuncts(const pbes_expression& expr, bool split_simple_expr = false)
{
  using namespace accessors;
  std::vector<pbes_expression> result;

  if (split_simple_expr)
  {
    utilities::detail::split(expr, std::back_insert_iterator<std::vector<pbes_expression> >(result), is_or, left, right);
  }
  else
  {
    utilities::detail::split(expr, std::back_insert_iterator<std::vector<pbes_expression> >(result), is_non_simple_disjunct, left, right);
  }

  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_simple_expr If true, pbes conjuncts are split, even if
///        no proposition variables occur. If false, pbes conjuncts are only split
///        if a proposition variable occurs somewhere in \a expr.
/// \note This never splits data conjuncts.
/// \return A sequence of operands
inline
std::vector<pbes_expression> split_conjuncts(const pbes_expression& expr, bool split_simple_expr = false)
{
  using namespace accessors;
  std::vector<pbes_expression> result;

  if (split_simple_expr)
  {
    utilities::detail::split(expr, std::back_insert_iterator<std::vector<pbes_expression> >(result), is_and, left, right);
  }
  else
  {
    utilities::detail::split(expr, std::back_insert_iterator<std::vector<pbes_expression> >(result), is_non_simple_conjunct, left, right);
  }

  return result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_FUNCTIONS_H
