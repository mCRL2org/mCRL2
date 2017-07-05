// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bqnf_visitor.h
/// \brief Visitor class for PBESs in Bounded Quantifier Normal Form (BQNF):
/// BQNF :== forall d: D . b => BQNF  |  exists d: D . b && BQNF  |  CONJ
/// CONJ :== And_{k: K} f_k && And_{i: I} forall v: D_I . g_i => DISJ^i
/// DISJ^i :== Or_{l: L_i} f_{il} || Or_{j: J_i} exists w: D_{ij} . g_{ij} && X_{ij}(e_{ij})
///
/// DEPRECATED
#ifndef MCRL2_PBES_DETAIL_BQNF_VISITOR_H
#define MCRL2_PBES_DETAIL_BQNF_VISITOR_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_functions.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief The current indent level. Used for debug output.
static int indent_count = 0;
/// \brief Increases the current indent level.
static void inc_indent()
{
  indent_count++;
}
/// \brief Decreases the current indent level.
static void dec_indent()
{
  indent_count--;
}

/// \brief Indents according to the current indent level.
static void indent()
{
  for(int i=0;i<indent_count;i++)
  {
    std::clog << "  ";
  }
}

/// \brief A visitor class for PBES equations in BQNF. There is a visit_<node>
/// function for each type of node. By default these functions do nothing, but check
/// that the PBES equations are indeed in BQNF, so they
/// must be overridden in order to add behaviour.
struct bqnf_visitor
{
  /// \brief flag that indicates if debug output should be printed.
  bool debug;

  /// \brief Returns a string representation of the type of the root node of the expression.
  /// \param e a PBES expression
  /// \return a string representation of the type of the root node of the expression.
  static std::string print_brief(const pbes_expression& e)
  {
    if (is_propositional_variable_instantiation(e)) {
      return std::string("PropVar ") + std::string(atermpp::down_cast<propositional_variable_instantiation>(e).name());
    } else if (is_simple_expression(e)) {
      return "SimpleExpr";
    } else if (is_and(e)) {
      return "And";
    } else if (is_or(e)) {
      return "Or";
    } else if (is_imp(e)) {
      return "Imp";
    } else if (is_forall(e)) {
      return std::string("ForAll(")+core::pp(atermpp::down_cast<forall>(e).variables())+std::string(")");
    } else if (is_exists(e)) {
      return std::string("Exists(")+core::pp(atermpp::down_cast<exists>(e).variables())+std::string(")");
    } else {
      return "Unknown type";
    }
  }

