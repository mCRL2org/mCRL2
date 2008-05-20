// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
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
  
  virtual bool visit_data_expression(const state_formula& /* e */, const data::data_expression& d)
  {
    return true;
  }

  virtual bool visit_true(const state_formula& /* e */)
  {
    return true;
  }

  virtual bool visit_false(const state_formula& /* e */)
  {
    return true;
  }

  virtual bool visit_not(const state_formula& /* e */, const state_formula& /* arg */)
  {
    return true;
  }

  virtual bool visit_and(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }

  virtual bool visit_or(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }    

  virtual bool visit_imp(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return true;
  }    

  virtual bool visit_forall(const state_formula& /* e */, const data::data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return true;
  }

  virtual bool visit_exists(const state_formula& /* e */, const data::data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return true;
  }

  virtual bool visit_must(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return true;
  }

  virtual bool visit_may(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return true;
  }

  virtual bool visit_yaled(const state_formula& /* e */)
  {
    return true;
  }

  virtual bool visit_yaled_timed(const state_formula& /* e */, const data::data_expression& /* d */)
  {
    return true;
  }

  virtual bool visit_delay(const state_formula& /* e */)
  {
    return true;
  }

  virtual bool visit_delay_timed(const state_formula& /* e */, const data::data_expression& /* d */)
  {
    return true;
  }

  virtual bool visit_var(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_expression_list& /* l */)
  {
    return true;
  }

  virtual bool visit_mu(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return true;
  }

  virtual bool visit_nu(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return true;
  }

  virtual void leave_data_expression()
  {}

  virtual void leave_true()
  {}

  virtual void leave_false()
  {}

  virtual void leave_not()
  {}

  virtual void leave_and()
  {}

  virtual void leave_or()
  {}    

  virtual void leave_imp()
  {}    

  virtual void leave_forall()
  {}

  virtual void leave_exists()
  {}

  virtual void leave_must()
  {}

  virtual void leave_may()
  {}

  virtual void leave_yaled()
  {}

  virtual void leave_yaled_timed()
  {}

  virtual void leave_delay()
  {}

  virtual void leave_delay_timed()
  {}

  virtual void leave_var()
  {}

  virtual void leave_mu()
  {}

  virtual void leave_nu()
  {}

  /// Visits the nodes of the state formula expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals state_formula(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
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
      throw std::runtime_error(std::string("error in state_formula_visitor::visit() : unknown lps expression ") + e.to_string());
    }
  }
};

} // namespace modal

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_FORMULA_VISITOR_H
