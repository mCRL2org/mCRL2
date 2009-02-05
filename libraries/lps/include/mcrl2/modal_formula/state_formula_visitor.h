// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_STATE_FORMULA_VISITOR_H
#define MCRL2_MODAL_STATE_FORMULA_VISITOR_H

#include <stdexcept>
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace modal {

//<StateFrm>     ::= <DataExpr>
//                 | StateTrue
//                 | StateFalse
//                 | StateNot(<StateFrm>)
//                 | StateAnd(<StateFrm>, <StateFrm>)
//                 | StateOr(<StateFrm>, <StateFrm>)
//                 | StateImp(<StateFrm>, <StateFrm>)
//                 | StateForall(<DataVarId>+, <StateFrm>)
//                 | StateExists(<DataVarId>+, <StateFrm>)
//                 | StateMust(<RegFrm>, <StateFrm>)
//                 | StateMay(<RegFrm>, <StateFrm>)
//                 | StateYaled
//                 | StateYaledTimed(<DataExpr>)
//                 | StateDelay
//                 | StateDelayTimed(<DataExpr>)
//                 | StateVar(<String>, <DataExpr>*)
//                 | StateNu(<String>, <DataVarIdInit>*, <StateFrm>)
//                 | StateMu(<String>, <DataVarIdInit>*, <StateFrm>)
//
/// Visitor class for visiting the nodes of a state formula expression.
struct state_formula_visitor
{
  virtual ~state_formula_visitor()
  { }

  /// \brief Visit data_expression node
  /// \param d A data expression
  /// \return The result of visiting the node
  virtual bool visit_data_expression(const state_formula& /* e */, const data::data_expression& d)
  {
    return true;
  }

  /// \brief Visit true node
  /// \return The result of visiting the node
  virtual bool visit_true(const state_formula& /* e */)
  {
    return true;
  }

  /// \brief Visit false node
  /// \return The result of visiting the node
  virtual bool visit_false(const state_formula& /* e */)
  {
    return true;
  }

  /// \brief Visit not node
  /// \return The result of visiting the node
  virtual bool visit_not(const state_formula& /* e */, const state_formula& /* arg */)
  {
    return true;
  }

  /// \brief Visit and node
  /// \return The result of visiting the node
  virtual bool visit_and(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }

  /// \brief Visit or node
  /// \return The result of visiting the node
  virtual bool visit_or(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }

  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual bool visit_imp(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }

  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual bool visit_forall(const state_formula& /* e */, const data::data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return true;
  }

  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual bool visit_exists(const state_formula& /* e */, const data::data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return true;
  }

  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual bool visit_must(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return true;
  }

  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual bool visit_may(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return true;
  }

  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual bool visit_yaled(const state_formula& /* e */)
  {
    return true;
  }

  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual bool visit_yaled_timed(const state_formula& /* e */, const data::data_expression& /* d */)
  {
    return true;
  }

  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual bool visit_delay(const state_formula& /* e */)
  {
    return true;
  }

  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual bool visit_delay_timed(const state_formula& /* e */, const data::data_expression& /* d */)
  {
    return true;
  }

  /// \brief Visit var node
  /// \return The result of visiting the node
  virtual bool visit_var(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_expression_list& /* l */)
  {
    return true;
  }

  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual bool visit_mu(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return true;
  }

  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual bool visit_nu(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return true;
  }

  /// \brief Leave data_expression node
  virtual void leave_data_expression()
  {}

  /// \brief Leave true node
  virtual void leave_true()
  {}

  /// \brief Leave false node
  virtual void leave_false()
  {}

  /// \brief Leave not node
  virtual void leave_not()
  {}

  /// \brief Leave and node
  virtual void leave_and()
  {}

  /// \brief Leave or node
  virtual void leave_or()
  {}

  /// \brief Leave imp node
  virtual void leave_imp()
  {}

  /// \brief Leave forall node
  virtual void leave_forall()
  {}

  /// \brief Leave exists node
  virtual void leave_exists()
  {}

  /// \brief Leave must node
  virtual void leave_must()
  {}

  /// \brief Leave may node
  virtual void leave_may()
  {}

  /// \brief Leave yaled node
  virtual void leave_yaled()
  {}

  /// \brief Leave yaled_timed node
  virtual void leave_yaled_timed()
  {}

  /// \brief Leave delay node
  virtual void leave_delay()
  {}

  /// \brief Leave delay_timed node
  virtual void leave_delay_timed()
  {}

  /// \brief Leave var node
  virtual void leave_var()
  {}

  /// \brief Leave mu node
  virtual void leave_mu()
  {}

  /// \brief Leave nu node
  virtual void leave_nu()
  {}

  /// \brief Visits the nodes of the state formula expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals state_formula(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param e A modal formula
  void visit(const state_formula& e)
  {
    using namespace state_frm;

    if (is_data(e)) {
      visit_data_expression(e, val(e));
      leave_data_expression();
    } else if (is_true(e)) {
      visit_true(e);
      leave_true();
    } else if (is_false(e)) {
      visit_false(e);
      leave_false();
    } else if (is_not(e)) {
      state_formula n = arg(e);
      bool result = visit_not(e, n);
      if (result)
      {
        visit(n);
      }
    } else if (is_and(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      bool result = visit_and(e, l, r);
      if (result)
      {
        visit(l);
        visit(r);
      }
    } else if (is_or(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      bool result = visit_or(e, l, r);
      if (result)
      {
        visit(l);
        visit(r);
      }
    } else if (is_imp(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      bool result = visit_imp(e, l, r);
      if (result)
      {
        visit(l);
        visit(r);
      }
    } else if (is_forall(e)) {
      data::data_variable_list qvars = var(e);
      state_formula qexpr = arg(e);
      bool result = visit_forall(e, qvars, qexpr);
      if (result)
      {
        visit(qexpr);
      }
    } else if (is_exists(e)) {
      data::data_variable_list qvars = var(e);
      state_formula qexpr = arg(e);
      bool result = visit_exists(e, qvars, qexpr);
      if (result)
      {
        visit(qexpr);
      }
    } else if(is_must(e)) {
      regular_formula r = act(e);
      state_formula s = arg(e);
      bool result = visit_must(e, r, s);
      if (result)
      {
        visit(s);
      }
    } else if(is_may(e)) {
      regular_formula r = act(e);
      state_formula s = arg(e);
      bool result = visit_may(e, r, s);
      if (result)
      {
        visit(s);
      }
    } else if (is_yaled(e)) {
      visit_yaled(e);
    } else if(is_yaled_timed(e)) {
      data::data_expression t = time(e);
      visit_yaled_timed(e, t);
    } else if (is_delay(e)) {
      visit_delay(e);
    } else if(is_delay_timed(e)) {
      data::data_expression t = time(e);
      visit_delay_timed(e, t);
    } else if(is_var(e)) {
      core::identifier_string n = name(e);
      data::data_expression_list l = param(e);
      visit_var(e, n, l);
    } else if(is_mu(e)) {
      core::identifier_string n = name(e);
      data::data_assignment_list a = ass(e);
      state_formula f = arg(e);
      bool result = visit_mu(e, n, a, f);
      if (result)
      {
        visit(f);
      }
    } else if(is_nu(e)) {
      core::identifier_string n = name(e);
      data::data_assignment_list a = ass(e);
      state_formula f = arg(e);
      bool result = visit_nu(e, n, a, f);
      if (result)
      {
        visit(f);
      }
    } else {
      throw mcrl2::runtime_error(std::string("error in state_formula_visitor::visit() : unknown lps expression ") + e.to_string());
    }
  }
};

} // namespace modal

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_FORMULA_VISITOR_H