  /// \brief Determines if an expression if of the form
  /// phi /\ psi where phi is a simple expression
  /// and psi is an arbitrary expression.
  /// \param e an expression
  /// \return true if e is of the form phi /\ psi.
  static bool is_inner_and(const pbes_expression& e)
  {
    bool result = true;
    if (!(is_propositional_variable_instantiation(e) || is_simple_expression(e))) {
      if (is_and(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        if (is_simple_expression(l)) {
          result = is_inner_and(r);
        } else {
          result = false;
        }
      } else {
        result = false;
      }
    }
    return result;
  }

  /// \brief Determines if an expression if of the form
  /// phi => psi or of the form phi \/  psi where phi is a simple expression
  /// and psi is an arbitrary expression.
  /// \param e an expression
  /// \return true if e is of the form phi => psi or phi \/ psi.
  static bool is_inner_implies(const pbes_expression& e)
  {
    bool result = true;
    if (!(is_propositional_variable_instantiation(e) || is_simple_expression(e))) {
      if (is_or(e) || is_imp(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        if (is_simple_expression(l)) {
          result = is_inner_implies(r);
        } else {
          result = false;
        }
      } else {
        result = false;
      }
    }
    return result;
  }

  /// \brief Destructor.
  virtual ~bqnf_visitor()
  { }

  /// \brief Constructor.
  bqnf_visitor():
    debug(false)
  { }

  /// \brief Visits a simple expression.
  /// An expression is simple if it does not contain propositional variables.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e is a simple expression.
  virtual bool visit_simple_expression(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    bool result = true;
    if (is_data(e) || is_true(e) || is_false(e)) {
      result = true;
    } else if (is_not(e)) {
      pbes_expression n = pbes_system::accessors::arg(e);
      result &= visit_simple_expression(sigma, var, n);
    } else if (is_and(e) || is_or(e) || is_imp(e)) {
      pbes_expression l = pbes_system::accessors::left(e);
      pbes_expression r = pbes_system::accessors::right(e);
      result &= visit_simple_expression(sigma, var, l);
      result &= visit_simple_expression(sigma, var, r);
    } else if (is_forall(e) || is_exists(e)) {
      pbes_expression a = pbes_system::accessors::arg(e);
      result &= visit_simple_expression(sigma, var, a);
    } else if (is_propositional_variable_instantiation(e)) {
      if (debug) {
        indent(); std::clog << "Not a simple expression!" << std::endl;
      } else {
        throw(std::runtime_error("Not a simple expression!"));
      }
    } else {
      throw(std::runtime_error("Unknown type of expression!"));
    }
    if (debug) {
      indent(); std::clog << "visit_simple_expression: " << pp(e) << ": " << (result?"true":"false") << std::endl;
    }
    return result;
  }

  /// \brief Visits a propositional variable expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e PBES expression
  /// \return true if the expression is a propositional variable or a simple expression.
  virtual bool visit_propositional_variable(const fixpoint_symbol& /*sigma*/, const propositional_variable& /*var*/, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_propositional_variable_instantiation(e) || is_simple_expression(e)) {
      // std::clog << pp(e) << std::endl;
    } else {
      result = false;
      if (debug) {
        indent(); std::clog << "Not a propositional variable or simple expression:" << core::pp(e) << std::endl;
      } else {
        throw(std::runtime_error("Not a propositional variable or simple expression!"));
      }
    }
    if (debug) {
      indent(); std::clog << "  visit_propositional_variable: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a conjunctive expression within an inner existential quantifier expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_inner_and(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_and(e)) {
      pbes_expression l = pbes_system::accessors::left(e);
      pbes_expression r = pbes_system::accessors::right(e);
      if (is_simple_expression(l)) {
        result &= visit_simple_expression(sigma, var, l);
        // std::clog << pp(l) << " /\\ " << std::endl;
        result &= visit_inner_and(sigma, var, r);
      } else {
        result &= visit_propositional_variable(sigma, var, e);
      }
    } else {
      result &= visit_propositional_variable(sigma, var, e);
    }
    if (debug) {
      indent(); std::clog << "  visit_inner_and: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a bounded existential quantifier expression within a disjunctive expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_inner_bounded_exists(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    pbes_expression qexpr = e;
    data::variable_list qvars;
    while (is_exists(qexpr)) {
      qvars = qvars + atermpp::down_cast<exists>(qexpr).variables();
      qexpr = pbes_system::accessors::arg(qexpr);
    }
    bool result = visit_inner_and(sigma, var, qexpr);
    if (debug) {
      indent(); std::clog << "visit_inner_bounded_exists: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a disjunctive expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_or(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_or(e) || is_imp(e)) {
      pbes_expression l = pbes_system::accessors::left(e);
      pbes_expression r = pbes_system::accessors::right(e);
      result &= visit_or(sigma, var, l);
      result &= visit_or(sigma, var, r);
    } else {
      result &= visit_inner_bounded_exists(sigma, var, e);
    }
    if (debug) {
      indent(); std::clog << "visit_or: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a bounded universal quantifier expression within a conjunctive expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_inner_bounded_forall(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    pbes_expression qexpr = e;
    data::variable_list qvars;
    while (is_forall(qexpr)) {
      qvars = qvars + atermpp::down_cast<forall>(qexpr).variables();
      qexpr = pbes_system::accessors::arg(qexpr);
    }
    bool result = true;
    if (is_or(qexpr) || is_imp(qexpr)) {
      pbes_expression l = pbes_system::accessors::left(qexpr);
      pbes_expression r = pbes_system::accessors::right(qexpr);
      if (is_simple_expression(l)) {
        result &= visit_simple_expression(sigma, var, l);
        // std::clog << pp(l) << (is_or(qexpr) ? " \\/ " : " => ") << std::endl;
        result &= visit_or(sigma, var, r);
      } else {
        result &= visit_or(sigma, var, qexpr);
      }
    } else {
      result &= visit_or(sigma, var, qexpr);
    }
    if (debug) {
      indent(); std::clog << "visit_inner_bounded_forall: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a conjunctive expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_and(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_and(e)) {
      pbes_expression l = pbes_system::accessors::left(e);
      pbes_expression r = pbes_system::accessors::right(e);
      result &= visit_and(sigma, var, l);
      // std::clog << " /\\ " << std::endl;
      result &= visit_and(sigma, var, r);
    } else {
      result &= visit_inner_bounded_forall(sigma, var, e);
    }
    if (debug) {
      indent(); std::clog << "visit_and: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a bounded universal quantifier expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_bounded_forall(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    assert(is_forall(e));
    pbes_expression qexpr = e;
    data::variable_list qvars;
    while (is_forall(qexpr)) {
      qvars = qvars + atermpp::down_cast<forall>(qexpr).variables();
      qexpr = pbes_system::accessors::arg(qexpr);
    }
    //std::clog << "  Bounded forall: " << pp(qvars) << " . " << std::endl;
    bool result = true;
    if (is_or(qexpr) || is_imp(qexpr)) {
      pbes_expression l = pbes_system::accessors::left(qexpr);
      pbes_expression r = pbes_system::accessors::right(qexpr);
      if (is_simple_expression(l)) {
        result &= visit_simple_expression(sigma, var, l);
        // std::clog << pp(l) << (is_or(qexpr) ? " \\/ " : " => ") << std::endl;
        result &= visit_bqnf_expression(sigma, var, r);
      } else {
        result &= visit_bqnf_expression(sigma, var, qexpr);
      }
    } else {
      result &= visit_bqnf_expression(sigma, var, qexpr);
    }
    if (debug) {
      indent(); std::clog << "visit_bounded_forall: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a bounded existential quantifier expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_bounded_exists(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    assert(is_exists(e));
    pbes_expression qexpr = e;
    data::variable_list qvars;
    while (is_exists(qexpr)) {
      qvars = qvars + atermpp::down_cast<exists>(qexpr).variables();
      qexpr = pbes_system::accessors::arg(qexpr);
    }
    bool result = true;
    if (is_and(qexpr)) {
      pbes_expression l = pbes_system::accessors::left(qexpr);
      pbes_expression r = pbes_system::accessors::right(qexpr);
      if (is_simple_expression(l)) {
        result &= visit_simple_expression(sigma, var, l);
        result &= visit_bqnf_expression(sigma, var, r);
      } else {
        result &= visit_bqnf_expression(sigma, var, qexpr);
      }
    } else {
      result &= visit_bqnf_expression(sigma, var, qexpr);
    }
    if (debug) {
      indent(); std::clog << "visit_bounded_exists: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a bounded quantifier expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e conforms to BQNF.
  virtual bool visit_bounded_quantifier(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_forall(e)) {
      result &= visit_bounded_forall(sigma, var, e);
    } else if (is_exists(e)) {
      result &= visit_bounded_exists(sigma, var, e);
    } else {
      // should not be possible
      throw(std::runtime_error("Not a quantifier expression!"));
    }
    if (debug) {
      indent(); std::clog << "visit_bounded_quantifier: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a BQNF expression.
  /// \param sigma fixpoint symbol (unused in this class)
  /// \param var propositional variable (unused in this class)
  /// \param e a PBES expression
  /// \return true if the expression e is in BQNF.
  virtual bool visit_bqnf_expression(const fixpoint_symbol& sigma, const propositional_variable& var, const pbes_expression& e)
  {
    inc_indent();
    bool result = true;
    if (is_simple_expression(e)) {
      result = true;
    } else if (is_forall(e) || is_exists(e)) {
      result &= visit_bounded_quantifier(sigma, var, e);
    } else {
      result &= visit_and(sigma, var, e);
    }
    if (debug) {
      indent(); std::clog << "visit_bqnf_expression: " << print_brief(e) << ": " << (result?"true":"false") << std::endl;
    }
    dec_indent();
    return result;
  }

  /// \brief Visits a BQNF expression.
  /// In the current BQNF visitor sigma and var parameters are added for use in bqnf2ppg_rewriter.
  /// This functions adds dummy values for sigma and var.
  /// \param e a PBES expression
  /// \return true if the expression e is in BQNF.
  virtual bool visit_bqnf_expression(const pbes_expression& e)
  {
    bool result = visit_bqnf_expression(fixpoint_symbol::nu(), propositional_variable("X"), e);
    return result;
  }

  /// \brief Visits a BQNF equation.
  /// \param eqn a PBES equation
  /// \return true if the right hand side of the equation is in BQNF.
  virtual bool visit_bqnf_equation(const pbes_equation& eqn)
  {
    if (debug) std::clog << "visit_bqnf_equation." << std::endl;
    const fixpoint_symbol& sigma = eqn.symbol();
    const propositional_variable& var = eqn.variable();
    const pbes_expression& e = eqn.formula();
    bool result = visit_bqnf_expression(sigma, var, e);
    if (debug) std::clog << "visit_bqnf_equation: equation " << var.name() << " is " << (result ? "" : "NOT ") << "in BQNF." << std::endl;
    return result;
  }

  /// \brief Visits a BQNF equation in debug mode.
  /// \param eqn a PBES equation
  /// \return true if the right hand side of the equation is in BQNF.
  virtual bool visit_bqnf_equation_debug(const pbes_equation& eqn)
  {
    debug = true;
    return visit_bqnf_equation(eqn);
  }

};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BQNF_VISITOR_H
