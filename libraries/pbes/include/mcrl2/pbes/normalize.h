// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normalize.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_NORMALIZE_H
#define MCRL2_PBES_NORMALIZE_H

#include "mcrl2/pbes/pbes_expression_visitor.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/pbes/pbes_equation.h"

namespace lps {

/// \internal
/// Puts a logical not around propositional variables named X
struct propositional_variable_negation
{
  identifier_string X;
  
  propositional_variable_negation(identifier_string X_)
    : X(X_)
  {}
  
  aterm_appl operator()(aterm_appl t) const
  {
    using namespace lps::pbes_expr;

    if (is_propositional_variable_instantiation(t) && (var_name(t) == X))
    {
      return not_(t);
    }
    else
    {
      return t;
    }
  }
};

/// Visitor that normalizes a pbes expression.
struct pbes_expression_normalize_builder: public pbes_expression_builder
{
  bool inside_not;
  
  pbes_expression_normalize_builder()
    : inside_not(false)
  {}

  pbes_expression visit_data_expression(const pbes_expression& /* f */, const data_expression& d)
  {
    using namespace lps::pbes_expr;
    return inside_not ? data_expr::not_(d)
                      : d;
  }

  pbes_expression visit_true(const pbes_expression& /* f */)
  {
    using namespace lps::pbes_expr;
    return inside_not ? data_expr::false_()
                      : data_expr::true_();
  }

  pbes_expression visit_false(const pbes_expression& /* f */)
  {
    using namespace lps::pbes_expr;
    return inside_not ? data_expr::true_()
                      : data_expr::false_();
  }

  pbes_expression visit_not(const pbes_expression& /* f */, const pbes_expression& arg )
  {
    using namespace lps::pbes_expr;
    inside_not = !inside_not;
    pbes_expression result = visit(arg);
    inside_not = !inside_not;
    return result;
  }

  pbes_expression visit_and(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    return inside_not ? or_(visit(not_(left)), visit(not_(right)))
                      : and_(visit(left), visit(right));
  }

  pbes_expression visit_or(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    return inside_not ? and_(visit(not_(left)), visit(not_(right)))
                      : or_(visit(left), visit(right));
  }    

  pbes_expression visit_imp(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    return inside_not ? and_(visit(left), visit(not_(right)))
                      : or_(visit(not_(left)), visit(right));
  }    

  pbes_expression visit_forall(const pbes_expression& /* f */, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace lps::pbes_expr;
    return inside_not ? exists(variables, visit(not_(expression)))
                      : forall(variables, visit(expression));
  }

  pbes_expression visit_exists(const pbes_expression& /* f */, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace lps::pbes_expr;
    return inside_not ? forall(variables, visit(not_(expression)))
                      : exists(variables, visit(expression));
  }

  pbes_expression visit_var(const pbes_expression& f, const identifier_string& /* n */, const data_expression_list& /* l */)
  {
    using namespace lps::pbes_expr;
    if (inside_not)
    {
      throw std::runtime_error(std::string("normalize error: illegal expression ") + f.to_string());
    }
    return f;
  }
};

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
/// \ret The result of the normalization.
inline
pbes_expression normalize(const pbes_expression& f)
{
  return pbes_expression_normalize_builder().visit(f);
}

/// Applies normalization to the right hand side of the equation.
/// \ret The result of the normalization.
/// 
inline
pbes_equation normalize(const pbes_equation& e)
{
  return pbes_equation(e.symbol(), e.variable(), normalize(e.formula()));
}

} // namespace lps

#endif // MCRL2_PBES_NORMALIZE_H
