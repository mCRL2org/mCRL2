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
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
// \brief Visitor for checking if a pbes expression is normalized.
struct is_normalized_traverser: public traverser<is_normalized_traverser>
{
  typedef traverser<is_normalized_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();
  
  bool result;

  is_normalized_traverser()
    : result(true)
  {}

  /// \brief Visit not node
  void enter(const not_& /* x */)
  {
    result = false;
  }

  /// \brief Visit imp node
  void enter(const imp& /* x */)
  {
    result = false;
  }
};
/// \endcond

/// \brief Checks if a pbes expression is normalized
/// \param t A PBES expression
/// \return True if the pbes expression is normalized
inline
bool is_normalized(const pbes_expression& t)
{
  is_normalized_traverser f;
  f(t);
  return f.result;
}

/// \brief The function normalize brings a pbes expression into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param f A PBES expression
/// \return The result of the normalization.
inline
pbes_expression normalize(pbes_expression f)
{
  namespace p = pbes_expr;
  using namespace accessors;

  if (is_pbes_not(f))
  {
    f = arg(f); // remove the not
    if (data::is_data_expression(f)) {
      return data::sort_bool::not_(f);
    } else if (is_pbes_true(f)) {
      return p::false_();
    } else if (is_pbes_false(f)) {
      return p::true_();
    } else if (is_pbes_not(f)) {
      return normalize(arg(f));
    } else if (is_pbes_and(f)) {
      return p::or_(normalize(not_(left(f))), normalize(not_(right(f))));
    } else if (is_pbes_or(f)) {
      return p::and_(normalize(not_(left(f))), normalize(not_(right(f))));
    } else if (is_pbes_imp(f)) {
      return p::and_(normalize(left(f)), normalize(not_(right(f))));
    } else if (is_pbes_forall(f)) {
      return p::exists(var(f), normalize(not_(arg(f))));
    } else if (is_pbes_exists(f)) {
      return p::forall(var(f), normalize(not_(arg(f))));
    } else if (is_propositional_variable_instantiation(f)) {
      throw mcrl2::runtime_error(std::string("normalize error: illegal argument ") + f.to_string());
    }
  }
  else // !is_pbes_not(f)
  {
    if (data::is_data_expression(f)) {
      return f;
    } else if (is_pbes_true(f)) {
      return f;
    } else if (is_pbes_false(f)) {
      return f;
    //} else if (is_not(f)) {
    // ;
    } else if (is_pbes_and(f)) {
      return p::and_(normalize(left(f)), normalize(right(f)));
    } else if (is_pbes_or(f)) {
      return p::or_(normalize(left(f)), normalize(right(f)));
    } else if (is_pbes_imp(f)) {
      return p::or_(normalize(not_(left(f))), normalize(right(f)));
    } else if (is_pbes_forall(f)) {
      return pbes_expr::forall(var(f), normalize(arg(f)));
    } else if (is_pbes_exists(f)) {
      return pbes_expr::exists(var(f), normalize(arg(f)));
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
