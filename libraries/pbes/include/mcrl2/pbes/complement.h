// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file complement.h
/// \brief Add your file description here.
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
#include "mcrl2/pbes/detail/sequence_substitution.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace lps {

/// Visitor that pushes a negation in a PBES expression as far as possible
/// inwards towards a data expression.
struct complement_builder: public pbes_expression_builder
{
  pbes_expression visit_data_expression(const pbes_expression& /* e */, const data_expression& d)
  {
    return data_expr::not_(d);
  }

  pbes_expression visit_true(const pbes_expression& /* e */)
  {
    return pbes_expr::false_();;
  }

  pbes_expression visit_false(const pbes_expression& /* e */)
  {
    return pbes_expr::true_();;
  }

  pbes_expression visit_and(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
  {
    return pbes_expr::or_(visit(left), visit(right));
  }

  pbes_expression visit_or(const pbes_expression& /* e */, const pbes_expression& left, const pbes_expression& right)
  {
    return pbes_expr::and_(visit(left), visit(right));
  }    

  pbes_expression visit_forall(const pbes_expression& /* e */, const data_variable_list& variables, const pbes_expression& expression)
  {
    return pbes_expr::exists(variables, visit(expression));
  }

  pbes_expression visit_exists(const pbes_expression& /* e */, const data_variable_list& variables, const pbes_expression& expression)
  {
    return pbes_expr::forall(variables, visit(expression));
  }

  pbes_expression visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
  {
    throw std::runtime_error(std::string("complement_builder error: unexpected propositional variable encountered ") + pp(v));
    return pbes_expression();
  }
};

/// Returns the expression obtained by pushing the negations in the pbes
/// expression as far as possible inwards towards a data expression.
inline
pbes_expression complement(const pbes_expression p)
{
  return complement_builder().visit(p);
}

} // namespace lps

#endif // MCRL2_PBES_COMPLEMENT_H
