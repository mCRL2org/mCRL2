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

#include <stdexcept>
#include "mcrl2/pbes/pbes_expression_builder.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/sequence_substitution.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
// \brief Visitor that pushes a negation in a PBES expression as far as possible
// inwards towards a data expression.
struct complement_builder: public pbes_expression_builder<pbes_expression>
{
  /// \brief Visit data_expression node
  /// \param d A data expression
  /// \return The result of visiting the node
  pbes_expression visit_data_expression(const pbes_expression& /* e */, const data::data_expression& d)
  {
    return data::sort_bool::not_(d);
  }

  /// \brief Visit true node
  /// \return The result of visiting the node
  pbes_expression visit_true(const pbes_expression& /* e */)
  {
    using namespace pbes_expr_optimized;
    return false_();
  }

  /// \brief Visit false node
  /// \return The result of visiting the node
  pbes_expression visit_false(const pbes_expression& /* e */)
  {
    using namespace pbes_expr_optimized;
    return true_();
  }

  /// \brief Visit and node
  /// \param left A PBES expression
  /// \param right A PBES expression
  /// \return The result of visiting the node
  pbes_expression visit_and(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace pbes_expr_optimized;
    return or_(visit(left), visit(right));
  }

  /// \brief Visit or node
  /// \param left A PBES expression
  /// \param right A PBES expression
  /// \return The result of visiting the node
  pbes_expression visit_or(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace pbes_expr_optimized;
    return and_(visit(left), visit(right));
  }

  /// \brief Visit forall node
  /// \param variables A sequence of data variables
  /// \param expression A PBES expression
  /// \return The result of visiting the node
  pbes_expression visit_forall(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& expression)
  {
    return pbes_expr_optimized::exists(variables, visit(expression));
  }

  /// \brief Visit exists node
  /// \param variables A sequence of data variables
  /// \param expression A PBES expression
  /// \return The result of visiting the node
  pbes_expression visit_exists(const pbes_expression& /* e */, const data::variable_list& variables, const pbes_expression& expression)
  {
    return pbes_expr_optimized::forall(variables, visit(expression));
  }

  /// \brief Visit propositional_variable node
  /// \param v A propositional variable instantiation
  /// \return The result of visiting the node
  pbes_expression visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
  {
    throw mcrl2::runtime_error(std::string("complement_builder error: unexpected propositional variable encountered ") + mcrl2::core::pp(v));
    return pbes_expression();
  }
};
/// \endcond

/// \brief Returns the complement of a pbes expression
/// \param p A PBES expression
/// \return The expression obtained by pushing the negations in the pbes
/// expression as far as possible inwards towards a data expression.
inline
pbes_expression complement(const pbes_expression p)
{
  return complement_builder().visit(p);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_COMPLEMENT_H
