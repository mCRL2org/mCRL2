// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/include/mcrl2/pbes/pbes_functions.h
/// \brief add your file description here.

#ifndef PBES_INCLUDE_MCRL2_PBES_FUNCTIONS_H
#define PBES_INCLUDE_MCRL2_PBES_FUNCTIONS_H

#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Visitor for printing the root node of a PBES.
struct print_brief_traverser: public pbes_expression_traverser<print_brief_traverser>
{
  typedef pbes_expression_traverser<print_brief_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  std::string result;

  print_brief_traverser()
    : result("")
  {}

  void operator()(const not_& /* x */)
  {
    result = "not";
  }

  void operator()(const imp& /* x */)
  {
    result = "imp";
  }

  void operator()(const forall& /* x */)
  {
    result = "forall";
  }

  void operator()(const exists& /* x */)
  {
    result = "exists";
  }

  void operator()(const propositional_variable_instantiation& x)
  {
    result = "propvar " + std::string(x.name());
  }

  void operator()(const pbes_equation& x)
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
  f(x);
  return f.result;
}


/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is a simple pbes expression.
struct is_simple_expression_traverser: public pbes_expression_traverser<is_simple_expression_traverser>
{
  typedef pbes_expression_traverser<is_simple_expression_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  bool result;

  is_simple_expression_traverser()
    : result(true)
  {}

  void enter(const propositional_variable_instantiation& /*x*/)
  {
    result = false;
  }
};
/// \endcond

/// \brief Determines if an expression is a simple expression.
/// An expression is simple if it is free of propositional variables.
/// \param x a PBES object
/// \return true if x is a simple expression.
template <typename T>
bool is_simple_expression(const T& x)
{
  is_simple_expression_traverser f;
  f(x);
  return f.result;
}

/// \brief Test for a disjunction
/// \param t A PBES expression
/// \return True if it is a disjunction and not a simple expression
inline bool is_non_simple_disjunct(const pbes_expression& t)
{
  return is_pbes_or(t) && !is_simple_expression(t);
}

/// \brief Test for a conjunction
/// \param t A PBES expression
/// \return True if it is a conjunction and not a simple expression
inline bool is_non_simple_conjunct(const pbes_expression& t)
{
  return is_pbes_and(t) && !is_simple_expression(t);
}

namespace pbes_expr {

/// \brief Splits a disjunction into a sequence of operands
/// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_simple_expressions if true, pbes disjuncts are split, even if
///        no proposition variables occur. If false, pbes disjuncts are only split
///        if a proposition variable occurs somewhere in \a expr.
/// \note This never splits data disjuncts.
/// \return A sequence of operands
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
/// \param split_simple_expressions if true, pbes conjuncts are split, even if
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

} // namespace pbes_expr

} // namespace pbes_system

} // namespace mcrl2

#endif // PBES_INCLUDE_MCRL2_PBES_FUNCTIONS_H
