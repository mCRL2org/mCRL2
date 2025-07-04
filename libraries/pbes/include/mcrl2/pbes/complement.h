// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

#include "mcrl2/data/consistency.h"
#include "mcrl2/pbes/builder.h"



namespace mcrl2::pbes_system
{

/// \cond INTERNAL_DOCS
// \brief Visitor that pushes a negation in a PBES expression as far as possible
// inwards towards a data expression.
template <typename Derived>
struct complement_builder: public pbes_expression_builder<Derived>
{
  using super = pbes_expression_builder<Derived>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = data::not_(x);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    pbes_expression left;
    static_cast<Derived&>(*this).apply(left, x.left());
    pbes_expression right;
    static_cast<Derived&>(*this).apply(right, x.right());
    optimized_or(result, left, right);
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    pbes_expression left;
    static_cast<Derived&>(*this).apply(left, x.left());
    pbes_expression right;
    static_cast<Derived&>(*this).apply(right, x.right());
    optimized_and(result, left, right);
  }

  template <class T>
  void apply(T& /* result */, const propositional_variable_instantiation& x)
  {
    throw mcrl2::runtime_error(std::string("complement_builder error: unexpected propositional variable encountered ") + mcrl2::pbes_system::pp(x));
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
  pbes_expression result;
  core::make_apply_builder<complement_builder>().apply(result, x);
  return result;
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_COMPLEMENT_H
