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
#ifndef PPG_VISITOR_H_
#define PPG_VISITOR_H_

#include "bqnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class ppg_visitor
  {
  public:
    typedef pbes_expression term_type;
    typedef core::term_traits<pbes_expression> tr;
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
    static std::string print_brief(const term_type& e)
    {
      return bqnf::print_brief(e);
    }

    /// \brief Visits a simple expression.
    /// An expression is simple if it does not contain propositional variables.
    /// \param e a PBES expression
    /// \return true if the expression e is a simple expression.
    virtual bool visit_simple_expression(const term_type& e)
    {
      //std::clog << "visit_simple_expression: " << print_brief(e) << std::endl;
      bool result = is_simple_expression(e);
      return result;
    }

    /// \brief Visits a propositional variable expression.
    /// \param e PBES expression
    /// \return true if the expression is a propositional variable or a simple expression.
    virtual bool visit_propositional_variable(const term_type& e)
    {
      //std::clog << "visit_propositional_variable: " << print_brief(e) << std::endl;
      bool result = true;
      if (!(tr::is_prop_var(e) || visit_simple_expression(e))) {
        //std::clog << "Not a propositional variable or simple expression!" << std::endl;
        result = false;
      }
      return result;
    }

    /// \brief Visits a conjunctive expression within an inner existential quantifier expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_inner_and(const term_type& e)
    {
      //std::clog << "visit_inner_and: " << print_brief(e) << std::endl;
      bool result = true;
      if (tr::is_and(e)) {
        term_type l = pbes_system::accessors::left(e);
        term_type r = pbes_system::accessors::right(e);
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
    virtual bool visit_inner_bounded_exists(const term_type& e)
    {
      //std::clog << "visit_inner_bounded_exists: " << print_brief(e) << std::endl;
      term_type qexpr = e;
      data::variable_list qvars;
      while (tr::is_exists(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      return visit_inner_and(qexpr);
    }

    /// \brief Visits a disjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_or(const term_type& e)
    {
      //std::clog << "visit_or: " << print_brief(e) << std::endl;
      bool result = true;
      if (tr::is_or(e)) {
        term_type l = pbes_system::accessors::left(e);
        term_type r = pbes_system::accessors::right(e);
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
    virtual bool visit_inner_implies(const term_type& e)
    {
      //std::clog << "visit_inner_implies: " << print_brief(e) << std::endl;
      bool result = true;
      if (tr::is_or(e) || tr::is_imp(e)) {
        term_type l = pbes_system::accessors::left(e);
        term_type r = pbes_system::accessors::right(e);
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
    virtual bool visit_inner_bounded_forall(const term_type& e)
    {
      //std::clog << "visit_inner_bounded_forall: " << print_brief(e) << std::endl;
      term_type qexpr = e;
      data::variable_list qvars;
      while (tr::is_forall(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      return visit_inner_implies(qexpr);
    }

    /// \brief Visits a conjunctive expression.
    /// \param e a PBES expression
    /// \return true if the expression e conforms to PPG.
    virtual bool visit_and(const term_type& e)
    {
      //std::clog << "visit_and: " << print_brief(e) << std::endl;
      bool result = true;
      if (tr::is_and(e)) {
        term_type l = pbes_system::accessors::left(e);
        term_type r = pbes_system::accessors::right(e);
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
    virtual bool visit_ppg_expression(const term_type& e)
    {
      //std::clog << "visit_ppg_expression." << std::endl;
      bool result = true;
      if (visit_propositional_variable(e)) {
        result = true;
      } else if (tr::is_and(e)) {
        result &= visit_inner_and(e) || visit_and(e);
      } else if (tr::is_or(e)) {
        result &= visit_inner_implies(e) || visit_or(e);
      } else if (tr::is_imp(e)) {
        result &= visit_inner_implies(e);
      } else if (tr::is_forall(e)) {
        result &= visit_inner_bounded_forall(e);
      } else if (tr::is_exists(e)) {
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

#endif /* PPG_VISITOR_H_ */
