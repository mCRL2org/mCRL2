#define MCRL2_PBES_TRANSLATE_DEBUG

// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_translate.h
/// \brief The pbes_translate algorithm.

#ifndef MCRL2_PBES_PBES_TRANSLATE_H
#define MCRL2_PBES_PBES_TRANSLATE_H

#include <algorithm>
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/free_variables.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/pbes/multi_action_equality.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2 {

namespace pbes_system {

inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const atermpp::vector<pbes_equation>& q)
{
  atermpp::vector<pbes_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}

inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const pbes_equation& e)
{
  atermpp::vector<pbes_equation> result(p);
  result.push_back(e);
  return result;
}

/// \brief Abstract algorithm class for translating a state formula and a specification to a pbes.
class pbes_translate_algorithm
{
  protected:

    ///////////////////////////////////////////////////////////////////////////////
    // timed_action
    /// \brief multi-action with time
    ///
    class timed_action
    {
      protected:
        lps::action_list     m_actions;
        data::data_expression m_time;

      public:
        timed_action(lps::action_list actions, data::data_expression time)
          : m_actions(actions), m_time(time)
        {}

        /// Returns true if time is available.
        ///
        bool has_time() const
        {
          return !data::data_expr::is_nil(m_time);
        }

        /// Returns the sequence of actions. Returns an empty list if is_delta() holds.
        ///
        lps::action_list actions() const
        {
          return m_actions;
        }

        /// Returns the time expression.
        ///
        data::data_expression time() const
        {
          return m_time;
        }

        /// Returns a term representing the name of the first lps::action.
        ///
        core::identifier_string name() const
        {
          return front(m_actions).label().name();
        }

        /// Returns the argument of the multi lps::action.
        data::data_expression_list arguments() const
        {
          return front(m_actions).arguments();
        }

        /// Applies a substitution to this lps::action and returns the result.
        /// The Substitution object must supply the method aterm operator()(aterm).
        ///
        template <typename Substitution>
        timed_action substitute(Substitution f)
        {
          return timed_action(m_actions.substitute(f), m_time.substitute(f));
        }

        std::ostream& operator<<(std::ostream& to) const
        {
          to << "TimedAction(" << actions();
          if (has_time())
            to << "," << time();
          to << ")";
          return to;
        }
    };

    data::data_variable_list Par(core::identifier_string x, data::data_variable_list l, modal::state_formula f)
    {
      using namespace modal::state_frm;

      if (is_data(f)) {
        return data::data_variable_list();
      } else if (is_true(f)) {
        return data::data_variable_list();
      } else if (is_false(f)) {
        return data::data_variable_list();
      } else if (is_not(f)) {
        return Par(x, l, not_arg(f));
      } else if (is_and(f)) {
        return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
      } else if (is_or(f)) {
        return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
      } else if (is_imp(f)) {
        return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
      } else if (is_must(f)) {
        return Par(x, l, mod_form(f));
      } else if (is_may(f)) {
        return Par(x, l, mod_form(f));
      } else if (is_forall(f)) {
        return Par(x, l + quant_vars(f), quant_form(f));
      } else if (is_exists(f)) {
        return Par(x, l + quant_vars(f), quant_form(f));
      } else if (is_var(f)) {
        return data::data_variable_list();
      } else if (is_mu(f) || (is_nu(f))) {
        if (mu_name(f) == x)
        {
          return l;
        }
        else
        {
          data::data_variable_list xf = detail::mu_variables(f);
          modal::state_formula g = arg3(f);
          return xf + Par(x, l + xf, g);
        }
      } else if (is_yaled_timed(f)) {
        return data::data_variable_list();
      } else if (is_delay_timed(f)) {
        return data::data_variable_list();
      }
      assert(false);
      return data::data_variable_list();
    }

