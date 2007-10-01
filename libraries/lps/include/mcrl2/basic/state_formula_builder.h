// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/state_formula_builder.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_STATE_FORMULA_BUILDER_H
#define MCRL2_PBES_STATE_FORMULA_BUILDER_H

#include <stdexcept>
#include "mcrl2/basic/state_formula.h"

namespace lps {

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
  
  virtual state_formula visit_data_expression(const state_formula& /* e */, const data_expression& d)
  {
    return state_formula();
  }

  virtual state_formula visit_true(const state_formula& /* e */)
  {
    return state_formula();
  }

  virtual state_formula visit_false(const state_formula& /* e */)
  {
    return state_formula();
  }

  virtual state_formula visit_not(const state_formula& /* e */, const state_formula& /* arg */)
  {
    return state_formula();
  }

  virtual state_formula visit_and(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }

  virtual state_formula visit_or(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }    

  virtual state_formula visit_imp(const state_formula& /* e */, const state_formula& /* left */, const state_formula& /* right */)
  {
    return state_formula();
  }    

  virtual state_formula visit_forall(const state_formula& /* e */, const data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return state_formula();
  }

  virtual state_formula visit_exists(const state_formula& /* e */, const data_variable_list& /* variables */, const state_formula& /* expression */)
  {
    return state_formula();
  }

  virtual state_formula visit_must(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return state_formula();
  }

  virtual state_formula visit_may(const state_formula& /* e */, const regular_formula& /* r */, const state_formula& /* f */)
  {
    return state_formula();
  }

  virtual state_formula visit_yaled(const state_formula& /* e */)
  {
    return state_formula();
  }

  virtual state_formula visit_yaled_timed(const state_formula& /* e */, const data_expression& /* d */)
  {
    return state_formula();
  }

  virtual state_formula visit_delay(const state_formula& /* e */)
  {
    return state_formula();
  }

  virtual state_formula visit_delay_timed(const state_formula& /* e */, const data_expression& /* d */)
  {
    return state_formula();
  }

  virtual state_formula visit_var(const state_formula& /* e */, const identifier_string& /* n */, const data_expression_list& /* l */)
  {
    return state_formula();
  }

  virtual state_formula visit_mu(const state_formula& /* e */, const identifier_string& /* n */, const data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return state_formula();
  }

  virtual state_formula visit_nu(const state_formula& /* e */, const identifier_string& /* n */, const data_assignment_list& /* a */, const state_formula& /* f */)
  {
    return state_formula();
  }

  /// Visits the nodes of the state formula expression, and calls the corresponding visit_<node>
  /// member functions. If the return value of a visit function equals state_formula(),
  /// the recursion in this node is continued automatically, otherwise the returned
  /// value is used for rebuilding the expression.
  state_formula visit(const state_formula& e)
  {
    using namespace state_frm;

    if (is_data(e)) {
      state_formula result = visit_data_expression(e, data_arg(e));
      return (result == state_formula()) ? e : result;
    } else if (is_true(e)) {
      state_formula result = visit_true(e);
      return (result == state_formula()) ? e : result;
    } else if (is_false(e)) {
      state_formula result = visit_false(e);
      return (result == state_formula()) ? e : result;
    } else if (is_not(e)) {
      const state_formula& arg = not_arg(e);
      state_formula result = visit_not(e, arg);
      return (result == state_formula()) ? not_(visit(arg)) : result;
    } else if (is_and(e)) {
      const state_formula& left  = lhs(e);
      const state_formula& right = rhs(e);
      state_formula result = visit_and(e, left, right);
      return (result == state_formula()) ? and_(visit(left), visit(right)) : result;
    } else if (is_or(e)) {
      const state_formula& left  = lhs(e);
      const state_formula& right = rhs(e);
      state_formula result = visit_or(e, left, right);
      return (result == state_formula()) ? or_(visit(left), visit(right)) : result;
    } else if (is_imp(e)) {
      const state_formula& left  = lhs(e);
      const state_formula& right = rhs(e);
      state_formula result = visit_imp(e, left, right);
      return (result == state_formula()) ? or_(visit(left), visit(right)) : result;
    } else if (is_forall(e)) {
      const data_variable_list& qvars = quant_vars(e);
      const state_formula& qexpr = quant_form(e);
      state_formula result = visit_forall(e, qvars, qexpr);
      return (result == state_formula()) ? forall(qvars, visit(qexpr)) : result;
    } else if (is_exists(e)) {
      const data_variable_list& qvars = quant_vars(e);
      const state_formula& qexpr = quant_form(e);
      state_formula result = visit_exists(e, qvars, qexpr);
      return (result == state_formula()) ? exists(qvars, visit(qexpr)) : result;
    } else if(is_must(e)) {
      const regular_formula& r = mod_act(e);
      const state_formula& s = mod_form(e);
      state_formula result = visit_must(e, r, s);
      return (result == state_formula()) ? must(r, visit(s)) : result;
    } else if(is_may(e)) {
      const regular_formula& r = mod_act(e);
      const state_formula& s = mod_form(e);
      state_formula result = visit_may(e, r, s);
      return (result == state_formula()) ? may(r, visit(s)) : result;
    } else if (is_yaled(e)) {
      state_formula result = visit_yaled(e);
      return (result == state_formula()) ? e : result;
    } else if(is_yaled_timed(e)) {
      const data_expression& t = time(e);
      state_formula result = visit_yaled_timed(e, t);
      return (result == state_formula()) ? e : result;
    } else if (is_delay(e)) {
      state_formula result = visit_delay(e);
      return (result == state_formula()) ? e : result;
    } else if(is_delay_timed(e)) {
      const data_expression& t = time(e);
      state_formula result = visit_delay_timed(e, t);
      return (result == state_formula()) ? e : result;
    } else if(is_var(e)) {
      const identifier_string& n = var_name(e);
      const data_expression_list& l = var_val(e);
      state_formula result = visit_var(e, n, l);
      return (result == state_formula()) ? e : result;
    } else if(is_mu(e)) {
      const identifier_string& n = mu_name(e);
      const data_assignment_list& a = mu_params(e);
      const state_formula& f = mu_form(e);
      state_formula result = visit_mu(e, n, a, f);
      return (result == state_formula()) ? mu(n, a, visit(f)) : result;
    } else if(is_nu(e)) {
      const identifier_string& n = mu_name(e);
      const data_assignment_list& a = mu_params(e);
      const state_formula& f = mu_form(e);
      state_formula result = visit_nu(e, n, a, f);
      return (result == state_formula()) ? nu(n, a, visit(f)) : result;
    } else {
      throw std::runtime_error(std::string("error in state_formula_builder::visit() : unknown lps expression ") + e.to_string());
      return state_formula();
    }
  }
};

} // namespace lps

#endif // MCRL2_PBES_STATE_FORMULA_BUILDER_H
