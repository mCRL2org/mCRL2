// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_builder.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_STATE_FORMULA_BUILDER_H
#define MCRL2_PBES_STATE_FORMULA_BUILDER_H

#include "mcrl2/exception.h"
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
/// Visitor class for visiting the nodes of a state formula expression. During traversal
/// of the nodes, the expression is rebuilt from scratch.
struct state_formula_builder
{
  virtual ~state_formula_builder()
  { }

  /// \brief Visit data_expression node
  /// \param d A data expression
  /// \return The result of visiting the node
  virtual state_formula visit_data_expression(const state_formula& /* e */, const new_data::data_expression& d)
  {
    return state_formula();
  }

  /// \brief Visit true node
  /// \return The result of visiting the node
  virtual state_formula visit_true(const state_formula& /* e */)
  {
    return state_formula();
  }

  /// \brief Visit false node
  /// \return The result of visiting the node
  virtual state_formula visit_false(const state_formula& /* e */)
  {
    return state_formula();
  }

  /// \brief Visit not node
  /// \return The result of visiting the node
  virtual state_formula visit_not(const state_formula& /* e */, const state_formula& /* arg */)
  {
    return state_formula();
  }

  /// \brief Visit and node
  /// \return The result of visiting the node
  virtual state_formula visit_and(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }

  /// \brief Visit or node
  /// \return The result of visiting the node
  virtual state_formula visit_or(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }

  /// \brief Visit imp node
  /// \return The result of visiting the node
  virtual state_formula visit_imp(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }

  /// \brief Visit forall node
  /// \return The result of visiting the node
  virtual state_formula visit_forall(const state_formula& /* e */, const new_data::variable_list& /* variables */, const state_formula& /* expression */)
  {
    return state_formula();
  }

  /// \brief Visit exists node
  /// \return The result of visiting the node
  virtual state_formula visit_exists(const state_formula& /* e */, const new_data::variable_list& /* variables */, const state_formula& /* expression */)
  {
    return state_formula();
  }

  /// \brief Visit must node
  /// \return The result of visiting the node
  virtual state_formula visit_must(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return state_formula();
  }

  /// \brief Visit may node
  /// \return The result of visiting the node
  virtual state_formula visit_may(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return state_formula();
  }

  /// \brief Visit yaled node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled(const state_formula& /* e */)
  {
    return state_formula();
  }

  /// \brief Visit yaled_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_yaled_timed(const state_formula& /* e */, const new_data::data_expression& /* d */)
  {
    return state_formula();
  }

  /// \brief Visit delay node
  /// \return The result of visiting the node
  virtual state_formula visit_delay(const state_formula& /* e */)
  {
    return state_formula();
  }

  /// \brief Visit delay_timed node
  /// \return The result of visiting the node
  virtual state_formula visit_delay_timed(const state_formula& /* e */, const new_data::data_expression& /* d */)
  {
    return state_formula();
  }

  /// \brief Visit var node
  /// \return The result of visiting the node
  virtual state_formula visit_var(const state_formula& /* e */, const core::identifier_string& /* n */, const new_data::data_expression_list& /* l */)
  {
    return state_formula();
  }

  /// \brief Visit mu node
  /// \return The result of visiting the node
  virtual state_formula visit_mu(const state_formula& /* e */, const core::identifier_string& /* n */, const new_data::assignment_list& /* a */, const state_formula& /* f */)
  {
    return state_formula();
  }

  /// \brief Visit nu node
  /// \return The result of visiting the node
  virtual state_formula visit_nu(const state_formula& /* e */, const core::identifier_string& /* n */, const new_data::assignment_list& /* a */, const state_formula& /* f */)
  {
    return state_formula();
  }

  /// \brief Visits the nodes of the state formula expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals state_formula(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  /// \param e A modal formula
  /// \return The visit result
  state_formula visit(const state_formula& e)
  {
    using namespace state_frm;

    if (is_data(e)) {
      state_formula result = visit_data_expression(e, val(e));
      return (result == state_formula()) ? e : result;
    } else if (is_true(e)) {
      state_formula result = visit_true(e);
      return (result == state_formula()) ? e : result;
    } else if (is_false(e)) {
      state_formula result = visit_false(e);
      return (result == state_formula()) ? e : result;
    } else if (is_not(e)) {
      state_formula n = arg(e);
      state_formula result = visit_not(e, n);
      return (result == state_formula()) ? not_(visit(n)) : result;
    } else if (is_and(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      state_formula result = visit_and(e, l, r);
      return (result == state_formula()) ? and_(visit(l), visit(r)) : result;
    } else if (is_or(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      state_formula result = visit_or(e, l, r);
      return (result == state_formula()) ? or_(visit(l), visit(r)) : result;
    } else if (is_imp(e)) {
      state_formula l = left(e);
      state_formula r = right(e);
      state_formula result = visit_imp(e, l, r);
      return (result == state_formula()) ? imp(visit(l), visit(r)) : result;
    } else if (is_forall(e)) {
      new_data::variable_list qvars = var(e);
      state_formula qexpr = arg(e);
      state_formula result = visit_forall(e, qvars, qexpr);
      return (result == state_formula()) ? state_frm::forall(qvars, visit(qexpr)) : result;
    } else if (is_exists(e)) {
      new_data::variable_list qvars = var(e);
      state_formula qexpr = arg(e);
      state_formula result = visit_exists(e, qvars, qexpr);
      return (result == state_formula()) ? state_frm::exists(qvars, visit(qexpr)) : result;
    } else if(is_must(e)) {
      const regular_formula& r = act(e);
      state_formula s = arg(e);
      state_formula result = visit_must(e, r, s);
      return (result == state_formula()) ? must(r, visit(s)) : result;
    } else if(is_may(e)) {
      const regular_formula& r = act(e);
      state_formula s = arg(e);
      state_formula result = visit_may(e, r, s);
      return (result == state_formula()) ? may(r, visit(s)) : result;
    } else if (is_yaled(e)) {
      state_formula result = visit_yaled(e);
      return (result == state_formula()) ? e : result;
    } else if(is_yaled_timed(e)) {
      const new_data::data_expression& t = time(e);
      state_formula result = visit_yaled_timed(e, t);
      return (result == state_formula()) ? e : result;
    } else if (is_delay(e)) {
      state_formula result = visit_delay(e);
      return (result == state_formula()) ? e : result;
    } else if(is_delay_timed(e)) {
      const new_data::data_expression& t = time(e);
      state_formula result = visit_delay_timed(e, t);
      return (result == state_formula()) ? e : result;
    } else if(is_var(e)) {
      const core::identifier_string& n = name(e);
      const new_data::data_expression_list& l = param(e);
      state_formula result = visit_var(e, n, l);
      return (result == state_formula()) ? e : result;
    } else if(is_mu(e)) {
      const core::identifier_string& n = name(e);
      const new_data::assignment_list& a = ass(e);
      state_formula f = arg(e);
      state_formula result = visit_mu(e, n, a, f);
      return (result == state_formula()) ? mu(n, a, visit(f)) : result;
    } else if(is_nu(e)) {
      const core::identifier_string& n = name(e);
      const new_data::assignment_list& a = ass(e);
      state_formula f = arg(e);
      state_formula result = visit_nu(e, n, a, f);
      return (result == state_formula()) ? nu(n, a, visit(f)) : result;
    } else {
      throw mcrl2::runtime_error(std::string("error in state_formula_builder::visit() : unknown lps expression ") + e.to_string());
      return state_formula();
    }
  }
};

} // namespace modal

} // namespace mcrl2

#endif // MCRL2_PBES_STATE_FORMULA_BUILDER_H