    modal::state_formula preprocess_formula(const modal::state_formula& formula, const lps::specification& spec)
    {
      using namespace detail;
      using namespace modal::state_frm;

      modal::state_formula f = formula;
      std::set<core::identifier_string> formula_variable_names = data::detail::find_variable_names(formula);
      std::set<core::identifier_string> spec_variable_names = data::detail::find_variable_names(spec);
      std::set<core::identifier_string> spec_names = data::find_identifiers(spec);

      // rename data variables in f, to prevent name clashes with data variables in spec
      data::set_identifier_generator generator;
      generator.add_identifiers(spec_variable_names);
      f = modal::rename_data_variables(f, generator);

      // rename predicate variables in f, to prevent name clashes
      data::xyz_identifier_generator xyz_generator;
      xyz_generator.add_identifiers(spec_names);
      xyz_generator.add_identifiers(formula_variable_names);
      f = rename_predicate_variables(f, xyz_generator);

      // wrap the formula inside a 'nu' if needed
      if (!is_mu(f) && !is_nu(f))
      {
        atermpp::aterm_list context = make_list(f, spec);
        core::identifier_string X = data::fresh_identifier(context, std::string("X"));
        f = nu(X, data::data_assignment_list(), f);
      }

      return f;
    }

  public:
    pbes_translate_algorithm()
    {}

    virtual pbes<> run(const modal::state_formula& formula, const lps::specification& spec) = 0;
};

/// \brief Algorithm for translating a state formula and a timed specification to a pbes.
class pbes_translate_algorithm_timed: public pbes_translate_algorithm
{
  protected:

    pbes_expression sat_top(timed_action a, modal::action_formula b)
    {
      using namespace modal::act_frm;
      namespace d = data::data_expr;
      namespace p = pbes_expr_optimized;

      pbes_expression result;

      if (is_mult_act(b)) {
        lps::action_list b_actions = mult_params(b);
        result = equal_multi_actions(a.actions(), b_actions);
      } else if (is_true(b)) {
        result = p::true_();
      } else if (is_at(b)) {
        data::data_expression t = a.time();
        modal::action_formula alpha = at_form(b);
        data::data_expression t1 = at_time(b);
        result = p::and_(sat_top(a, alpha), p::val(d::equal_to(t, t1)));
      } else if (is_not(b)) {
        result = p::not_(sat_top(a, not_arg(b)));
      } else if (is_and(b)) {
        result = p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_or(b)) {
        result = p::or_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_imp(b)) {
        result = p::imp(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_forall(b)) {
        data::data_variable_list x = quant_vars(b);
        assert(x.size() > 0);
        modal::action_formula alpha = quant_form(b);
        data::data_variable_list y = fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), a.time(), b)));
        result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      } else if (is_exists(b)) {
        data::data_variable_list x = quant_vars(b);
        assert(x.size() > 0);
        modal::action_formula alpha = quant_form(b);
        data::data_variable_list y = fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), a.time(), b)));
        result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      } else {
        throw std::runtime_error(std::string("sat_top[timed] error: unknown lps::action formula ") + b.to_string());
      }
      return result;
    }

    /// f0 is the original formula
    pbes_expression RHS(modal::state_formula f0, modal::state_formula f, lps::linear_process lps, data::data_variable T, std::set<std::string>& context)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << "<RHS>" << pp(f) << std::flush;
