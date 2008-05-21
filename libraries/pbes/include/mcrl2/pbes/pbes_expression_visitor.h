// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_visitor.h
/// \brief Visitor class for pbes expressions.

#ifndef MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
#define MCRL2_PBES_PBES_EXPRESSION_VISITOR_H

#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace pbes_system {

/// A visitor class for pbes expressions. There is a visit_<node> and a leave_<node>
/// function for each type of node. By default these functions do nothing, so they
/// must be overridden to add behavior. If the visit_<node> function returns true,
/// the recursion is continued in the children of the node.
struct pbes_expression_visitor
{
  /// These names can be used as return types of the visit functions, to make
  /// the code more readible.
  ///
  enum return_type
  {
    stop_recursion = false,
    continue_recursion = true
  };
  
  /// Destructor.
  ///
  virtual ~pbes_expression_visitor()
  { }
  
  /// Visit data expression node.
  ///
  virtual bool visit_data_expression(const pbes_expression& e, const data::data_expression& d)
  {
    return continue_recursion;
  }

  /// Leave data expression node.
  ///
  virtual void leave_data_expression()
  {}

  /// Visit true node.
  ///
  virtual bool visit_true(const pbes_expression& e)
  {
    return continue_recursion;
  }

  /// Leave true node.
  ///
  virtual void leave_true()
  {}

  /// Visit false node.
  ///
  virtual bool visit_false(const pbes_expression& e)
  {
    return continue_recursion;
  }

  /// Leave false node.
  ///
  virtual void leave_false()
  {}

  /// Visit not node.
  ///
  virtual bool visit_not(const pbes_expression& e, const pbes_expression& /* arg */)
  {
    return continue_recursion;
  }

  /// Leave not node.
  ///
  virtual void leave_not()
  {}

  /// Visit and node.
  ///
  virtual bool visit_and(const pbes_expression& e, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return continue_recursion;
  }

  /// Leave and node.
  ///
  virtual void leave_and()
  {}

  /// Visit or node.
  ///
  virtual bool visit_or(const pbes_expression& e, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return continue_recursion;
  }    

  /// Leave or node.
  ///
  virtual void leave_or()
  {}

  /// Visit imp node.
  ///
  virtual bool visit_imp(const pbes_expression& e, const pbes_expression& /* left */, const pbes_expression& /* right */)
  {
    return continue_recursion;
  }

  /// Leave imp node.
  ///
  virtual void leave_imp()
  {}

  /// Visit forall node.
  ///
  virtual bool visit_forall(const pbes_expression& e, const data::data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return continue_recursion;
  }

  /// Leave forall node.
  ///
  virtual void leave_forall()
  {}

  /// Visit exists node.
  ///
  virtual bool visit_exists(const pbes_expression& e, const data::data_variable_list& /* variables */, const pbes_expression& /* expression */)
  {
    return continue_recursion;
  }

  /// Leave exists node.
  ///
  virtual void leave_exists()
  {}

  /// Visit propositional variable node.
  ///
  virtual bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& /* v */)
  {
    return continue_recursion;
  }
  
  /// Leave propositional variable node.
  ///
  virtual void leave_propositional_variable()
  {}

  /// Visits the nodes of the pbes expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a member function equals false, then the
  /// recursion in this node is stopped.
  void visit(const pbes_expression& e)
  {
    using namespace pbes_expr_optimized;
    using namespace accessors;

    if (is_data(e)) {
      visit_data_expression(e, val(e));
      leave_data_expression();
    } else if (is_pbes_true(e)) {
      visit_true(e);
      leave_true();
    } else if (is_pbes_false(e)) {
      visit_false(e);
      leave_false();
    } else if (is_pbes_not(e)) {
      pbes_expression a = arg(e);
      bool result = visit_not(e, a);
      if (result) {
        visit(a);
      }
      leave_not();
    } else if (is_pbes_and(e)) {
      pbes_expression l = left(e);
      pbes_expression r = right(e);
      bool result = visit_and(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_and();
    } else if (is_pbes_or(e)) {
      pbes_expression l = left(e);
      pbes_expression r = right(e);
      bool result = visit_or(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_or();
    } else if (is_pbes_imp(e)) {
      pbes_expression l = left(e);
      pbes_expression r = right(e);
      bool result = visit_imp(e, l, r);
      if (result) {
        visit(l);
        visit(r);
      }
      leave_imp();
    } else if (is_pbes_forall(e)) {
      data::data_variable_list qvars = var(e);
      pbes_expression qexpr = arg(e);
      bool result = visit_forall(e, qvars, qexpr);
      if (result) {
        visit(qexpr);
      }
      leave_forall();
    } else if (is_pbes_exists(e)) {
      data::data_variable_list qvars = var(e);
      pbes_expression qexpr = arg(e);
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_VISITOR_H
