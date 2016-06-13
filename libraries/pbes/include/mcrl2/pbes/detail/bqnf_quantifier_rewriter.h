// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bqnf_quantifier_rewriter.h
/// \brief Replaces universal quantifier over conjuncts with conjuncts over universal quantifiers:
/// rewrite_bqnf_expression(forall x . /\_i phi_i) =
///   /\_i forall (x intersection free(phi_i)) . phi_i.
/// This rewriter is experimental.
#ifndef BQNF_QUANTIFIER_REWRITER_H_
#define BQNF_QUANTIFIER_REWRITER_H_

#include "mcrl2/utilities/detail/memory_utility.h"
#include "bqnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct bqnf_quantifier_rewriter: public bqnf_visitor
  {
    /// \brief The equation type.
    typedef pbes_equation equation_type;
    /// \brief The expression type of the equation.
    typedef pbes_expression term_type;
    typedef core::term_traits<pbes_expression> tr;

    /// \brief Constructor.
    bqnf_quantifier_rewriter()
    {
    }


    /// \brief Filters the expression e such that subexpressions that are data expression that
    /// do not refer to variables in the set d are discarded:
    /// filter(e, d) = e if e is a data expression and the intersection of free(e) and d is not empty;
    /// filter(e1 'op' e2, d) = filter(e1, d) 'op' filter(e2, d).
    /// \param e a simple PBES expression.
    /// \param d a set of variables.
    /// \return the filtered expression.
    virtual term_type filter(const term_type& e, const std::set<data::variable>& d)
    {
      assert(is_simple_expression(e));
      term_type empty;
      if (tr::is_data(e))
      {
        std::vector<data::variable> intersection;
        for (const data::variable& var: tr::free_variables(e))
        {
          if (d.find(var) != d.end())
          {
            intersection.push_back(var);
          }
        }
        if (intersection.empty())
        {
          return e;
        }
        else
        {
          return empty;
        }
      }
      else if (tr::is_and(e) || tr::is_or(e) || tr::is_imp(e))
      {
        term_type l = filter(pbes_system::accessors::left(e), d);
        term_type r = filter(pbes_system::accessors::right(e), d);
        if (l==empty && r==empty)
        {
          return empty;
        }
        else if (l==empty)
        {
          return r;
        }
        else if (r==empty)
        {
          if (tr::is_imp(e))
          {
            return tr::not_(l);
          }
          else
          {
            return l;
          }
        }
        else
        {
          if (tr::is_and(e))
          {
            return tr::and_(l, r);
          }
          else if (tr::is_or(e))
          {
            return tr::or_(l, r);
          }
          else // tr::is_imp(e)
          {
            return tr::imp(l, r);
          }
        }
      }
      else
      {
        std::clog << "filter: Unexpected expression: " << pp(e) << std::endl;
        throw(std::runtime_error("filter: Unexpected expression."));
      }
    }



    /// \brief Filters a 'guard' g with respect to a set of variables d and
    /// an expression phi_i such that parts of g that not relevant for phi_i
    /// are discarded.
    /// filter_guard(g, phi_i, d) = ( exists_{d intersects (free(g) - free(phi_i))} . filter(g, {d - free(phi_i)))
    ///   && filter(g, d intersects free(phi_i)).
    /// \param g a simple expression that functions as guard.
    /// \param phi_i a PBES expression.
    /// \param d a set of relevant variables.
    /// \return the expression g filtered with respect to phi_i and d.
    virtual term_type filter_guard(const term_type& g, const term_type& phi_i, const data::variable_list& d)
    {
      term_type result = tr::true_();
      data::variable_list free_g = tr::free_variables(g);
      std::set<data::variable> free_phi_i;
      for (const data::variable& v: tr::free_variables(phi_i))
      {
        free_phi_i.insert(v);
      }
      std::set<data::variable> free_g_minus_free_phi_i;
      for (const data::variable& var: free_g)
      {
        if (free_phi_i.find(var)==free_phi_i.end()) { // !free_phi_i.contains(v)
          free_g_minus_free_phi_i.insert(var);
        }
      }
      std::vector<data::variable> d_intersects_free_g_minus_free_phi_i;
      std::set<data::variable> d_minus_free_phi_i;
      std::set<data::variable> d_intersects_free_phi_i;
      for (const data::variable& var: d)
      {
        if (free_g_minus_free_phi_i.find(var) != free_g_minus_free_phi_i.end()) // free_g_minus_free_phi_i.contains(v)
        {
          d_intersects_free_g_minus_free_phi_i.push_back(var);
        }
        if (free_phi_i.find(var) != free_phi_i.end()) // free_phi_i.contains(v)
        {
          d_intersects_free_phi_i.insert(var);
        }
        else
        {
          d_minus_free_phi_i.insert(var);
        }
      }
      // Now I have the sets I need; let's generate terms.
      term_type e_1 = filter(g, d_minus_free_phi_i);
      if (!d_intersects_free_g_minus_free_phi_i.empty())
      {
        e_1 =
// data::exists(    // N.B. Removing this, since it does not make sense to convert a pbes_system::exists to a data::exists (Wieger).
            tr::exists(
            data::variable_list(d_intersects_free_g_minus_free_phi_i.begin(), d_intersects_free_g_minus_free_phi_i.end()),
            e_1)
// )
           ;
      }
      term_type e_2 = filter(g, d_intersects_free_phi_i);
      term_type empty;
      if (e_1 == empty)
      {
        if (e_2 != empty)
        {
          result = e_2;
        }
      }
      else
      {
        if (e_2 == empty)
        {
          result = e_1;
        }
        else
        {
          result = tr::and_(e_1, e_2);
        }
      }
      return result;
    }


    /// \brief Rewrites a bounded universal quantifier expression.
    /// If the subexpression is a conjunction, the quantifier over the conjunction
    /// is transformed to a conjunction of quantifier expressions.
    /// rewrite_bounded_forall(forall x . phi => /\_i psi_i) =
    ///   /\_i forall (x intersection free(psi_i)) .
    ///   filter_guard(phi, psi_i, x) => rewrite_bqnf_expression(psi_i).
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_bounded_forall(const term_type& e)
    {
      //std::clog << "rewrite_bounded_forall: " << pp(e) << std::endl;
      assert(tr::is_forall(e));
      data::variable_list qvars = tr::var(e);
      term_type qexpr = pbes_system::accessors::arg(e);
      while (tr::is_forall(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      // forall qvars . qexpr
      term_type result;
      if (tr::is_prop_var(qexpr) || is_simple_expression(qexpr)) {
        // forall d . phi | forall d . X(e)
        result = e;
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
        if (tr::is_prop_var(psi)) {
          // forall d . phi => X(e)
          result = e;
        } else if (!qvars.empty() || !(tr::is_or(qexpr) ? tr::is_true(phi) : tr::is_false(phi))) {
          // forall d . phi => psi
          std::vector<term_type> conjuncts;
          if (tr::is_and(psi)) {
            conjuncts = split_conjuncts(psi);
          } else {
            conjuncts.push_back(psi);
          }
          term_type conjunction = tr::true_();
          for (std::vector<term_type>::const_iterator c = conjuncts.begin(); c != conjuncts.end(); ++c) {
            term_type phi_i = *c;
            term_type r = rewrite_bqnf_expression(phi_i);
            if (tr::is_or(qexpr)) {
              if (!tr::is_true(phi)) {
                phi = filter_guard(phi, r, qvars);
                r = tr::or_(phi, r);
              }
            } else {
              if (!tr::is_false(phi)) {
                phi = filter_guard(phi, r, qvars);
                r = tr::imp(phi, r);
              }
            }
            std::vector<data::variable> qvars_i;
            data::variable_list free_phi_i_list = tr::free_variables(phi_i);
            std::set<data::variable> free_phi_i;
            for (const data::variable& v : tr::free_variables(phi_i))
            {
              free_phi_i.insert(v);
            }
            for (const data::variable& var: qvars)
            {
              if (free_phi_i.find(var) != free_phi_i.end()) // free_phi_i.contains(v)
              {
                qvars_i.push_back(var);
              }
            }
            // qvars_i = qvars intersects free(phi_i)
            if (!qvars_i.empty()) {
              r = tr::forall(data::variable_list(qvars_i.begin(), qvars_i.end()), r);
            }
            if (tr::is_true(conjunction)) {
              conjunction = r;
            } else {
              conjunction = tr::and_(conjunction, r);
            }
          }
          result = conjunction;
        } else {
          // qexpr not of the form phi => psi
          //std::clog << "rewrite_bounded_forall: unexpected qexpr = " << pp(qexpr) << std::endl;
          throw(std::runtime_error("rewrite_bounded_forall: unexpected qexpr."));
        }
      }
      return result;
    }



    /// \brief Rewrites a bounded existential quantifier expression.
    /// rewrite_bounded_exists(exists v . phi) = exists v. rewrite_bqnf_expression(phi)
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_bounded_exists(const term_type& e)
    {
      //std::clog << "rewrite_bounded_exists" << pp(e) << std::endl;
      assert(tr::is_exists(e));
      term_type qexpr = pbes_system::accessors::arg(e);
      data::variable_list qvars = tr::var(e);
      while (tr::is_exists(qexpr)) {
        qvars = qvars + tr::var(qexpr);
        qexpr = pbes_system::accessors::arg(qexpr);
      }
      term_type r = rewrite_bqnf_expression(qexpr);
      term_type result = tr::exists(qvars, r);
      return result;
    }


    /// \brief Rewrites a conjunctive expression.
    /// rewrite_imp(/\_i phi_i) = /\_i rewrite_bqnf_expression(phi_i).
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_and(const term_type& e)
    {
      //std::clog << "rewrite_and: " << pp(e) << std::endl;
      term_type conjunction = tr::true_();
      std::vector<equation_type> new_eqns;
      std::vector<term_type> conjuncts = split_conjuncts(e);
      for (std::vector<term_type>::const_iterator c = conjuncts.begin(); c != conjuncts.end(); ++c) {
        term_type expr = *c;
        term_type r = rewrite_bqnf_expression(expr);
        if (tr::is_true(conjunction)) {
          conjunction = r;
        } else {
          conjunction = tr::and_(conjunction, r);
        }
      }
      return conjunction;
    }


    /// \brief Rewrites a disjunctive expression.
    /// rewrite_imp(\/_i phi_i) = \/_i rewrite_bqnf_expression(phi_i).
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_or(const term_type& e)
    {
      //std::clog << "rewrite_or: " << pp(e) << std::endl;
      term_type disjunction = tr::false_();
      std::vector<term_type> new_exprs;
      std::vector<term_type> disjuncts = split_disjuncts(e);
      for (std::vector<term_type>::const_iterator d = disjuncts.begin(); d != disjuncts.end(); ++d) {
        term_type expr = *d;
        term_type r = rewrite_bqnf_expression(expr);
        if (tr::is_false(disjunction)) {
          disjunction = r;
        } else {
          disjunction = tr::or_(disjunction, r);
        }
      }
      return disjunction;
    }


    /// \brief Rewrites an implication:
    /// rewrite_imp(phi => psi) =
    ///   rewrite_bqnf_expression(phi) => rewrite_bqnf_expression(psi).
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_imp(const term_type& e)
    {
      //std::clog << "rewrite_imp: " << pp(e) << std::endl;
      term_type l = rewrite_bqnf_expression(pbes_system::accessors::left(e));
      term_type r = rewrite_bqnf_expression(pbes_system::accessors::right(e));
      term_type result = tr::imp(l, r);
      return result;
    }


    /// \brief Rewrites a BQNF expression.
    /// Replaces universal quantifier over conjuncts with conjuncts over universal quantifiers:
    /// rewrite_bqnf_expression(forall x . /\_i phi_i) =
    ///   /\_i forall (x intersection free(phi_i)) . phi_i
    /// For subexpressions the transformation is done in the usual way:
    /// rewrite_bqnf_expression(phi 'op' psi) =
    ///   rewrite_bqnf_expression(phi) 'op' rewrite_bqnf_expression(psi).
    /// \param e a PBES expression
    /// \return the expression resulting from the transformation.
    virtual term_type rewrite_bqnf_expression(const term_type& e)
    {
      //std::clog << "rewrite_bqnf_expression: " << pp(e) << std::endl;
      term_type result;
      if (tr::is_prop_var(e) || is_simple_expression(e)) {
        // Eqn of the form sigma X(d: D) = phi && Y(h(d, l)), with phi a simple formula.
        // Add sigma X(d) = e.
        result = e;
      } else if (tr::is_forall(e)) {
        result = rewrite_bounded_forall(e);
      } else if (tr::is_exists(e)) {
        result = rewrite_bounded_exists(e);
      } else if (tr::is_or(e)) {
        result = rewrite_or(e);
      } else if (tr::is_imp(e)) {
        result = rewrite_imp(e);
      } else if (tr::is_and(e)) {
        result = rewrite_and(e);
      } else {
        std::clog << "Unexpected expression: " << pp(e) << std::endl;
        throw(std::runtime_error("Unexpected expression."));
      }
      return result;
    }

  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif /* BQNF_QUANTIFIER_REWRITER_H_ */
