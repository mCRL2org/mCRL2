// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file complement.h
/// \brief The complement function for pbes expressions.
//
// Comp ( val(b) ) = val (! b)
//
// Comp ( X(d) ) = "ABORT, should not happen"
//
// Comp ( PbesAnd (f, g) ) = PbesOr (Comp (f), Comp (g) )
//
// Comp ( PbesOr (f, g) ) = PbesAnd (Comp (f), Comp (g) )
//
// Comp (PbesForAll (f) ) = PbesExists (Comp (f) )
//
// Comp (PbesExists (f) ) = PbesForall (Comp (f) )
//
// Comp (Comp (f) ) = f

#ifndef MCRL2_PBES_COMPLEMENT_H
#define MCRL2_PBES_COMPLEMENT_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2
{

namespace pbes_system
{

/// \cond INTERNAL_DOCS
// \brief Visitor that pushes a negation in a PBES expression as far as possible
// inwards towards a data expression.
template <typename Derived>
struct complement_builder: public pbes_expression_builder<Derived>
{
  typedef pbes_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  pbes_expression apply(const data::data_expression& x)
  {
    return data::sort_bool::not_(x);
  }

  pbes_expression apply(const and_& x)
  {
    return pbes_expr_optimized::or_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
  }

  pbes_expression apply(const or_& x)
  {
    return pbes_expr_optimized::and_(static_cast<Derived&>(*this).apply(x.left()), static_cast<Derived&>(*this).apply(x.right()));
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    throw mcrl2::runtime_error(std::string("complement_builder error: unexpected propositional variable encountered ") + mcrl2::pbes_system::pp(x));
    return pbes_expression();
  }
};
/// \endcond

/// \brief Returns the complement of a pbes expression
/// \param x A PBES expression
/// \return The expression obtained by pushing the negations in the pbes
/// expression as far as possible inwards towards a data expression.
inline
pbes_expression complement(const pbes_expression& x)
{
  return core::make_apply_builder<complement_builder>().apply(x);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_COMPLEMENT_H
