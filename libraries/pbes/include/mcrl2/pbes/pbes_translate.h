// #define MCRL2_PBES_TRANSLATE_DEBUG
// #define MCRL2_EQUAL_MULTI_ACTIONS_DEBUG

// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_translate.h
/// \brief The pbes_translate algorithm.

#ifndef MCRL2_PBES_PBES_TRANSLATE_H
#define MCRL2_PBES_PBES_TRANSLATE_H

#include <sstream>
#include <algorithm>
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/global_variables.h"
#include "mcrl2/core/find.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/sequence_substitution.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/atermpp/detail/aterm_list_utility.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
//
/// \brief Concatenates two sequences of PBES equations
/// \param p A sequence of PBES equations
/// \param q A sequence of PBES equations
/// \return The concatenation result
inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const atermpp::vector<pbes_equation>& q)
{
  atermpp::vector<pbes_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}
/// \endcond

/// \cond INTERNAL_DOCS
//
/// \brief Appends a PBES equation to a sequence of PBES equations
/// \param p A sequence of PBES equations
/// \param e A PBES equation
/// \return The append result
inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const pbes_equation& e)
{
  atermpp::vector<pbes_equation> result(p);
  result.push_back(e);
  return result;
}
/// \endcond

/// \cond INTERNAL_DOCS
namespace detail {

  /// \brief Negates a propositional variable
  struct propositional_variable_negator
  {
    const propositional_variable& v_;

    propositional_variable_negator(const propositional_variable& v)
      : v_(v)
    {}

    /// \brief Function call operator
    /// \param t A propositional variable instantiation
    /// \return The function result
    pbes_expression operator()(propositional_variable_instantiation t) const
    {
      if (t.name() == v_.name())
      {
        return pbes_expr::not_(t);
      }
      else
      {
        return t;
      }
    }
  };

  inline
  std::string print(const atermpp::vector<pbes_equation>& v)
  {
    atermpp::vector<atermpp::aterm_appl> l;
    for (atermpp::vector<pbes_equation>::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      l.push_back(pbes_equation_to_aterm(*i));
    }
    return core::pp(data::convert<atermpp::aterm_list>(l));
  }

} // namespace detail
/// \endcond

/// \brief Abstract algorithm class for translating a state formula and a specification to a pbes.
class pbes_translate_algorithm
{
  protected:
    /// \brief The Par function of the translation
    /// \param x A
    /// \param l A sequence of data variables
    /// \param f A modal formula
    /// \return The function result
    data::variable_list Par(core::identifier_string x, data::variable_list l, state_formulas::state_formula f)
    {
      using namespace state_formulas::state_frm;
      using state_formulas::state_frm::is_variable;
      using atermpp::detail::operator+;
        
      data::variable_list result;
        
      if (is_data(f)) {
        // result = data::variable_list();
      } else if (is_true(f)) {
        // result = data::variable_list();
      } else if (is_false(f)) {
        // result = data::variable_list();
      } else if (is_not(f)) {
        result = Par(x, l, arg(f));
      } else if (is_and(f)) {
        result = Par(x, l, left(f)) + Par(x, l, right(f));
      } else if (is_or(f)) {
        result = Par(x, l, left(f)) + Par(x, l, right(f));
      } else if (is_imp(f)) {
        result = Par(x, l, left(f)) + Par(x, l, right(f));
      } else if (is_must(f)) {
        result = Par(x, l, arg(f));
      } else if (is_may(f)) {
        result = Par(x, l, arg(f));
      } else if (is_forall(f)) {
        result = Par(x, l + var(f), arg(f));
      } else if (is_exists(f)) {
        result = Par(x, l + var(f), arg(f));
      } else if (is_variable(f)) {
        result = data::variable_list();
      } else if (is_mu(f) || (is_nu(f))) {
        if (name(f) == x)
        {
          result = l;
        }
        else
        {
          data::variable_list xf = detail::mu_variables(f);
          state_formulas::state_formula g = arg3(f);
          result = Par(x, l + xf, g);
        }
      } else if (is_yaled_timed(f)) {
        result = data::variable_list();
      } else if (is_delay_timed(f)) {
        result = data::variable_list();
      }
      else
      {
        assert(false);
      }
#ifdef MCRL2_PBES_TRANSLATE_PAR_DEBUG
std::cerr << "\n<Par>(" << core::pp(x) << ", " << core::pp(l) << ", " << core::pp(f) << ") = " << core::pp(result) << std::endl;
#endif
      return result;
    }

