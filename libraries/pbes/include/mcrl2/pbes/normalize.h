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
    inside_not = false;
    return inside_not ? data_expr::not_(d)
                      : d;
  }

  pbes_expression visit_true(const pbes_expression& /* f */)
  {
    using namespace lps::pbes_expr;
    inside_not = false;
    return inside_not ? data_expr::false_()
                      : data_expr::true_();
  }

  pbes_expression visit_false(const pbes_expression& /* f */)
  {
    using namespace lps::pbes_expr;
    inside_not = false;
    return inside_not ? data_expr::true_()
                      : data_expr::false_();
  }

  pbes_expression visit_not(const pbes_expression& /* f */, const pbes_expression& arg )
  {
    using namespace lps::pbes_expr;
    inside_not = !inside_not;
    pbes_expression result = visit(arg);
    return result;
  }

  pbes_expression visit_and(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    bool b = inside_not;
    inside_not = false;
    return b ? or_(visit(not_(left)), visit(not_(right)))
             : and_(visit(left), visit(right));
  }

  pbes_expression visit_or(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    bool b = inside_not;
    inside_not = false;
    return b ? and_(visit(not_(left)), visit(not_(right)))
             : or_(visit(left), visit(right));
  }    

  pbes_expression visit_imp(const pbes_expression& /* f */, const pbes_expression& left, const pbes_expression& right)
  {
    using namespace lps::pbes_expr;
    bool b = inside_not;
    inside_not = false;
    return b ? and_(visit(left), visit(not_(right)))
             : or_(visit(not_(left)), visit(right));
  }    

  pbes_expression visit_forall(const pbes_expression& /* f */, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace lps::pbes_expr;
    bool b = inside_not;
    inside_not = false;
    return b ? exists(variables, visit(not_(expression)))
             : forall(variables, visit(expression));
  }

  pbes_expression visit_exists(const pbes_expression& /* f */, const data_variable_list& variables, const pbes_expression& expression)
  {
    using namespace lps::pbes_expr;
    bool b = inside_not;
    inside_not = false;
    return b ? forall(variables, visit(not_(expression)))
             : exists(variables, visit(expression));
  }

  pbes_expression visit_propositional_variable(const pbes_expression& f, const propositional_variable_instantiation& /* v */)
  {
    using namespace lps::pbes_expr;
    inside_not = false;
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
pbes_expression normalize(pbes_expression f)
{
  using namespace lps::pbes_expr;

  if (is_not(f))
  {
    f = not_arg(f); // remove the not
    if (is_data(f)) {
      return data_expr::not_(f);
    } else if (is_true(f)) {
      return false_();
    } else if (is_false(f)) {
      return true_();
    } else if (is_not(f)) {
      return normalize(not_arg(f));
    } else if (is_and(f)) {
      return or_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_or(f)) {
      return and_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_imp(f)) {
      return and_(normalize(lhs(f)), normalize(not_(rhs(f))));
    } else if (is_forall(f)) {
      return exists(quant_vars(f), normalize(not_(quant_expr(f))));
    } else if (is_exists(f)) {
      return forall(quant_vars(f), normalize(not_(quant_expr(f))));
    } else if (is_propositional_variable_instantiation(f)) {
      throw std::runtime_error(std::string("normalize error: illegal argument ") + f.to_string());
    }
  }
  else // !is_not(f)
  {
    if (is_data(f)) {
      return f;
    } else if (is_true(f)) {
      return f;
    } else if (is_false(f)) {
      return f;
    //} else if (is_not(f)) {
    // ;
    } else if (is_and(f)) {
      return and_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_or(f)) {
      return or_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_imp(f)) {
      return or_(normalize(not_(lhs(f))), normalize(rhs(f)));
    } else if (is_forall(f)) {
      return forall(quant_vars(f), normalize(quant_expr(f)));
    } else if (is_exists(f)) {
      return exists(quant_vars(f), normalize(quant_expr(f)));
    } else if (is_propositional_variable_instantiation(f)) {
      return f;
    }
  }
  throw std::runtime_error(std::string("normalize error: unknown argument ") + f.to_string());
  return pbes_expression();
}

/// The function normalize brings a pbes expression into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \ret The result of the normalization.
inline
pbes_expression normalize_old(const pbes_expression& f)
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
