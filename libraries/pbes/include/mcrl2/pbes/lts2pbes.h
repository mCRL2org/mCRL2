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

//#define MCRL2_LTS2PBES_DEBUG
#ifdef MCRL2_LTS2PBES_DEBUG
#define MCRL2_PBES_TRANSLATE_DEBUG
#endif

#include <map>
#include <boost/lexical_cast.hpp>
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/modal_formula/count_fixpoints.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/utilities/progress_meter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// custom LTS type that maps states to a vector of outgoing edges
class lts2pbes_lts
{
  public:
    typedef std::size_t state_type;
    typedef std::size_t label_type;

    typedef std::vector<std::pair<label_type, state_type> > edge_list;
    typedef std::map<state_type, edge_list> lts_type;

  protected:
    lts_type m_map;
    atermpp::vector<lps::multi_action> m_action_labels;
    std::size_t m_state_count;
    edge_list m_empty_edge_list;

  public:
    lts2pbes_lts(const lts::lts_lts_t& lts0)
    {
      const std::vector<lts::transition>& transitions = lts0.get_transitions();
      for (std::vector<lts::transition>::const_iterator i = transitions.begin(); i != transitions.end(); ++i)
      {
        state_type s = i->from();
        label_type a = i->label();
        state_type t = i->to();
        m_map[s].push_back(std::make_pair(a, t));
      }

      for (lts::lts_lts_t::labels_size_type i = 0; i < lts0.num_action_labels(); i++)
      {
        lts::detail::action_label_lts a = lts0.action_label(i);
        m_action_labels.push_back(lps::multi_action(a.actions(), a.time()));
      }
      m_state_count = lts0.num_states();
    }

    // returns the outgoing edges of state s
    const edge_list& edges(state_type s) const
    {
      lts_type::const_iterator i = m_map.find(s);
      if (i == m_map.end())
      {
        return m_empty_edge_list;
      }
      else
      {
        return i->second;
      }
    }

    const atermpp::vector<lps::multi_action>& action_labels() const
    {
      return m_action_labels;
    }

    std::size_t state_count() const
    {
      return m_state_count;
    }
};

} // namespace detail

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class lts2pbes_algorithm: public pbes_translate_algorithm_untimed_base
{
  public:
    typedef lts::lts_lts_t::states_size_type state_type;
    typedef pbes_system::detail::lts2pbes_lts::edge_list edge_list;

  protected:
    const lts::lts_lts_t& lts0;
    pbes_system::detail::lts2pbes_lts lts1;
    state_formulas::state_formula f0;
    utilities::progress_meter m_progress_meter;

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
        atermpp::vector<pbes_expression> v;
        action_formulas::action_formula alpha(af::act(f));
        state_formulas::state_formula phi(af::arg(f));

        // traverse all transitions s --a--> t
        const edge_list& m = lts1.edges(s);
        for (edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
        {
          state_type t = i->second;
          const lps::multi_action& a = lts1.action_labels()[i->first];
          v.push_back(imp(sat_top(a, alpha), RHS(phi, t)));
        }
        result = z::join_and(v.begin(), v.end());
      }
      else if (sf::is_may(f))
      {
        atermpp::vector<pbes_expression> v;
        action_formulas::action_formula alpha(af::act(f));
        state_formulas::state_formula phi(af::arg(f));

        // traverse all transitions s --a--> t
        const edge_list& m = lts1.edges(s);
        for (edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
        {
          state_type t = i->second;
          const lps::multi_action& a = lts1.action_labels()[i->first];
          v.push_back(and_(sat_top(a, alpha), RHS(phi, t)));
        }
        result = z::join_or(v.begin(), v.end());
      }
      else if (sf::is_variable(f))
      {
        core::identifier_string X = af::name(f);
        core::identifier_string X_s = make_identifier(X, s);
        data::data_expression_list d = af::param(f);
        result = propositional_variable_instantiation(X_s, d + Par(X, data::variable_list(), f0));
      }
      else if (sf::is_mu(f) || (sf::is_nu(f)))
      {
        core::identifier_string X = af::name(f);
        core::identifier_string X_s = make_identifier(X, s);
        data::data_expression_list e = detail::mu_expressions(f);
        result = propositional_variable_instantiation(X_s, e + Par(X, data::variable_list(), f0));
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
    atermpp::vector<pbes_equation> E(const state_formulas::state_formula& f)
    {
#ifdef MCRL2_LTS2PBES_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<E>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      namespace sf = state_formulas;
      namespace af = state_formulas::detail::accessors;

      atermpp::vector<pbes_equation> result;

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
        atermpp::vector<pbes_equation> v;

        // traverse all states of the LTS
        for (state_type s = 0; s < lts1.state_count(); s++)
        {
          core::identifier_string X = af::name(f);
          core::identifier_string X_s = make_identifier(X, s);
          propositional_variable Xs(X_s, d + Par(X, data::variable_list(), f0));
          v.push_back(pbes_equation(sigma, Xs, RHS(af::arg(f), s)));
          m_progress_meter.step();
        }
        result = v + E(af::arg(f));
      }
      else
      {
        throw mcrl2::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
      }
#ifdef MCRL2_LTS2PBES_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<Eresult>" << pbes_system::pp(result) << std::flush;
#endif

      return result;
    }

  public:
    /// \brief Constructor.
    lts2pbes_algorithm(const lts::lts_lts_t& l)
      : lts0(l), lts1(l)
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

      std::size_t num_fixpoints = state_formulas::count_fixpoints(f0);
      std::size_t num_steps = num_fixpoints * lts1.state_count();
      m_progress_meter.set_size(num_steps);
      mCRL2log(log::verbose) << "Generating " << num_steps << " equations." << std::endl;

      // compute the equations
      atermpp::vector<pbes_equation> eqn = E(f0);

      // compute the initial state
      state_type s0 = lts0.initial_state();
      core::identifier_string Xs0 = make_identifier(af::name(f0), s0);
      data::data_expression_list e = detail::mu_expressions(f0);
      propositional_variable_instantiation init(Xs0, e);

      pbes<> result = pbes<>(lts0.data(), eqn, atermpp::set<data::variable>(), init);
      return result;
    }
};

/// \brief Translates an LTS and a modal formula into a PBES that represents the corresponding
/// model checking problem.
/// \param l A labelled transition system
/// \param f A modal formula
inline
pbes<> lts2pbes(const lts::lts_lts_t& l, const state_formulas::state_formula& f)
{
  lts2pbes_algorithm algorithm(l);
  return algorithm.run(f);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LTS2PBES_H
