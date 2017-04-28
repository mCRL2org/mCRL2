// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/ppg_visitor.h
/// \brief Visitor class for Parameterised Parity Games (PPG), PBES expressions of the form:
/// PPG :== /\_{i: I} f_i && /\_{j: J} forall v: D_j . ( g_j => X_j(e_j) )
///       | \/_{i: I} f_i || \/_{j: J} exists v: D_j . ( g_j && X_j(e_j) ).
///
/// DEPRECATED
#ifndef MCRL2_PBES_DETAIL_PPG_VISITOR_H
#define MCRL2_PBES_DETAIL_PPG_VISITOR_H

#include "mcrl2/pbes/detail/bqnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class ppg_visitor
  {
  public:
    /// \brief The BQNF visitor type.
    typedef bqnf_visitor bqnf;

    /// \brief Destructor.
    virtual ~ppg_visitor()
    { }

    /// \brief Constructor.
    ppg_visitor()
    { }

    /// \brief Returns a string representation of the type of the root node of the expression.
    /// \param e a PBES expression
    /// \return a string representation of the type of the root node of the expression.
    static std::string print_brief(const pbes_expression& e)
    {
      return bqnf::print_brief(e);
    }

    /// \brief Visits a simple expression.
    /// An expression is simple if it does not contain propositional variables.
    /// \param e a PBES expression
    /// \return true if the expression e is a simple expression.
    virtual bool visit_simple_expression(const pbes_expression& e)
    {
      //std::clog << "visit_simple_expression: " << print_brief(e) << std::endl;
      bool result = is_simple_expression(e);
      return result;
    }

    /// \brief Visits a propositional variable expression.
    /// \param e PBES expression
    /// \return true if the expression is a propositional variable or a simple expression.
    virtual bool visit_propositional_variable(const pbes_expression& e)
    {
      //std::clog << "visit_propositional_variable: " << print_brief(e) << std::endl;
      bool result = true;
      if (!(is_propositional_variable_instantiation(e) || visit_simple_expression(e))) {
        //std::clog << "Not a propositional variable or simple expression!" << std::endl;
        result = false;
      }
      return result;
    }

    /// \brief Visits a conjunctive expression within an inner existential quantifier expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_inner_and(const pbes_expression& e)
    {
      //std::clog << "visit_inner_and: " << print_brief(e) << std::endl;
      bool result = true;
      if (is_and(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        if (visit_simple_expression(l)) {
          result &= visit_inner_and(r);
        } else {
          result &= visit_propositional_variable(e);
        }
      } else {
        result &= visit_propositional_variable(e);
      }
      return result;
    }

    /// \brief Visits a bounded existential quantifier expression within a disjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_inner_bounded_exists(const pbes_expression& e)
    {
      //std::clog << "visit_inner_bounded_exists: " << print_brief(e) << std::endl;
      pbes_expression qexpr = e;
      data::variable_list qvars;
      while (is_exists(qexpr)) {
        qvars = qvars + quantifier_variables(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      return visit_inner_and(qexpr);
    }

    /// \brief Visits a disjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_or(const pbes_expression& e)
    {
      //std::clog << "visit_or: " << print_brief(e) << std::endl;
      bool result = true;
      if (is_or(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        result &= visit_or(l);
        result &= visit_or(r);
      } else {
        result &= visit_inner_bounded_exists(e);
      }
      return result;
    }

    /// \brief Visits a disjunctive expression within an inner universal quantifier expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_inner_implies(const pbes_expression& e)
    {
      //std::clog << "visit_inner_implies: " << print_brief(e) << std::endl;
      bool result = true;
      if (is_or(e) || is_imp(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        if (visit_simple_expression(l)) {
          result &= visit_inner_implies(r);
        } else {
          result &= visit_propositional_variable(e);
        }
      } else {
        result &= visit_propositional_variable(e);
      }
      return result;
    }

    /// \brief Visits a bounded universal quantifier expression within a conjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_inner_bounded_forall(const pbes_expression& e)
    {
      //std::clog << "visit_inner_bounded_forall: " << print_brief(e) << std::endl;
      pbes_expression qexpr = e;
      data::variable_list qvars;
      while (is_forall(qexpr)) {
        qvars = qvars + quantifier_variables(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      return visit_inner_implies(qexpr);
    }

    /// \brief Visits a conjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_and(const pbes_expression& e)
    {
      //std::clog << "visit_and: " << print_brief(e) << std::endl;
      bool result = true;
      if (is_and(e)) {
        pbes_expression l = pbes_system::accessors::left(e);
        pbes_expression r = pbes_system::accessors::right(e);
        result &= visit_and(l);
        result &= visit_and(r);
      } else {
        result &= visit_inner_bounded_forall(e);
      }
      return result;
    }

    /// \brief Visits a PPG expression.
    /// \param e a PBES expression
    /// \return true if the expression e is in PPG form.
    virtual bool visit_ppg_expression(const pbes_expression& e)
    {
      //std::clog << "visit_ppg_expression." << std::endl;
      bool result = true;
      if (visit_propositional_variable(e)) {
        result = true;
      } else if (is_and(e)) {
        result &= visit_inner_and(e) || visit_and(e);
      } else if (is_or(e)) {
        result &= visit_inner_implies(e) || visit_or(e);
      } else if (is_imp(e)) {
        result &= visit_inner_implies(e);
      } else if (is_forall(e)) {
        result &= visit_inner_bounded_forall(e);
      } else if (is_exists(e)) {
        result &= visit_inner_bounded_exists(e);
      } else {

      }
      //std::clog << "visit_ppg_expression: equation is " << (result ? "" : "NOT ") << "in PPG form." << std::endl;
      return result;
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PPG_VISITOR_H
