// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bqnf2ppg_rewriter.h
/// \brief Rewrites a PBES equation with expressions in Bounded Quantifier Normal Form (BQNF)
/// to (a sequence of ) Parameterised Parity Game (PPG) equations.
/// The transformation is described in [Kant & Van de Pol 2012].
/// Possibly new equations are introduced. The resulting sequence of equations is available
/// through the result() function.
///
/// DEPRECATED
#ifndef BQNF2PPG_REWRITER_H_
#define BQNF2PPG_REWRITER_H_

#include "bqnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct bqnf2ppg_rewriter: public bqnf_visitor
  {
    /// \brief The equation type.
    typedef pbes_equation equation_type;
    /// \brief The expression type of the equation.
    typedef pbes_expression term_type;
    typedef core::term_traits<pbes_expression> tr;

    /// \brief The set of variable names already in use.
    std::set<std::string> variable_names;
    /// \brief A map from variable name prefix to the last suffix that has been added.
    std::map<std::string,int> variable_name_suffix;
    /// \brief The sequence of equations resulting from the transformation.
    std::vector<equation_type> equations;

    /// \brief Constructor.
    /// \param p The PBES of which the equation is part. Used to avoid name clashes when introducing new variables.
    bqnf2ppg_rewriter(const pbes& p)
    {
      for (std::vector<equation_type>::const_iterator eqn = p.equations().begin(); eqn != p.equations().end(); ++eqn) {
        equation_type e = (*eqn);
        propositional_variable var = e.variable();
        variable_names.insert(core::pp(var.name()));
      }
    }

    /// \brief Returns the result of the transformation.
    /// \return the sequence of equations resulting from the transformation.
    virtual std::vector<equation_type> result() {
      return equations;
    }


    /// \brief Generates a fresh variable name, based on s (extending s).
    /// \param s A string.
    /// \return a fresh variable name.
    virtual std::string fresh_variable_name(const std::string& s) {
      //std::clog << "fresh_variable_name: ";
      std::string base; std::string name;
      name = base = s;
      int suffix = 1;
      while (variable_names.find(name)!=variable_names.end()) {
        if (variable_name_suffix.find(base) != variable_name_suffix.end()) {
          suffix = variable_name_suffix[base] + 1;
        }
        variable_name_suffix[base] = suffix;
        std::stringstream ss;
        ss << base << "_" << suffix;
        ss >> name;
      }
      variable_names.insert(name);
      //std::clog << name << std::endl;
      return name;
    }


    /// \brief Visits a disjunctive expression qexpr within a bounded universal quantifier expression e.
    /// Rewrites the expression qexpr to PPG form and adds a universal quantifier over the variables qvars.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e the parent PBES expression.
    /// \param qvars the quantifier variables of the quantifier in the parent expression.
    /// \param qexpr the PBES expression within the quantifier expression.
    /// \return true if the expression qexpr conforms to BQNF.
    virtual bool visit_inner_implies(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e, const data::variable_list& qvars, const term_type& qexpr)
    {
      bool result = true;
      inc_indent();
      //indent(); std::clog << "visit_inner_implies: " << print_brief(e) << std::endl;
      if (tr::is_prop_var(qexpr) || is_simple_expression(qexpr)) {
        // Eqn of the form sigma X(d: D) = Forall l: D . phi => Y(h(d, l)), with phi a simple formula.
        // Add sigma X(d) = e.
        equation_type eqn = equation_type(sigma, var, e);
        equations.push_back(eqn);
      } else {
        term_type phi = tr::is_or(qexpr) ? tr::true_() : tr::false_();
        term_type psi = qexpr;
        if (tr::is_or(qexpr) || tr::is_imp(qexpr)) {
          term_type l = pbes_system::accessors::left(qexpr);
          term_type r = pbes_system::accessors::right(qexpr);
          if (is_simple_expression(l)) {
            phi = l;
            psi = r;
          }
        }
        if (is_inner_implies(psi)) {
          // Eqn of the form sigma X(d: D) = Forall l: D . phi => Y(h(d, l)), with phi a simple formula.
          // Add sigma X(d) = e.
          equation_type eqn = equation_type(sigma, var, e);
          equations.push_back(eqn);
        } else if (!qvars.empty() || !(tr::is_or(qexpr) ? tr::is_true(phi) : tr::is_false(phi))) {
          std::string fresh_varname = fresh_variable_name(var.name());

          data::variable_list variable_parameters = var.parameters() + qvars;
          // Create fresh propositional variable.
          propositional_variable fresh_var =
              propositional_variable(fresh_varname, variable_parameters);
          propositional_variable_instantiation fresh_var_instantiation =
              propositional_variable_instantiation(fresh_varname, atermpp::container_cast<data::data_expression_list>(variable_parameters));
          term_type expr;
          if (tr::is_or(qexpr)) {
            if (tr::is_true(phi)) {
              expr = fresh_var_instantiation;
            } else {
              expr = tr::or_(phi, fresh_var_instantiation);
            }
          } else {
            if (tr::is_false(phi)) {
              expr = fresh_var_instantiation;
            } else {
              expr = tr::imp(phi, fresh_var_instantiation);
            }
          }
          if (!qvars.empty()) {
            expr = tr::forall(qvars, expr);
          }

          // Add sigma X(d) = forall (qvars) . phi => fresh_X(d+qvars).
          equation_type eqn = equation_type(sigma, var, expr);
          equations.push_back(eqn);

          // Rewrite new equation: sigma fresh_X(d+qvars) = psi.
          equation_type new_eqn = equation_type(sigma, fresh_var, psi);
          visit_bqnf_equation(new_eqn);
        } else {
          //std::clog << "visit_inner_implies: false." << std::endl;
          // qexpr not of the form phi /\ psi
          result = false;
        }
      }
      dec_indent();
      return result;
    }


    /// \brief Visits a bounded universal quantifier expression. Rewrites the expression to PPG form by
    /// collecting quantified variables and calling visit_inner_implies.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \return true if the expression e conforms to BQNF.
    virtual bool visit_bounded_forall(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e)
    {
      inc_indent();
      //indent(); std::clog << "visit_bounded_forall: " << print_brief(e) << std::endl;
      assert(tr::is_forall(e));
      data::variable_list qvars = tr::var(e);
      term_type qexpr = pbes_system::accessors::arg(e);
      while (tr::is_forall(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      bool result = visit_inner_implies(sigma, var, e, qvars, qexpr);
      if (!result) {
        std::clog << "Unexpected result: expression not in form phi => psi: " << pp(e) << std::endl;
        throw(std::runtime_error("Unexpected result: expression not in form phi => psi."));
      }
      dec_indent();
      return result;
    }



    /// \brief Visits a conjunctive expression qexpr within a bounded existential quantifier expression e.
    /// Rewrites the expression qexpr to PPG form and adds a existential quantifier over the variables qvars.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e the parent PBES expression.
    /// \param qvars the quantifier variables of the quantifier in the parent expression.
    /// \param qexpr the PBES expression within the quantifier expression.
    /// \return true if the expression qexpr conforms to BQNF.
    virtual bool visit_inner_and(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e, const data::variable_list& qvars, const term_type& qexpr)
    {
      bool result = true;
      inc_indent();
      //indent(); std::clog << "visit_inner_and: " << print_brief(e) << std::endl;
      if (tr::is_prop_var(qexpr) || is_simple_expression(qexpr)) {
        // Eqn of the form sigma X(d: D) = Exists l: D . phi && Y(h(d, l)), with phi a simple formula.
        // Add sigma X(d) = e.
        equation_type eqn = equation_type(sigma, var, e);
        equations.push_back(eqn);
        //std::clog << "visit_inner_and: simple expr." << std::endl;
      } else {
        term_type phi = tr::true_();
        term_type psi = qexpr;
        if (tr::is_and(qexpr)) {
          term_type l = pbes_system::accessors::left(qexpr);
          term_type r = pbes_system::accessors::right(qexpr);
          bool l_is_simple = is_simple_expression(l);
          if (l_is_simple) {
            phi = l;
            psi = r;
          }
        }
        if (is_inner_and(psi)) {
          // Eqn of the form sigma X(d: D) = Exists l: D . phi && Y(h(d, l)), with phi a simple formula.
          // Add sigma X(d) = e.
          equation_type eqn = equation_type(sigma, var, e);
          equations.push_back(eqn);
          //std::clog << "visit_inner_and: phi /\\ psi. psi is propvar." << std::endl;
        } else if (!qvars.empty() || !tr::is_true(phi)) {
          std::string fresh_varname = fresh_variable_name(var.name());
          data::variable_list variable_parameters = var.parameters() + qvars;
          // Create fresh propositional variable.
          propositional_variable fresh_var =
              propositional_variable(fresh_varname, variable_parameters);
          propositional_variable_instantiation fresh_var_instantiation =
              propositional_variable_instantiation(fresh_varname, atermpp::container_cast<data::data_expression_list>(variable_parameters));
          term_type expr;
          if (tr::is_true(phi)) {
            expr = fresh_var_instantiation;
          } else {
            expr = tr::and_(phi, fresh_var_instantiation);
          }
          if (!qvars.empty()) {
            expr = tr::exists(qvars, expr);
          }

          // Add sigma X(d) = exists (qvars) . phi && fresh_X(d+qvars).
          equation_type eqn = equation_type(sigma, var, expr);
          equations.push_back(eqn);

          // Rewrite new equation: sigma fresh_X(d+qvars) = psi.
          equation_type new_eqn = equation_type(sigma, fresh_var, psi);
          //std::clog << "visit_inner_and: phi /\\ psi. fresh_var = " << pp(e) << ", phi = " << pp(phi) << ", psi = " << pp(psi) << std::endl;
          visit_bqnf_equation(new_eqn);
        } else {
          //std::clog << "visit_inner_and: false." << std::endl;
          // qexpr not of the form phi => psi
          result = false;
        }
      }
      dec_indent();
      return result;
    }


    /// \brief Visits a bounded existential quantifier expression. Rewrites the expression to PPG form by
    /// collecting quantified variables and calling visit_inner_and.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \return true if the expression e conforms to BQNF.
    virtual bool visit_bounded_exists(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e)
    {
      inc_indent();
      //indent(); std::clog << "visit_bounded_exists" << print_brief(e) << std::endl;
      assert(tr::is_exists(e));
      term_type qexpr = pbes_system::accessors::arg(e);
      data::variable_list qvars = tr::var(e);
      while (tr::is_exists(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      bool result = visit_inner_and(sigma, var, e, qvars, qexpr);
      if (!result) {
        std::clog << "Unexpected result: expression not in form phi /\\ psi: " << pp(e) << std::endl;
        throw(std::runtime_error("Unexpected result: expression not in form phi /\\ psi."));
      }
      dec_indent();
      return result;
    }

    /// \brief Rewrites an inner bounded universal quantifier expression to PPG form.
    /// Returns the rewritten expression and possibly a newly introduced equation.
    /// The new equation is itself not rewritten yet.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \param dummy dummy equation, returned as equation if no equation is generated.
    /// \return a pair consisting of the expression that replaces the original expression e
    /// and an equation that is possibly introduced as result of the transformation (or the
    /// dummy equation if no new equation is created).
    virtual std::pair<term_type,equation_type> rewrite_inner_bounded_forall(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e, const equation_type& dummy)
    {
      inc_indent();
      //indent(); std::clog << "rewrite_inner_bounded_forall: " << print_brief(e) << std::endl;
      term_type qexpr = e;
      data::variable_list qvars;
      while (tr::is_forall(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }

      term_type expr;
      equation_type eqn = dummy;

      if (tr::is_prop_var(qexpr) || is_simple_expression(qexpr)) {
        // Eqn of the form phi => Y(h(d, l)), with phi a simple formula.
        expr = e;
      } else {
        term_type phi = tr::is_or(qexpr) ? tr::true_() : tr::false_();
        term_type psi = qexpr;
        if (tr::is_or(qexpr) || tr::is_imp(qexpr)) {
          term_type l = pbes_system::accessors::left(qexpr);
          term_type r = pbes_system::accessors::right(qexpr);
          if (is_simple_expression(l)) {
            phi = l;
            psi = r;
          }
        }
        if (is_inner_implies(psi)) {
          // Eqn of the form sigma X(d: D) = Forall l: D . phi => Y(h(d, l)), with phi a simple formula.
          expr = e;
        } else {
          std::string fresh_varname = fresh_variable_name(var.name());
          data::variable_list variable_parameters = var.parameters() + qvars;
          // Create fresh propositional variable.
          propositional_variable fresh_var =
              propositional_variable(fresh_varname, variable_parameters);
          propositional_variable_instantiation fresh_var_instantiation =
              propositional_variable_instantiation(fresh_varname, atermpp::container_cast<data::data_expression_list>(variable_parameters));
          // expr = forall (qvars) . phi => fresh_X(d+qvars).
          if (tr::is_or(qexpr)) {
            if (tr::is_true(phi)) {
              expr = fresh_var_instantiation;
            } else {
              expr = tr::or_(phi, fresh_var_instantiation);
            }
          } else {
            if (tr::is_false(phi)) {
              expr = fresh_var_instantiation;
            } else {
              expr = tr::imp(phi, fresh_var_instantiation);
            }
          }
          if (tr::is_forall(e)) {
            expr = tr::forall(qvars, expr);
          }

          // New equation: sigma fresh_X(d+qvars) = psi.
          eqn = equation_type(sigma, fresh_var, psi);
        }
      }
      dec_indent();
      return std::make_pair(expr, eqn);
    }

    /// \brief Visits a conjunctive expression. Rewrites the expression to PPG form.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \return true if the expression e conforms to BQNF.
    virtual bool visit_and(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e)
    {
      inc_indent();
      //indent(); std::clog << "visit_and: " << print_brief(e) << std::endl;
      equation_type dummy;
      term_type conjunction = tr::true_();
      std::vector<equation_type> new_eqns;
      std::vector<term_type> conjuncts = pbes_expr::split_conjuncts(e);
      for (std::vector<term_type>::const_iterator c = conjuncts.begin(); c != conjuncts.end(); ++c) {
        term_type expr = *c;
        std::pair<term_type,equation_type> p = rewrite_inner_bounded_forall(sigma, var, expr, dummy);
        if (tr::is_true(conjunction)) {
          conjunction = p.first;
        } else {
          conjunction = tr::and_(conjunction, p.first);
        }
        if (p.second != dummy) {
          new_eqns.push_back(p.second);
        }
      }
      // Add conjunction with simplified terms.
      equation_type eqn = equation_type(sigma, var, conjunction);
      equations.push_back(eqn);
      for (std::vector<equation_type>::const_iterator new_eqn = new_eqns.begin(); new_eqn != new_eqns.end(); ++new_eqn) {
        // Rewrite new equation.
        visit_bqnf_equation(*new_eqn);
      }
      dec_indent();
      return true;
    }

    /// \brief Rewrites an inner bounded existential quantifier expression to PPG form.
    /// Returns the rewritten expression and possibly a newly introduced equation.
    /// The new equation is itself not rewritten yet.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \param dummy dummy equation, returned as equation if no equation is generated.
    /// \return a pair consisting of the expression that replaces the original expression e
    /// and an equation that is possibly introduced as result of the transformation (or the
    /// dummy equation if no new equation is created).
    virtual std::pair<term_type,equation_type> rewrite_inner_bounded_exists(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e, const equation_type& dummy)
    {
      inc_indent();
      //indent(); std::clog << "rewrite_inner_bounded_exists: " << print_brief(e) << std::endl;
      term_type qexpr = e;
      data::variable_list qvars;
      while (tr::is_exists(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }

      term_type expr;
      equation_type eqn = dummy;

      if (tr::is_prop_var(qexpr) || is_simple_expression(qexpr)) {
        // Eqn of the form phi && Y(h(d, l)), with phi a simple formula.
        expr = e;
      } else {
        term_type phi = tr::true_();
        term_type psi = qexpr;
        if (tr::is_and(qexpr)) {
          term_type l = pbes_system::accessors::left(qexpr);
          term_type r = pbes_system::accessors::right(qexpr);
          if (is_simple_expression(l)) {
            phi = l;
            psi = r;
          }
        }
        if (is_inner_and(psi)) {
          // Eqn of the form sigma X(d: D) = Forall l: D . phi => Y(h(d, l)), with phi a simple formula.
          expr = e;
        } else {
          std::string fresh_varname = fresh_variable_name(var.name());
          data::variable_list variable_parameters = var.parameters() + qvars;
          // Create fresh propositional variable.
          propositional_variable fresh_var =
              propositional_variable(fresh_varname, variable_parameters);
          propositional_variable_instantiation fresh_var_instantiation =
              propositional_variable_instantiation(fresh_varname, atermpp::container_cast<data::data_expression_list>(variable_parameters));
          // expr = forall (qvars) . phi => fresh_X(d+qvars).
          if (tr::is_true(phi)) {
            expr = fresh_var_instantiation;
          } else {
            expr = tr::and_(phi, fresh_var_instantiation);
          }
          if (tr::is_exists(e)) {
            expr = tr::exists(qvars, expr);
          }

          // New equation: sigma fresh_X(d+qvars) = psi.
          eqn = equation_type(sigma, fresh_var, psi);
        }
      }
      dec_indent();
      return std::make_pair(expr, eqn);
    }

    /// \brief Visits a disjunctive expression. Rewrites the expression to PPG form.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \return true if the expression e conforms to BQNF.
    virtual bool visit_or(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e)
    {
      inc_indent();
      //indent(); std::clog << "visit_or: " << print_brief(e) << std::endl;
      equation_type dummy;
      term_type disjunction = tr::false_();
      std::vector<equation_type> new_eqns;
      std::vector<term_type> disjuncts = pbes_expr::split_disjuncts(e);
      for (std::vector<term_type>::const_iterator d = disjuncts.begin(); d != disjuncts.end(); ++d) {
        term_type expr = *d;
        std::pair<term_type,equation_type> p = rewrite_inner_bounded_exists(sigma, var, expr, dummy);
        if (tr::is_false(disjunction)) {
          disjunction = p.first;
        } else {
          disjunction = tr::or_(disjunction, p.first);
        }
        if (p.second != dummy) {
          new_eqns.push_back(p.second);
        }
      }
      // Add disjunction with simplified terms.
      equation_type eqn = equation_type(sigma, var, disjunction);
      equations.push_back(eqn);
      for (std::vector<equation_type>::const_iterator new_eqn = new_eqns.begin(); new_eqn != new_eqns.end(); ++new_eqn) {
        // Rewrite new equation.
        visit_bqnf_equation(*new_eqn);
      }
      dec_indent();
      return true;
    }

    /// \brief Visits a BQNF expression. Rewrites the expression to PPG form.
    /// The resulting equations are added to the sequence of result equations.
    /// \param sigma fixpoint symbol of the parent equation.
    /// \param var propositional variable of the parent equation.
    /// \param e a PBES expression.
    /// \return true if the right hand side of the equation is in BQNF.
    virtual bool visit_bqnf_expression(const fixpoint_symbol& sigma, const propositional_variable& var, const term_type& e)
    {
      inc_indent();
      //indent(); std::clog << "visit_bqnf_expression: " << print_brief(e) << std::endl;
      if (tr::is_prop_var(e) || is_simple_expression(e)) {
        // Eqn of the form sigma X(d: D) = phi && Y(h(d, l)), with phi a simple formula.
        // Add sigma X(d) = e.
        equation_type eqn = equation_type(sigma, var, e);
        equations.push_back(eqn);
      } else if (tr::is_forall(e) || tr::is_exists(e)) {
        visit_bounded_quantifier(sigma, var, e);
      } else if (tr::is_or(e) || tr::is_imp(e)) {
        data::variable_list empty_seq;
        visit_inner_implies(sigma, var, e, empty_seq, e) || visit_or(sigma, var, e);
      } else if (tr::is_and(e)) {
        data::variable_list empty_seq;
        visit_inner_and(sigma, var, e, empty_seq, e) || visit_and(sigma, var, e);
      }
      dec_indent();
      return true;
    }

    /// \brief Visits a BQNF equation. Rewrites the equation to a PPG equation.
    /// The resulting equations are added to the sequence of result equations.
    /// \param eqn a PBES equation
    /// \return true if the right hand side of the equation is in BQNF.
    virtual bool visit_bqnf_equation(const equation_type& eqn)
    {
      const fixpoint_symbol& sigma = eqn.symbol();
      const propositional_variable& var = eqn.variable();
      const term_type& e = eqn.formula();
      //std::clog << "visit_bqnf_equation: rewrite equation: " << pp(sigma) << " " << var.name() << " = ..." << std::endl;
      if (tr::is_prop_var(e) || is_simple_expression(e)) {
        // Eqn of the form sigma X(d: D) = g(d) && Y(h(d)), with g(d) a simple formula
        // Add eqn
        equations.push_back(eqn);
      } else {
        // Postpone processing of the equation to the next level.
        visit_bqnf_expression(sigma, var, e);
      }
      //std::clog << "visit_bqnf_equation: generated " << equations.size() << " equations." << std::endl;
      return true;
    }

  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif /* BQNF2PPG_REWRITER_H_ */