    /// \brief Renames data variables and predicate variables in the formula \p f, and
    /// wraps the formula inside a 'nu' if needed. This is needed as a preprocessing
    /// step for the algorithm.
    /// \param formula A modal formula
    /// \param spec A linear process specification
    /// \return The preprocessed formula
    state_formulas::state_formula preprocess_formula(const state_formulas::state_formula& formula, const lps::specification& spec)
    {
      using namespace detail;
      using namespace state_formulas::state_frm;
      using state_formulas::state_frm::is_variable;

      state_formulas::state_formula f = formula;
      std::set<core::identifier_string> formula_variable_names = data::detail::find_variable_names(formula);
      std::set<core::identifier_string> spec_variable_names = data::detail::find_variable_names(specification_to_aterm(spec, false));
      std::set<core::identifier_string> spec_names = core::find_identifiers(specification_to_aterm(spec, false));

      // rename data variables in f, to prevent name clashes with data variables in spec
      data::set_identifier_generator generator;
      generator.add_identifiers(spec_variable_names);
      f = state_formulas::rename_variables(f, generator);

      // rename predicate variables in f, to prevent name clashes
      data::xyz_identifier_generator xyz_generator;
      xyz_generator.add_identifiers(spec_names);
      xyz_generator.add_identifiers(formula_variable_names);
      f = rename_predicate_variables(f, xyz_generator);

      // wrap the formula inside a 'nu' if needed
      if (!is_mu(f) && !is_nu(f))
      {
        atermpp::aterm_list context = make_list(f, specification_to_aterm(spec, false));
        core::identifier_string X = data::fresh_identifier(context, std::string("X"));
        f = nu(X, data::assignment_list(), f);
      }

      return f;
    }

  public:
    /// \brief Constructor.
    pbes_translate_algorithm()
    {}

    /// \brief Destructor.
    virtual ~pbes_translate_algorithm()
    {}

    /// \brief Runs the algorithm
    /// \param formula A state formula
    /// \param spec A specification
    /// \return The result of the translation
    virtual pbes<> run(const state_formulas::state_formula& formula, const lps::specification& spec) = 0;
};

/// \brief Algorithm for translating a state formula and a timed specification to a pbes.
class pbes_translate_algorithm_timed: public pbes_translate_algorithm
{
  protected:

    /// \brief The \p sat_top function of the translation
    /// \param a A timed multi-action
    /// \param b An action formula
    /// \return The function result
    pbes_expression sat_top(const lps::multi_action& a, action_formulas::action_formula b)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<sat>" << a.to_string() << " " << pp(b) << std::flush;
#endif
      using namespace action_formulas::act_frm;
      using namespace action_formulas::accessors;
      namespace d = data;
      namespace p = pbes_expr_optimized;

      pbes_expression result;

