// Author(s): Simona Orzan.
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normal_forms.h



#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/pbes_equation.h"

using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;
using namespace mcrl2::pbes_system::accessors;


//auxiliary function that composes two prenex forms into a new prenex form
static pbes_expression join
(pbes_expression left, pbes_expression right, bool op)
{
  using namespace pbes_expr;

  if (is_forall(left))
    return (pbes_expr::forall(var(left),join(arg(left),right,op)));
  if (is_exists(left))
    return (exists(var(left),join(arg(left),right,op)));
  if (is_forall(right))
    return (pbes_expr::forall(var(right),join(left,arg(right),op)));
  if (is_exists(right))
    return (pbes_expr::exists(var(right),join(left,arg(right),op)));

  return (op?pbes_expr::and_(left,right):pbes_expr::or_(left,right));
}

/// Visitor that transforms a pbes expression to a prenex normal form
/// (i.e., all quantifiers pulled to the beginning of the expression).
/// Useful if quantifiers are problematic, for instance with the prover.
///
/// Precondition for the input pbes_expression:
/// - normalized form (no ! or ->)
/// - no variable should occur both free and bounded!
struct pbes_expression_prenex_builder: public pbes_expression_builder<pbes_expression>
{

  pbes_expression_prenex_builder(){}

  pbes_expression visit_data_expression
  (const pbes_expression& f, const data_expression& d)
  {
    return f;
  }

  pbes_expression visit_true
  (const pbes_expression& f)
  {
  return pbes_expr::true_();
  }

  pbes_expression visit_false
  (const pbes_expression& f)
  {
  return pbes_expr::false_();
  }

  pbes_expression visit_not
  (const pbes_expression&  f, const pbes_expression& arg )
  {
    throw mcrl2::runtime_error(std::string("prenex error: illegal expression ") + f.to_string());
    return f;
  }

  pbes_expression visit_and
  (const pbes_expression& f, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace pbes_expr;
    pbes_expression pleft = visit(left); // prenex form left
    pbes_expression pright = visit(right); // prenex form right
    return (join(pleft,pright,true));
  }

  pbes_expression visit_or
  (const pbes_expression& f, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace pbes_expr;
    pbes_expression pleft = visit(left); // prenex form left
    pbes_expression pright = visit(right); // prenex form right
    return (join(pleft,pright,false));
  }

  pbes_expression visit_imp
  (const pbes_expression& f, const pbes_expression& left, const pbes_expression& right)
  {
    throw mcrl2::runtime_error(std::string("prenex error: illegal expression ") + f.to_string());
    return f;
  }

  pbes_expression visit_forall
  (const pbes_expression& f, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace pbes_expr;
    return forall(variables, visit(expression));
  }

  pbes_expression visit_exists
  (const pbes_expression& f, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace pbes_expr;
    return exists(variables, visit(expression));
  }

  pbes_expression visit_var
  (const pbes_expression& f, const identifier_string& /* n */, const data_expression_list& /* l */)
  {
    using namespace pbes_expr;
    return f;
  }
};



/// This function brings a pbes expression into PRENEX normal form,
/// i.e. an expression with all quantifiers pulled to the beginning.
/// It assumes as input a normalized pbes expression,
/// where no variable occurs both free and bounded,
/// and no variable occurs bounded twice.
/// \return The PRENEX normal form.
inline
pbes_expression pbes_expression_to_prenex(const pbes_expression& f)
{
  return pbes_expression_prenex_builder().visit(f);
}
