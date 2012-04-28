// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/lts2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_LTS2PBES_H
#define MCRL2_PBES_LTS2PBES_H

#include <boost/lexical_cast.hpp>
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/pbes/pbes_translate.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class lts2pbes_algorithm: public pbes_translate_algorithm_untimed_base
{
  public:
    typedef lts::lts_lts_t::states_size_type state_type;

  protected:
    const lts::lts_lts_t& lts0;
    state_formulas::state_formula f0;

    core::identifier_string make_identifier(const core::identifier_string& name, state_type s)
    {
      return core::identifier_string(std::string(name) + "@" + boost::lexical_cast<std::string>(s));
    }

    /// \brief The \p RHS function of the translation
    /// \param f A modal formula
    /// \param s A state of an LTS
    pbes_expression RHS(const state_formulas::state_formula& f, state_type s)
    {
#ifdef MCRL2_LTS2PBES_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<RHS>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      namespace sf = state_formulas;
      namespace af = state_formulas::detail::accessors;
      namespace z = pbes_expr_optimized;
      using atermpp::detail::operator+;

      pbes_expression result;

      if (data::is_data_expression(f))
      {
        result = pbes_expression(f);
      }
      else if (sf::is_true(f))
      {
        result = z::true_();
      }
      else if (sf::is_false(f))
      {
        result = z::false_();
      }
      else if (sf::is_and(f))
      {
        result = z::and_(RHS(af::left(f), s), RHS(af::right(f), s));
      }
      else if (sf::is_or(f))
      {
        result = z::or_(RHS(af::left(f), s), RHS(af::right(f), s));
      }
      else if (sf::is_forall(f))
      {
        result = pbes_expr::forall(af::var(f), RHS(af::arg(f), s));
      }
      else if (sf::is_exists(f))
      {
        result = pbes_expr::exists(af::var(f), RHS(af::arg(f), s));
      }
      else if (sf::is_must(f))
      {
        std::vector<pbes_expression> v;
        action_formulas::action_formula alpha(af::act(f));
        state_formulas::state_formula phi(af::arg(f));

        // traverse all transitions s --a--> t
        const std::vector<lts::transition> &trans=lts0.get_transitions();
        for( std::vector<lts::transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
        {
          if (s == r->from())
          {
            lts::detail::action_label_lts a = lts0.action_label(r->label());
            state_type t = r->to();
            v.push_back(imp(sat_top(a, alpha), RHS(phi, t)));
          }
        }
        result = z::join_and(v.begin(), v.end());
      }
      else if (sf::is_may(f))
      {
        std::vector<pbes_expression> v;
        action_formulas::action_formula alpha(af::act(f));
        state_formulas::state_formula phi(af::arg(f));

        // traverse all transitions s --a--> t
        const std::vector<lts::transition> &trans=lts0.get_transitions();
        for( std::vector<lts::transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
        {
          if (s == r->from())
          {
            lts::detail::action_label_lts a = lts0.action_label(r->label());
            state_type t = r->to();
            v.push_back(and_(sat_top(a, alpha), RHS(phi, t)));
          }
        }
        result = z::join_or(v.begin(), v.end());
      }
      else if (sf::is_variable(f))
      {
        core::identifier_string X = make_identifier(af::name(f), s);
        data::data_expression_list d = af::param(f);
        result = propositional_variable_instantiation(X, d + Par(X, data::variable_list(), f0));
      }
      else if (sf::is_mu(f) || (sf::is_nu(f)))
      {
        core::identifier_string X = make_identifier(af::name(f), s);
        data::data_expression_list e = detail::mu_expressions(f);
        result = propositional_variable_instantiation(X, e + Par(X, data::variable_list(), f0));
      }
      else
      {
        throw mcrl2::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
      }

#ifdef MCRL2_LTS2PBES_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<RHSresult>" << pbes_system::pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p E function of the translation
    /// \param f A modal formula
    std::vector<pbes_equation> E(const state_formulas::state_formula& f)
    {
#ifdef MCRL2_LTS2PBES_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<E>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      namespace sf = state_formulas;
      namespace af = state_formulas::detail::accessors;

      std::vector<pbes_equation> result;

      if (data::is_data_expression(f))
      {
        // do nothing
      }
      else if (sf::is_true(f))
      {
        // do nothing
      }
      else if (sf::is_false(f))
      {
        // do nothing
      }
      else if (sf::is_and(f))
      {
        result = E(af::left(f)) + E(af::right(f));
      }
      else if (sf::is_or(f))
      {
        result = E(af::left(f)) + E(af::right(f));
      }
      else if (sf::is_forall(f))
      {
        result = E(af::arg(f));
      }
      else if (sf::is_exists(f))
      {
        result = E(af::arg(f));
      }
      else if (sf::is_must(f))
      {
        result = E(af::arg(f));
      }
      else if (sf::is_may(f))
      {
        result = E(af::arg(f));
      }
      else if (sf::is_variable(f))
      {
        // do nothing
      }
      else if (sf::is_mu(f) || (sf::is_nu(f)))
      {
        data::variable_list d = detail::mu_variables(f);
        fixpoint_symbol sigma = sf::is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
        std::vector<pbes_equation> v;

        // traverse all states of the LTS
        for (state_type s = 0; s < lts0.num_states(); s++)
        {
          core::identifier_string X = make_identifier(af::name(f), s);
          propositional_variable Xs(X, d + Par(X, data::variable_list(), f0));
          v.push_back(pbes_equation(sigma, Xs, RHS(af::arg(f), s)));
        }
        result = v + E(af::arg(f));
      }
      else
      {
        throw mcrl2::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
      }
#ifdef MCRL2_LTS2PBES_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<Eresult>" << detail::print(result) << std::flush;
#endif
      return result;
    }

  public:
    /// \brief Constructor.
    lts2pbes_algorithm(const lts::lts_lts_t& l)
      : lts0(l)
    {}

    /// \brief Runs the translation algorithm
    /// \param formula A modal formula
    /// \param spec A linear process specification
    /// \return The result of the translation
    pbes<> run(const state_formulas::state_formula& formula)
    {
      namespace sf = state_formulas;
      namespace af = state_formulas::detail::accessors;

      if (!state_formulas::is_monotonous(formula))
      {
        throw mcrl2::runtime_error(std::string("lps2pbes error: the formula ") + state_formulas::pp(formula) + " is not monotonous!");
      }

      // wrap the formula inside a 'nu' if needed
      if (!sf::is_mu(formula) && !sf::is_nu(formula))
      {
        data::set_identifier_generator generator;
        generator.add_identifiers(state_formulas::find_identifiers(formula));
        core::identifier_string X = generator("X");
        f0 = sf::nu(X, data::assignment_list(), formula);
      }
      else
      {
        f0 = formula;
      }

      // compute the equations
      std::vector<pbes_equation> eqn = E(f0);

      // compute the initial state
      state_type s0 = lts0.initial_state();
      core::identifier_string Xs0 = make_identifier(af::name(f0), s0);
      data::data_expression_list e = detail::mu_expressions(f0);
      propositional_variable_instantiation init(Xs0, e);

      pbes<> result = pbes<>(lts0.data(), eqn, std::set<data::variable>(), init);
      return result;
    }
};

/// \brief Translates an LTS and a modal formula into a PBES that represents the corresponding
/// model checking problem.
/// \param f A modal formula
/// \param s A state of an LTS
inline
pbes<> lts2pbes(const lts::lts_lts_t& l, const state_formulas::state_formula& f)
{
  lts2pbes_algorithm algorithm(l);
  return algorithm.run(f);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LTS2PBES_H