#endif
      using namespace pbes_expr_optimized;
      using namespace pbes_system::accessors;
      using lps::summand_list;
      namespace s = modal::state_frm;
      namespace d = data::data_expr;

      pbes_expression result;

      if (!s::is_not(f))
      {
        if (s::is_data(f)) {
          result = pbes_expression(f);
        } else if (s::is_true(f)) {
          result = true_();
        } else if (s::is_false(f)) {
          result = false_();
        } else if (s::is_and(f)) {
	  	    result = and_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
        } else if (s::is_or(f)) {
          result = or_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
        } else if (s::is_imp(f)) {
	  	    // TODO: generalize
	  	    // result = imp(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
	  	    result = or_(RHS(f0, s::not_(s::lhs(f)), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
        } else if (s::is_must(f)) {
          atermpp::vector<pbes_expression> v;
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            data::data_expression ti(i->time());
            timed_action ai(i->actions(), ti);
            data::data_assignment_list gi = i->assignments();
            data::data_variable_list xp(lps.process_parameters());
            data::data_variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, f1, lps, T, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::data_variable_list y = fresh_variables(yi, context);
            ci = ci.substitute(make_list_substitution(yi, y));
            ti = ti.substitute(make_list_substitution(yi, y));
            ai = ai.substitute(make_list_substitution(yi, y));
            gi = gi.substitute(make_list_substitution(yi, y));

            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = val(ci);
            pbes_expression p3 = val(d::greater(ti, T));
            rhs = rhs.substitute(make_substitution(T, ti));
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = forall(y, imp(and_(and_(p1, p2), p3), rhs));
            v.push_back(p);
          }
          result = join_and(v.begin(), v.end());
        } else if (s::is_may(f)) {
          atermpp::vector<pbes_expression> v;
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            data::data_expression ti(i->time());
            timed_action ai(i->actions(), ti);
            data::data_assignment_list gi = i->assignments();
            data::data_variable_list xp(lps.process_parameters());
            data::data_variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, f1, lps, T, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::data_variable_list y = fresh_variables(yi, context);
            ci = ci.substitute(make_list_substitution(yi, y));
            ti = ti.substitute(make_list_substitution(yi, y));
            ai = ai.substitute(make_list_substitution(yi, y));
            gi = gi.substitute(make_list_substitution(yi, y));

            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = val(ci);
            pbes_expression p3 = val(d::greater(ti, T));
            rhs = rhs.substitute(make_substitution(T, ti));
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = exists(y, and_(and_(and_(p1, p2), p3), rhs));
            v.push_back(p);
          }
          result = join_or(v.begin(), v.end());
        } else if (s::is_delay_timed(f)) {
          data::data_expression t = s::time(f);
          atermpp::vector<pbes_expression> v;
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            data::data_expression ck(i->condition());
            data::data_expression tk(i->time());
            data::data_variable_list yk = i->summation_variables();
            pbes_expression p = exists(yk, and_(val(ck), val(d::less_equal(t, tk))));
            v.push_back(p);
          }
          result = or_(join_or(v.begin(), v.end()), val(d::less_equal(t, T)));
        } else if (s::is_yaled_timed(f)) {
          data::data_expression t = s::time(f);
          atermpp::vector<pbes_expression> v;
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            data::data_expression ck(i->condition());
            data::data_expression tk(i->time());
            data::data_variable_list yk = i->summation_variables();
            pbes_expression p = exists(yk, and_(val(d::not_(ck)), val(d::greater(t, tk))));
            v.push_back(p);
          }
          result = and_(join_or(v.begin(), v.end()), val(d::greater(t, T)));
        } else if (s::is_var(f)) {
          core::identifier_string X = s::var_name(f);
          data::data_expression_list d = s::var_val(f);
          data::data_variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, T + d + xp + Par(X, data::data_variable_list(), f0));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::mu_name(f);
          data::data_expression_list d = detail::mu_expressions(f);
          data::data_variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, T + d + xp + Par(X, data::data_variable_list(), f0));
        } else {
          throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = s::not_arg(f);
        if (s::is_data(f)) {
          result = pbes_expression(d::not_(f));
        } else if (s::is_true(f)) {
          result = false_();
        } else if (s::is_false(f)) {
          result = true_();
        } else if (s::is_not(f)) {
	  	    result = s::not_arg(f);
        } else if (s::is_and(f)) {
	  	    result = or_(RHS(f0, s::not_(s::lhs(f)), lps, T, context), RHS(f0, s::not_(s::rhs(f)), lps, T, context));
        } else if (s::is_or(f)) {
          result = and_(RHS(f0, s::not_(s::lhs(f)), lps, T, context), RHS(f0, s::not_(s::rhs(f)), lps, T, context));
        } else if (s::is_imp(f)) {
          result = and_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::not_(s::rhs(f)), lps, T, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = forall(s::quant_vars(f), RHS(f0, s::not_(s::quant_form(f)), lps, T, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = exists(s::quant_vars(f), RHS(f0, s::not_(s::quant_form(f)), lps, T, context));
        } else if (s::is_must(f)) {
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          result = RHS(f0, s::may(alpha, s::not_(f1)), lps, T, context);
        } else if (s::is_may(f)) {
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          result = RHS(f0, s::must(alpha, s::not_(f1)), lps, T, context);
        } else if (s::is_delay_timed(f)) {
          data::data_expression t = s::time(f);
          result = RHS(f0, s::yaled_timed(t), lps, T, context);
        } else if (s::is_yaled_timed(f)) {
          data::data_expression t = s::time(f);
          result = RHS(f0, s::delay_timed(t), lps, T, context);
        } else if (s::is_var(f)) {
          result = RHS(f0, f, lps, T, context);
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::mu_name(f);
          data::data_assignment_list xf = s::mu_params(f);
          modal::state_formula phi = s::mu_form(f);
          if (s::is_mu(f))
          {
            result = RHS(f0, s::mu(X, xf, s::not_(phi)), lps, T, context);
          }
          else
          {
            result = RHS(f0, s::nu(X, xf, s::not_(phi)), lps, T, context);
          }
        } else {
          throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << " -> " << pp(result) << std::endl;
#endif
      return result;
    }

    /// f0 is the original formula
    atermpp::vector<pbes_equation> E(modal::state_formula f0, modal::state_formula f, lps::linear_process lps, data::data_variable T)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << "<E>" << pp(f) << std::flush;
#endif
      using namespace modal::state_frm;
      atermpp::vector<pbes_equation> result;

      if (!is_not(f))
      {
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_and(f)) {
          result = E(f0, not_(lhs(f)), lps, T) + E(f0, rhs(f), lps, T);
        } else if (is_or(f)) {
          result = E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
        } else if (is_imp(f)) {
          result = E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
        } else if (is_forall(f)) {
          result = E(f0, quant_form(f), lps, T);
        } else if (is_exists(f)) {
          result = E(f0, quant_form(f), lps, T);
        } else if (is_must(f)) {
          result = E(f0, mod_form(f), lps, T);
        } else if (is_may(f)) {
          result = E(f0, mod_form(f), lps, T);
        } else if (is_var(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = mu_name(f);
          data::data_variable_list xf = detail::mu_variables(f);
          data::data_variable_list xp = lps.process_parameters();
          modal::state_formula g = mu_form(f);
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
          propositional_variable v(X, T + xf + xp + Par(X, data::data_variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, T, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps, T);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw std::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = not_arg(f);
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_not(f)) {
          result = E(f0, not_arg(f), lps, T);
        } else if (is_and(f)) {
          result = E(f0, not_(lhs(f)), lps, T) + E(f0, not_(rhs(f)), lps, T);
        } else if (is_or(f)) {
          result = E(f0, not_(lhs(f)), lps, T) + E(f0, not_(rhs(f)), lps, T);
        } else if (is_imp(f)) {
          result = E(f0, lhs(f), lps, T) + E(f0, not_(rhs(f)), lps, T);
        } else if (is_forall(not_(f))) {
          result = E(f0, quant_form(f), lps, T);
        } else if (is_exists(not_(f))) {
          result = E(f0, quant_form(f), lps, T);
        } else if (is_must(f)) {
          result = E(f0, not_(mod_form(f)), lps, T);
        } else if (is_may(f)) {
          result = E(f0, not_(mod_form(f)), lps, T);
        } else if (is_var(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = mu_name(f);
          data::data_variable_list xf = detail::mu_variables(f);
          data::data_variable_list xp = lps.process_parameters();
          modal::state_formula g = not_(mu_form(f));
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
          propositional_variable v(X, T + xf + xp + Par(X, data::data_variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, T, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps, T);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw std::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << " -> " << pp(pbes_equation_list(result.begin(), result.end())) << std::endl;
#endif
      return result;
    }

  public:
    /// Constructor.
    pbes_translate_algorithm_timed()
    {}

    pbes<> run(const modal::state_formula& formula, const lps::specification& spec)
    {
      using namespace modal::state_frm;

      lps::linear_process lps = spec.process();

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      modal::state_formula f = preprocess_formula(formula, spec);

      // make sure the lps is timed
      data::data_variable T = fresh_variable(make_list(f, lps), data::sort_expr::real(), "T");
      atermpp::aterm_list context = make_list(T, spec.initial_process(), lps, f);
      lps = lps::detail::make_timed_lps(lps, context);

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps, T);

      // compute initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(is_mu(f) || is_nu(f));
      core::identifier_string Xf = mu_name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state();
      atermpp::set<data::data_variable> free_variables(spec.process().free_variables().begin(), spec.process().free_variables().end());
      propositional_variable_instantiation init(Xe, data::data_expr::real(0) + fi + pi + Par(Xf, data::data_variable_list(), f));

      // add sort real to data_spec (if needed)
      data::data_specification data_spec = spec.data();
      if (std::find(spec.data().sorts().begin(), spec.data().sorts().end(), data::sort_expr::real()) == spec.data().sorts().end())
      {
        data_spec = data::set_sorts(data_spec, push_front(data_spec.sorts(), data::sort_expr::real()));
      }

      pbes<> result(data_spec, e, free_variables, init);
      result.normalize();
      assert(result.is_normalized());
      assert(result.is_closed());
      return result;
    }
};

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class pbes_translate_algorithm_untimed: public pbes_translate_algorithm
{
  protected:
    pbes_expression sat_top(lps::action_list a, modal::action_formula b)
    {
      using namespace modal::act_frm;
      namespace p = pbes_expr_optimized;

      pbes_expression result;

      if (is_mult_act(b)) {
        lps::action_list b_actions = mult_params(b);
        result = equal_multi_actions(a, b_actions);
      } else if (is_true(b)) {
        result = p::true_();
      } else if (is_not(b)) {
        result = p::not_(sat_top(a, not_arg(b)));
      } else if (is_and(b)) {
        result = p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_or(b)) {
        result = p::or_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_imp(b)) {
        result = p::imp(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
      } else if (is_forall(b)) {
        data::data_variable_list x = quant_vars(b);
        modal::action_formula alpha = quant_form(b);
        if (x.size() > 0)
        {
          data::data_variable_list y = fresh_variables(x, data::detail::find_variable_name_strings(make_list(a, b)));
          result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
        }
        else
          result = sat_top(a, alpha);
      } else if (is_exists(b)) {
        data::data_variable_list x = quant_vars(b);
        modal::action_formula alpha = quant_form(b);
        if (x.size() > 0)
        {
          data::data_variable_list y = fresh_variables(x, data::detail::find_variable_name_strings(make_list(a, b)));
          result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
        }
        else
          result = sat_top(a, alpha);
      } else {
        throw std::runtime_error(std::string("sat_top[untimed] error: unknown lps::action formula ") + b.to_string());
      }
      return result;
    }

    /// f0 is the original formula
    pbes_expression RHS(modal::state_formula f0, modal::state_formula f, lps::linear_process lps, std::set<std::string>& context)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << "<RHS>" << pp(f) << std::flush;
#endif
      using namespace pbes_expr_optimized;
      using namespace accessors;
      using lps::summand_list;
      namespace s = modal::state_frm;

      pbes_expression result;

      if (!is_not(f))
      {
        if (s::is_data(f)) {
          result = pbes_expression(f);
        } else if (s::is_true(f)) {
          result = true_();
        } else if (s::is_false(f)) {
          result = false_();
        } else if (s::is_not(f)) {
          result = not_(RHS(f0, s::not_arg(f), lps, context));
        } else if (s::is_and(f)) {
          result = and_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
        } else if (s::is_or(f)) {
          result = or_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
        } else if (s::is_imp(f)) {
          result = imp(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
        } else if (s::is_must(f)) {
          atermpp::vector<pbes_expression> v;
          modal::action_formula alpha(s::mod_act(f));
          modal::state_formula f1(s::mod_form(f));
          for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::action_list ai(i->actions());
            data::data_assignment_list gi = i->assignments();
            data::data_variable_list xp(lps.process_parameters());
            data::data_variable_list yi(i->summation_variables());
        
            pbes_expression rhs = RHS(f0, f1, lps, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::data_variable_list y = fresh_variables(yi, context);
            ci = ci.substitute(make_list_substitution(yi, y));
            ai = ai.substitute(make_list_substitution(yi, y));
            gi = gi.substitute(make_list_substitution(yi, y));
            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = val(ci);
            rhs = rhs.substitute(data::assignment_list_substitution(gi));
        
            pbes_expression p = forall(y, imp(and_(p1, p2), rhs));
            v.push_back(p);
          }
          result = join_and(v.begin(), v.end());
        } else if (s::is_may(f)) {
          atermpp::vector<pbes_expression> v;
          modal::action_formula alpha(s::mod_act(f));
          modal::state_formula f1(s::mod_form(f));
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::action_list ai(i->actions());
            data::data_assignment_list gi = i->assignments();
            data::data_variable_list xp(lps.process_parameters());
            data::data_variable_list yi(i->summation_variables());
        
            pbes_expression rhs = RHS(f0, f1, lps, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::data_variable_list y = fresh_variables(yi, context);
            ci = ci.substitute(make_list_substitution(yi, y));
            ai = ai.substitute(make_list_substitution(yi, y));
            gi = gi.substitute(make_list_substitution(yi, y));
            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = val(ci);
            rhs = rhs.substitute(data::assignment_list_substitution(gi));
        
            pbes_expression p = exists(y, and_(and_(p1, p2), rhs));
            v.push_back(p);
          }
          result = join_or(v.begin(), v.end());
        } else if (s::is_var(f)) {
          core::identifier_string X = s::var_name(f);
          data::data_expression_list d = s::var_val(f);
          data::data_variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, d + xp + Par(X, data::data_variable_list(), f0));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::mu_name(f);
          data::data_expression_list d = detail::mu_expressions(f);
          data::data_variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, d + xp + Par(X, data::data_variable_list(), f0));
        } else {
          throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = s::not_arg(f);
        if (s::is_data(f)) {
          result = pbes_expression(data::data_expr::not_(f));
        } else if (s::is_true(f)) {
          result = false_();
        } else if (s::is_false(f)) {
          result = true_();
        } else if (s::is_not(f)) {
	  	    result = s::not_arg(f);
        } else if (s::is_and(f)) {
	  	    result = or_(RHS(f0, s::not_(s::lhs(f)), lps, context), RHS(f0, s::not_(s::rhs(f)), lps, context));
        } else if (s::is_or(f)) {
          result = and_(RHS(f0, s::not_(s::lhs(f)), lps, context), RHS(f0, s::not_(s::rhs(f)), lps, context));
        } else if (s::is_imp(f)) {
          result = and_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::not_(s::rhs(f)), lps, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = forall(s::quant_vars(f), RHS(f0, s::not_(s::quant_form(f)), lps, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::quant_vars(f));
          context.insert(names.begin(), names.end());
          result = exists(s::quant_vars(f), RHS(f0, s::not_(s::quant_form(f)), lps, context));
        } else if (s::is_must(f)) {
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          result = RHS(f0, s::may(alpha, s::not_(f1)), lps, context);
        } else if (s::is_may(f)) {
          modal::action_formula alpha = s::mod_act(f);
          modal::state_formula f1 = s::mod_form(f);
          result = RHS(f0, s::must(alpha, s::not_(f1)), lps, context);
        } else if (s::is_delay(f)) {
          result = RHS(f0, s::yaled(), lps, context);
        } else if (s::is_yaled(f)) {
          result = RHS(f0, s::delay(), lps, context);
        } else if (s::is_var(f)) {
          result = RHS(f0, f, lps, context);
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::mu_name(f);
          data::data_assignment_list xf = s::mu_params(f);
          modal::state_formula phi = s::mu_form(f);
          if (s::is_mu(f))
          {
            result = RHS(f0, s::mu(X, xf, s::not_(phi)), lps, context);
          }
          else
          {
            result = RHS(f0, s::nu(X, xf, s::not_(phi)), lps, context);
          }
        } else {
          throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << " -> " << pp(result) << std::endl;
#endif
      return result;
    }

    /// f0 is the original formula
    atermpp::vector<pbes_equation> E(modal::state_formula f0, modal::state_formula f, lps::linear_process lps)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << "<E>" << pp(f) << std::flush;
#endif
      using namespace modal::state_frm;
      atermpp::vector<pbes_equation> result;

      if (!is_not(f))
      {
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_not(f)) {
          result = E(f0, not_arg(f), lps);
        } else if (is_and(f)) {
          result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
        } else if (is_or(f)) {
          result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
        } else if (is_imp(f)) {
          result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
        } else if (is_forall(f)) {
          result = E(f0, quant_form(f), lps);
        } else if (is_exists(f)) {
          result = E(f0, quant_form(f), lps);
        } else if (is_must(f)) {
          result = E(f0, mod_form(f), lps);
        } else if (is_may(f)) {
          result = E(f0, mod_form(f), lps);
        } else if (is_var(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = mu_name(f);
          data::data_variable_list xf = detail::mu_variables(f);
          data::data_variable_list xp = lps.process_parameters();
          modal::state_formula g = mu_form(f);
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
          propositional_variable v(X, xf + xp + Par(X, data::data_variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps);
        } else if (is_yaled(f)) {
          // do nothing
        } else if (is_delay(f)) {
          // do nothing
        } else {
          throw std::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = not_arg(f);
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_not(f)) {
          result = E(f0, not_arg(f), lps);
        } else if (is_and(f)) {
          result = E(f0, not_(lhs(f)), lps) + E(f0, not_(rhs(f)), lps);
        } else if (is_or(f)) {
          result = E(f0, not_(lhs(f)), lps) + E(f0, not_(rhs(f)), lps);
        } else if (is_imp(f)) {
          result = E(f0, lhs(f), lps) + E(f0, not_(rhs(f)), lps);
        } else if (is_forall(not_(f))) {
          result = E(f0, quant_form(f), lps);
        } else if (is_exists(not_(f))) {
          result = E(f0, quant_form(f), lps);
        } else if (is_must(f)) {
          result = E(f0, not_(mod_form(f)), lps);
        } else if (is_may(f)) {
          result = E(f0, not_(mod_form(f)), lps);
        } else if (is_var(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = mu_name(f);
          data::data_variable_list xf = detail::mu_variables(f);
          data::data_variable_list xp = lps.process_parameters();
          modal::state_formula g = not_(mu_form(f));
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
          propositional_variable v(X, xf + xp + Par(X, data::data_variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw std::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cout << " -> " << pp(pbes_equation_list(result.begin(), result.end())) << std::endl;
#endif
      return result;
    }

  public:
    /// Constructor.
    pbes_translate_algorithm_untimed()
    {}

    pbes<> run(const modal::state_formula& formula, const lps::specification& spec)
    {
      using namespace modal::state_frm;
      lps::linear_process lps = spec.process();

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      modal::state_formula f = preprocess_formula(formula, spec);

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps);

      // compute the initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(is_mu(f) || is_nu(f));
      core::identifier_string Xf = mu_name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state();
      atermpp::set<data::data_variable> free_variables(spec.process().free_variables().begin(), spec.process().free_variables().end());
      propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, data::data_variable_list(), f));

      pbes<> result = pbes<>(spec.data(), e, free_variables, init);
      result.normalize();
      assert(result.is_normalized());
      assert(result.is_closed());
      return result;
    }
};

/// \brief Translates a modal::state_formula and a lps::specification to a pbes. If the pbes evaluates
/// to true, the formula holds for the lps::specification.
/// \param formula the state formula
/// \param spec the lps::specification
/// \param determines whether the timed or untimed variant of the algorithm is chosen
/// \return The resulting pbes
pbes<> pbes_translate(const modal::state_formula& formula, const lps::specification& spec, bool timed = false)
{
  if (formula.has_time() || spec.process().has_time())
  {
    timed = true;
  }

  if (timed)
  {
    pbes_translate_algorithm_untimed algorithm;
    return algorithm.run(formula, spec);
  }
  else
  {
    pbes_translate_algorithm_timed algorithm;
    return algorithm.run(formula, spec);
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_TRANSLATE_H
