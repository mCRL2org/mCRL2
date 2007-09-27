// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
#define MCRL2_PBES_PBES_EXPRESSION_VISITOR_H

#include "mcrl2/pbes/pbes_expression.h"

namespace lps {

struct pbes_expression_visitor
{
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };
  
  virtual ~pbes_expression_visitor()
  { }
  
  virtual bool visit_data_expression(const pbes_expression& e, const data_expression& d)
  {
    return true;
  }

  virtual void leave_data_expression()
  {}

  virtual bool visit_true(const pbes_expression& e)
  {
    return true;
  }

  virtual void leave_true()
  {}

  virtual bool visit_false(const pbes_expression& e)
  {
    return true;
  }

  virtual void leave_false()
  {}

  virtual bool visit_and(const pbes_expression& e, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return true;
  }

  virtual void leave_and()
  {}

  virtual bool visit_or(const pbes_expression& e, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return true;
  }    

  virtual void leave_or()
  {}

  virtual bool visit_forall(const pbes_expression& e, const data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return true;
  }

  virtual void leave_forall()
  {}

  virtual bool visit_exists(const pbes_expression& e, const data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return true;
  }

  virtual void leave_exists()
  {}

  virtual bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& /* v */)
  {
    return true;
  }
  
  virtual void leave_propositional_variable()
  {}

  /// Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  void visit(const pbes_expression& e)
  {
    using namespace pbes_expr;

    if (is_data(e)) {
      visit_data_expression(e, val(e));
      leave_data_expression();
    } else if (is_true(e)) {
      visit_true(e);
      leave_true();
    } else if (is_false(e)) {
      visit_false(e);
      leave_false();
    } else if (is_and(e)) {
      const pbes_expression& left  = lhs(e);
      const pbes_expression& right = rhs(e);
      bool result = visit_and(e, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_and();
    } else if (is_or(e)) {
      const pbes_expression& left  = lhs(e);
      const pbes_expression& right = rhs(e);
      bool result = visit_or(e, left, right);
      if (result) {
        visit(left);
        visit(right);
      }
      leave_or();
    } else if (is_forall(e)) {
      const data_variable_list& qvars = quant_vars(e);
      const pbes_expression& qexpr = quant_expr(e);
      bool result = visit_forall(e, qvars, qexpr);
      if (result) {
        visit(qexpr);
      }
      leave_forall();
    } else if (is_exists(e)) {
      const data_variable_list& qvars = quant_vars(e);
      const pbes_expression& qexpr = quant_expr(e);
      bool result = visit_exists(e, qvars, qexpr);
      if (result) {
        visit(qexpr);
      }
      leave_exists();
    }
    else if(is_propositional_variable_instantiation(e)) {
      visit_propositional_variable(e, propositional_variable_instantiation(e));
      leave_propositional_variable();
    }
  }
};

} // namespace lps

#endif // MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