      if (is_mult_act(b)) {
        result = lps::equal_multi_actions(a, lps::multi_action(mult_params(b)));
      } else if (is_true(b)) {
        result = p::true_();
      } else if (is_false(b)) {
        result = p::false_();
      } else if (is_data(b)) {
        result = b;
      } else if (is_at(b)) {
        data::data_expression t = a.time();
        action_formulas::action_formula alpha = arg(b);
        data::data_expression t1 = time(b);
        result = p::and_(sat_top(a, alpha), d::equal_to(t, t1));
      } else if (is_not(b)) {
        result = p::not_(sat_top(a, arg(b)));
      } else if (is_and(b)) {
        result = p::and_(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_or(b)) {
        result = p::or_(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_imp(b)) {
        result = p::imp(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_forall(b)) {
        data::variable_list x = var(b);
        assert(x.size() > 0);
        action_formulas::action_formula alpha = arg(b);
        data::variable_list y = data::convert< data::variable_list >(fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), a.time(), b))));
        result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      } else if (is_exists(b)) {
        data::variable_list x = var(b);
        assert(x.size() > 0);
        action_formulas::action_formula alpha = arg(b);
        data::variable_list y = data::convert< data::variable_list >(fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), a.time(), b))));
        result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      } else {
        throw mcrl2::runtime_error(std::string("sat_top[timed] error: unknown lps::action formula ") + b.to_string());
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<satresult>" << pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p RHS function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param T A data variable
    /// \param context A set of strings that may not be used for naming a fresh variable
    /// \return The function result
    pbes_expression RHS(state_formulas::state_formula f0, state_formulas::state_formula f, lps::linear_process lps, data::variable T, std::set<std::string>& context)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<RHS>" << pp(f) << std::flush;
#endif
      using namespace pbes_expr_optimized;
      using namespace pbes_system::accessors;
      using lps::summand_list;
      namespace s = state_formulas::state_frm;
      namespace d = data;
      using atermpp::detail::operator+;

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
          result = and_(RHS(f0, s::left(f), lps, T, context), RHS(f0, s::right(f), lps, T, context));
        } else if (s::is_or(f)) {
          result = or_(RHS(f0, s::left(f), lps, T, context), RHS(f0, s::right(f), lps, T, context));
        } else if (s::is_imp(f)) {
          // TODO: generalize
          // result = imp(RHS(f0, s::left(f), lps, T, context), RHS(f0, s::right(f), lps, T, context));
          result = or_(RHS(f0, s::not_(s::left(f)), lps, T, context), RHS(f0, s::right(f), lps, T, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::forall(s::var(f), RHS(f0, s::arg(f), lps, T, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::exists(s::var(f), RHS(f0, s::arg(f), lps, T, context));
        } else if (s::is_must(f)) {
          atermpp::vector<pbes_expression> v;
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::multi_action ai = i->multi_action();
            data::assignment_list gi = i->assignments();
            data::variable_list xp(lps.process_parameters());
            data::variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, phi, lps, T, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::variable_list y = data::convert< data::variable_list >(fresh_variables(yi, context));
            ci = make_double_sequence_substitution_adaptor(yi, y)(ci);
            ai = ai.substitute(make_double_sequence_substitution_adaptor(yi, y));
            gi = make_double_sequence_substitution_adaptor(yi, y)(gi);
            data::data_expression ti = ai.time();

            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = ci;
            pbes_expression p3 = d::greater(ti, T);
            rhs = rhs.substitute(make_substitution(T, ti));
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = pbes_expr::forall(y, imp(and_(and_(p1, p2), p3), rhs));
            v.push_back(p);
          }
          result = join_and(v.begin(), v.end());
        } else if (s::is_may(f)) {
          atermpp::vector<pbes_expression> v;
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::multi_action ai = i->multi_action();
            data::assignment_list gi = i->assignments();
            data::variable_list xp(lps.process_parameters());
            data::variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, phi, lps, T, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::variable_list y = data::convert< data::variable_list >(fresh_variables(yi, context));
            ci = make_double_sequence_substitution_adaptor(yi, y)(ci);
            ai = ai.substitute(make_double_sequence_substitution_adaptor(yi, y));
            gi = make_double_sequence_substitution_adaptor(yi, y)(gi);
            data::data_expression ti = ai.time();

            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = ci;
            pbes_expression p3 = d::greater(ti, T);
            rhs = rhs.substitute(make_substitution(T, ti));
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = pbes_expr::exists(y, and_(and_(and_(p1, p2), p3), rhs));
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
            data::variable_list yk = i->summation_variables();
            pbes_expression p = pbes_expr::exists(yk, and_(ck, d::less_equal(t, tk)));
            v.push_back(p);
          }
          result = or_(join_or(v.begin(), v.end()), d::less_equal(t, T));
        } else if (s::is_yaled_timed(f)) {
          data::data_expression t = s::time(f);
          atermpp::vector<pbes_expression> v;
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            data::data_expression ck(i->condition());
            data::data_expression tk(i->time());
            data::variable_list yk = i->summation_variables();
            pbes_expression p = pbes_expr::exists(yk, and_(data::sort_bool::not_(ck), d::greater(t, tk)));
            v.push_back(p);
          }
          result = and_(join_or(v.begin(), v.end()), d::greater(t, T));
        } else if (s::is_variable(f)) {
          core::identifier_string X = s::name(f);
          data::data_expression_list d = s::param(f);
          data::variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, T + d + xp + Par(X, data::variable_list(), f0));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::name(f);
          data::data_expression_list d = detail::mu_expressions(f);
          data::variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, T + d + xp + Par(X, data::variable_list(), f0));
        } else {
          throw mcrl2::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = s::arg(f);
        if (s::is_data(f)) {
          result = pbes_expression(data::sort_bool::not_(f));
        } else if (s::is_true(f)) {
          result = false_();
        } else if (s::is_false(f)) {
          result = true_();
        } else if (s::is_not(f)) {
          result = RHS(f0, s::arg(f), lps, T, context);
        } else if (s::is_and(f)) {
          result = or_(RHS(f0, s::not_(s::left(f)), lps, T, context), RHS(f0, s::not_(s::right(f)), lps, T, context));
        } else if (s::is_or(f)) {
          result = and_(RHS(f0, s::not_(s::left(f)), lps, T, context), RHS(f0, s::not_(s::right(f)), lps, T, context));
        } else if (s::is_imp(f)) {
          result = and_(RHS(f0, s::left(f), lps, T, context), RHS(f0, s::not_(s::right(f)), lps, T, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::forall(s::var(f), RHS(f0, s::not_(s::arg(f)), lps, T, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::exists(s::var(f), RHS(f0, s::not_(s::arg(f)), lps, T, context));
        } else if (s::is_must(f)) {
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          result = RHS(f0, s::may(alpha, s::not_(phi)), lps, T, context);
        } else if (s::is_may(f)) {
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          result = RHS(f0, s::must(alpha, s::not_(phi)), lps, T, context);
        } else if (s::is_delay_timed(f)) {
          data::data_expression t = s::time(f);
          result = RHS(f0, s::yaled_timed(t), lps, T, context);
        } else if (s::is_yaled_timed(f)) {
          data::data_expression t = s::time(f);
          result = RHS(f0, s::delay_timed(t), lps, T, context);
        } else if (s::is_variable(f)) {
          result = not_(RHS(f0, f, lps, T, context));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::name(f);
          data::assignment_list xf = s::ass(f);
          state_formulas::state_formula phi = s::arg(f);
          if (s::is_mu(f))
          {
            result = RHS(f0, s::mu(X, xf, s::not_(phi)), lps, T, context);
          }
          else
          {
            result = RHS(f0, s::nu(X, xf, s::not_(phi)), lps, T, context);
          }
        } else {
          throw mcrl2::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<RHSresult>" << pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p E function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param T A data variable
    /// \return The function result
    atermpp::vector<pbes_equation> E(state_formulas::state_formula f0, state_formulas::state_formula f, lps::linear_process lps, data::variable T)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<E>" << pp(f) << std::flush;
#endif
      using namespace state_formulas::state_frm;
      using state_formulas::state_frm::is_variable;
      using namespace data;
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
          result = E(f0, not_(left(f)), lps, T) + E(f0, right(f), lps, T);
        } else if (is_or(f)) {
          result = E(f0, left(f), lps, T) + E(f0, right(f), lps, T);
        } else if (is_imp(f)) {
          result = E(f0, not_(left(f)), lps, T) + E(f0, right(f), lps, T);
        } else if (is_forall(f)) {
          result = E(f0, arg(f), lps, T);
        } else if (is_exists(f)) {
          result = E(f0, arg(f), lps, T);
        } else if (is_must(f)) {
          result = E(f0, arg(f), lps, T);
        } else if (is_may(f)) {
          result = E(f0, arg(f), lps, T);
        } else if (is_variable(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = name(f);
          data::variable_list xf = detail::mu_variables(f);
          data::variable_list xp = lps.process_parameters();
          state_formulas::state_formula g = arg(f);
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
          propositional_variable v(X, T + xf + xp + Par(X, data::variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, T, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps, T);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw mcrl2::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = arg(f);
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_not(f)) {
          result = E(f0, arg(f), lps, T);
        } else if (is_and(f)) {
          result = E(f0, not_(left(f)), lps, T) + E(f0, not_(right(f)), lps, T);
        } else if (is_or(f)) {
          result = E(f0, not_(left(f)), lps, T) + E(f0, not_(right(f)), lps, T);
        } else if (is_imp(f)) {
          result = E(f0, left(f), lps, T) + E(f0, not_(right(f)), lps, T);
        } else if (is_forall(not_(f))) {
          result = E(f0, arg(f), lps, T);
        } else if (is_exists(not_(f))) {
          result = E(f0, arg(f), lps, T);
        } else if (is_must(f)) {
          result = E(f0, not_(arg(f)), lps, T);
        } else if (is_may(f)) {
          result = E(f0, not_(arg(f)), lps, T);
        } else if (is_variable(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = name(f);
          data::variable_list xf = detail::mu_variables(f);
          data::variable_list xp = lps.process_parameters();
          state_formulas::state_formula g = not_(arg(f));
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
          propositional_variable v(X, T + xf + xp + Par(X, data::variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = replace_propositional_variables(RHS(f0, g, lps, T, context), detail::propositional_variable_negator(v));
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps, T);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw mcrl2::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<Eresult>" << detail::print(result) << std::flush;
#endif
      return result;
    }

  public:
    /// \brief Constructor.
    pbes_translate_algorithm_timed()
    {}

    /// \brief Runs the translation algorithm
    /// \param formula A modal formula
    /// \param spec A linear process specification
    /// \return The result of the translation
    pbes<> run(const state_formulas::state_formula& formula, const lps::specification& spec)
    {
      using namespace state_formulas::state_frm;
      using state_formulas::state_frm::is_variable;
      using atermpp::detail::operator+;

      lps::linear_process lps = spec.process();

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      state_formulas::state_formula f = preprocess_formula(formula, spec);

      // make sure the lps is timed
      data::variable T = fresh_variable(make_list(f, lps::linear_process_to_aterm(lps)), data::sort_real::real_(), "T");
      atermpp::aterm_list context = make_list(T, spec.initial_process(), lps::linear_process_to_aterm(lps), f);
      lps = lps::detail::make_timed_lps(lps, context);

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps, T);

      // compute initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(is_mu(f) || is_nu(f));
      core::identifier_string Xf = name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state();
      propositional_variable_instantiation init(Xe, data::sort_real::real_(0) + fi + pi + Par(Xf, data::variable_list(), f));

      // add sort real to data_spec (if needed)
      data::data_specification data_spec(spec.data());
      data_spec.add_sort(data::sort_real::real_());

      pbes<> result(data_spec, e, spec.global_variables(), init);
      result.normalize();
      assert(result.is_normalized());
      assert(result.is_closed());
      complete_data_specification(result);
      return result;
    }
};

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class pbes_translate_algorithm_untimed: public pbes_translate_algorithm
{
  protected:

    /// \brief The \p sat_top function of the translation
    /// \param a A sequence of actions
    /// \param b An action formula
    /// \return The function result
    pbes_expression sat_top(const lps::multi_action& a, action_formulas::action_formula b)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<sat>" << a.to_string() << " " << pp(b) << std::flush;
#endif
      using namespace action_formulas::act_frm;
      using namespace action_formulas::accessors;
      namespace p = pbes_expr_optimized;

      pbes_expression result;

      if (is_mult_act(b)) {
        result = lps::equal_multi_actions(a, lps::multi_action(mult_params(b)));
      } else if (is_true(b)) {
        result = p::true_();
      } else if (is_false(b)) {
        result = p::false_();
      } else if (is_data(b)) {
        result = b;
      } else if (is_not(b)) {
        result = p::not_(sat_top(a, arg(b)));
      } else if (is_and(b)) {
        result = p::and_(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_or(b)) {
        result = p::or_(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_imp(b)) {
        result = p::imp(sat_top(a, left(b)), sat_top(a, right(b)));
      } else if (is_forall(b)) {
        data::variable_list x = var(b);
        action_formulas::action_formula alpha = arg(b);
        if (x.size() > 0)
        {
          data::variable_list y = data::convert< data::variable_list >(fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), b))));
          result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
        }
        else
          result = sat_top(a, alpha);
      } else if (is_exists(b)) {
        data::variable_list x = var(b);
        action_formulas::action_formula alpha = arg(b);
        if (x.size() > 0)
        {
          data::variable_list y = data::convert< data::variable_list >(fresh_variables(x, data::detail::find_variable_name_strings(make_list(a.actions(), b))));
          result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
        }
        else
          result = sat_top(a, alpha);
      } else {
        throw mcrl2::runtime_error(std::string("sat_top[untimed] error: unknown lps::action formula ") + b.to_string());
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<satresult>" << pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p RHS function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param context A set of strings that may not be used for naming a fresh variable
    /// \return The function result
    pbes_expression RHS(state_formulas::state_formula f0, state_formulas::state_formula f, lps::linear_process lps, std::set<std::string>& context)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<RHS>" << pp(f) << std::flush;
#endif
      using namespace pbes_expr_optimized;
      using namespace data::detail;
      using namespace accessors;
      using lps::summand_list;
      using atermpp::detail::operator+;
      namespace s = state_formulas::state_frm;

      pbes_expression result;

    if (!s::is_not(f))
      {
        if (s::is_data(f)) {
          result = pbes_expression(f);
        } else if (s::is_true(f)) {
          result = true_();
        } else if (s::is_false(f)) {
          result = false_();
        } else if (s::is_not(f)) {
          result = not_(RHS(f0, s::arg(f), lps, context));
        } else if (s::is_and(f)) {
          result = and_(RHS(f0, s::left(f), lps, context), RHS(f0, s::right(f), lps, context));
        } else if (s::is_or(f)) {
          result = or_(RHS(f0, s::left(f), lps, context), RHS(f0, s::right(f), lps, context));
        } else if (s::is_imp(f)) {
          // DANGEROUS! result = imp(RHS(f0, s::left(f), lps, context), RHS(f0, s::right(f), lps, context));
          result = or_(RHS(f0, s::not_(s::left(f)), lps, context), RHS(f0, s::right(f), lps, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::forall(s::var(f), RHS(f0, s::arg(f), lps, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::exists(s::var(f), RHS(f0, s::arg(f), lps, context));
        } else if (s::is_must(f)) {
          atermpp::vector<pbes_expression> v;
          action_formulas::action_formula alpha(s::act(f));
          state_formulas::state_formula phi(s::arg(f));
          for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::action_list ai(i->actions());
            data::assignment_list gi = i->assignments();
            data::variable_list xp(lps.process_parameters());
            data::variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, phi, lps, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::variable_list y = data::convert< data::variable_list >(fresh_variables(yi, context));
            ci = make_double_sequence_substitution_adaptor(yi, y)(ci);
            ai = ai.substitute(make_double_sequence_substitution_adaptor(yi, y));
            gi = make_double_sequence_substitution_adaptor(yi, y)(gi);
            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = ci;
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = pbes_expr::forall(y, imp(and_(p1, p2), rhs));
            v.push_back(p);
          }
          result = join_and(v.begin(), v.end());
        } else if (s::is_may(f)) {
          atermpp::vector<pbes_expression> v;
          action_formulas::action_formula alpha(s::act(f));
          state_formulas::state_formula phi(s::arg(f));
          for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
          {
            if (i->is_delta())
              continue;
            data::data_expression ci(i->condition());
            lps::action_list ai(i->actions());
            data::assignment_list gi = i->assignments();
            data::variable_list xp(lps.process_parameters());
            data::variable_list yi(i->summation_variables());

            pbes_expression rhs = RHS(f0, phi, lps, context);
            std::set<std::string> rhs_context = data::detail::find_variable_name_strings(rhs);
            context.insert(rhs_context.begin(), rhs_context.end());
            data::variable_list y = data::convert< data::variable_list >(fresh_variables(yi, context));
            ci = make_double_sequence_substitution_adaptor(yi, y)(ci);
            ai = ai.substitute(make_double_sequence_substitution_adaptor(yi, y));
            gi = make_double_sequence_substitution_adaptor(yi, y)(gi);
            pbes_expression p1 = sat_top(ai, alpha);
            pbes_expression p2 = ci;
            rhs = rhs.substitute(data::assignment_list_substitution(gi));

            pbes_expression p = pbes_expr::exists(y, and_(and_(p1, p2), rhs));
            v.push_back(p);
          }
          result = join_or(v.begin(), v.end());
        } else if (s::is_variable(f)) {
          core::identifier_string X = s::name(f);
          data::data_expression_list d = s::param(f);
          data::variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, d + xp + Par(X, data::variable_list(), f0));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::name(f);
          data::data_expression_list d = detail::mu_expressions(f);
          data::variable_list xp = lps.process_parameters();
          result = propositional_variable_instantiation(X, d + xp + Par(X, data::variable_list(), f0));
        } else {
          throw mcrl2::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = s::arg(f);
        if (s::is_data(f)) {
          result = pbes_expression(data::sort_bool::not_(f));
        } else if (s::is_true(f)) {
          result = false_();
        } else if (s::is_false(f)) {
          result = true_();
        } else if (s::is_not(f)) {
          result = RHS(f0, s::arg(f), lps, context);
        } else if (s::is_and(f)) {
          result = or_(RHS(f0, s::not_(s::left(f)), lps, context), RHS(f0, s::not_(s::right(f)), lps, context));
        } else if (s::is_or(f)) {
          result = and_(RHS(f0, s::not_(s::left(f)), lps, context), RHS(f0, s::not_(s::right(f)), lps, context));
        } else if (s::is_imp(f)) {
          result = and_(RHS(f0, s::left(f), lps, context), RHS(f0, s::not_(s::right(f)), lps, context));
        } else if (s::is_forall(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::forall(s::var(f), RHS(f0, s::not_(s::arg(f)), lps, context));
        } else if (s::is_exists(f)) {
          std::set<std::string> names = data::detail::find_variable_name_strings(s::var(f));
          context.insert(names.begin(), names.end());
          result = pbes_expr::exists(s::var(f), RHS(f0, s::not_(s::arg(f)), lps, context));
        } else if (s::is_must(f)) {
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          result = RHS(f0, s::may(alpha, s::not_(phi)), lps, context);
        } else if (s::is_may(f)) {
          action_formulas::action_formula alpha = s::act(f);
          state_formulas::state_formula phi = s::arg(f);
          result = RHS(f0, s::must(alpha, s::not_(phi)), lps, context);
        } else if (s::is_delay(f)) {
          result = RHS(f0, s::yaled(), lps, context);
        } else if (s::is_yaled(f)) {
          result = RHS(f0, s::delay(), lps, context);
        } else if (s::is_variable(f)) {
          result = not_(RHS(f0, f, lps, context));
        } else if (s::is_mu(f) || (s::is_nu(f))) {
          core::identifier_string X = s::name(f);
          data::assignment_list xf = s::ass(f);
          state_formulas::state_formula phi = s::arg(f);
          if (s::is_mu(f))
          {
            result = RHS(f0, s::mu(X, xf, s::not_(phi)), lps, context);
          }
          else
          {
            result = RHS(f0, s::nu(X, xf, s::not_(phi)), lps, context);
          }
        } else {
          throw mcrl2::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<RHSresult>" << pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p E function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \return The function result
    atermpp::vector<pbes_equation> E(state_formulas::state_formula f0, state_formulas::state_formula f, lps::linear_process lps)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<E>" << pp(f) << std::flush;
#endif
      using namespace state_formulas::state_frm;
      using namespace data;
      using state_formulas::state_frm::is_variable;
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
          result = E(f0, arg(f), lps);
        } else if (is_and(f)) {
          result = E(f0, left(f), lps) + E(f0, right(f), lps);
        } else if (is_or(f)) {
          result = E(f0, left(f), lps) + E(f0, right(f), lps);
        } else if (is_imp(f)) {
          result = E(f0, not_(left(f)), lps) + E(f0, right(f), lps);
        } else if (is_forall(f)) {
          result = E(f0, arg(f), lps);
        } else if (is_exists(f)) {
          result = E(f0, arg(f), lps);
        } else if (is_must(f)) {
          result = E(f0, arg(f), lps);
        } else if (is_may(f)) {
          result = E(f0, arg(f), lps);
        } else if (is_variable(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = name(f);
          data::variable_list xf = detail::mu_variables(f);
          data::variable_list xp = lps.process_parameters();
          state_formulas::state_formula g = arg(f);
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
          propositional_variable v(X, xf + xp + Par(X, data::variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = RHS(f0, g, lps, context);
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps);
        } else if (is_yaled(f)) {
          // do nothing
        } else if (is_delay(f)) {
          // do nothing
        } else {
          throw mcrl2::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
        }
      }
      else // the formula is a negation
      {
        f = arg(f);
        if (is_data(f)) {
          // do nothing
        } else if (is_true(f)) {
          // do nothing
        } else if (is_false(f)) {
          // do nothing
        } else if (is_not(f)) {
          result = E(f0, arg(f), lps);
        } else if (is_and(f)) {
          result = E(f0, not_(left(f)), lps) + E(f0, not_(right(f)), lps);
        } else if (is_or(f)) {
          result = E(f0, not_(left(f)), lps) + E(f0, not_(right(f)), lps);
        } else if (is_imp(f)) {
          result = E(f0, left(f), lps) + E(f0, not_(right(f)), lps);
        } else if (is_forall(not_(f))) {
          result = E(f0, arg(f), lps);
        } else if (is_exists(not_(f))) {
          result = E(f0, arg(f), lps);
        } else if (is_must(f)) {
          result = E(f0, not_(arg(f)), lps);
        } else if (is_may(f)) {
          result = E(f0, not_(arg(f)), lps);
        } else if (is_variable(f)) {
          // do nothing
        } else if (is_mu(f) || (is_nu(f))) {
          core::identifier_string X = name(f);
          data::variable_list xf = detail::mu_variables(f);
          data::variable_list xp = lps.process_parameters();
          state_formulas::state_formula g = not_(arg(f));
          fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
          propositional_variable v(X, xf + xp + Par(X, data::variable_list(), f0));
          std::set<std::string> context;
          pbes_expression expr = replace_propositional_variables(RHS(f0, g, lps, context), detail::propositional_variable_negator(v));
          pbes_equation e(sigma, v, expr);
          result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps);
        } else if (is_yaled_timed(f)) {
          // do nothing
        } else if (is_delay_timed(f)) {
          // do nothing
        } else {
          throw mcrl2::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
        }
      }
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
std::cerr << "\n<Eresult>" << detail::print(result) << std::flush;
#endif
      return result;
    }

  public:
    /// \brief Constructor.
    pbes_translate_algorithm_untimed()
    {}

    /// \brief Runs the translation algorithm
    /// \param formula A modal formula
    /// \param spec A linear process specification
    /// \return The result of the translation
    pbes<> run(const state_formulas::state_formula& formula, const lps::specification& spec)
    {
      using namespace state_formulas::state_frm;
      using atermpp::detail::operator+;
      lps::linear_process lps = spec.process();

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      state_formulas::state_formula f = preprocess_formula(formula, spec);

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps);

      // compute the initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(is_mu(f) || is_nu(f));
      core::identifier_string Xf = name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state();
      propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, data::variable_list(), f));

      pbes<> result = pbes<>(spec.data(), e, spec.global_variables(), init);
      result.normalize();
      assert(result.is_normalized());
      assert(result.is_closed());
      complete_data_specification(result);
      return result;
    }
};

/// \brief Translates a state_formulas::state_formula and a lps::specification to a pbes. If the pbes evaluates
/// to true, the formula holds for the lps::specification.
/// \param formula A modal formula
/// \param spec A linear process specification
/// \param timed determines whether the timed or untimed variant of the algorithm is chosen
/// \return The resulting pbes
inline pbes<> pbes_translate(const state_formulas::state_formula& formula, const lps::specification& spec, bool timed = false)
{
  if (formula.has_time() || spec.process().has_time())
  {
    timed = true;
  }

  if (timed)
  {
    pbes_translate_algorithm_timed algorithm;
    return algorithm.run(formula, spec);
  }
  else
  {
    pbes_translate_algorithm_untimed algorithm;
    return algorithm.run(formula, spec);
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_TRANSLATE_H
