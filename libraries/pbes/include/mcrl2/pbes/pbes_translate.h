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

#include <iterator>
#include <sstream>
#include <algorithm>
#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/make_timed_lps.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/modal_formula/monotonicity.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_normalize.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/modal_formula/preprocess_state_formula.h"
#include "mcrl2/modal_formula/detail/state_variable_negator.h"
#include "mcrl2/modal_formula/detail/action_formula_accessors.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"
#include "mcrl2/pbes/monotonicity.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/detail/lps2pbes_indenter.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/detail/lps2pbes_rhs.h"
#include "mcrl2/pbes/detail/lps2pbes_e.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Abstract algorithm class for translating a state formula and a specification to a pbes.
class pbes_translate_algorithm
{
  protected:
    /// \brief The Par function of the translation
    /// \param x A
    /// \param l A sequence of data variables
    /// \param f A modal formula
    /// \return The function result
    data::variable_list Par(const core::identifier_string& x, const data::variable_list& l, const state_formulas::state_formula& f)
    {
#ifdef MCRL2_PBES_TRANSLATE_PAR_DEBUG
      std::cerr << "\n<Par>(" << core::pp(x) << ", " << data::pp(l) << ", " << state_formulas::pp(f) << ") = " << data::pp(result) << std::endl;
#endif
      return detail::Par(x, l, f);
    }

    /// \brief The \p sat_top function of the translation
    /// \param a A timed multi-action
    /// \param b An action formula
    /// \return The function result
    pbes_expression sat_top(const lps::multi_action& x, const action_formulas::action_formula& b)
    {
      return detail::Sat(x, b);
    }

  public:
    /// \brief Constructor.
    pbes_translate_algorithm()
    {}
};

/// \brief Algorithm for translating a state formula and a timed specification to a pbes.
class pbes_translate_algorithm_timed: public pbes_translate_algorithm
{
  protected:

    /// \brief The \p RHS function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param T A data variable
    /// \param id_generator A set of strings that may not be used for naming a fresh variable
    /// \return The function result
    pbes_expression RHS(
      state_formulas::state_formula f0,
      state_formulas::state_formula f,
      const lps::linear_process& lps,
      data::variable T,
      data::set_identifier_generator& id_generator
     )
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<RHS timed>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      pbes_expression result = detail::RHS(f0, f, lps, id_generator, T);
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<RHSresult>" << pbes_system::pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p E function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param T A data variable
    /// \return The function result
    atermpp::vector<pbes_equation> E(state_formulas::state_formula f0,
                                     state_formulas::state_formula f,
                                     const lps::linear_process& lps,
                                     data::variable T)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<E timed>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      atermpp::vector<pbes_equation> result = detail::E(f0, f, lps, T);
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<Eresult>" << detail::myprint(result) << std::flush;
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
      using namespace state_formulas::detail::accessors;
      using atermpp::detail::operator+;
      namespace s = state_formulas;

      lps::linear_process lps = spec.process();

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      state_formulas::state_formula f = state_formulas::preprocess_state_formula(formula, spec);

      // make sure the lps is timed
      std::set<core::identifier_string> id_generator = lps::find_identifiers(spec);
      std::set<core::identifier_string> fcontext = state_formulas::find_identifiers(f);
      id_generator.insert(fcontext.begin(), fcontext.end());

      data::variable T = fresh_variable(id_generator, data::sort_real::real_(), "T");
      id_generator.insert(T.name());
      lps::detail::make_timed_lps(lps, id_generator);

      // remove occurrences of ! and =>
      if (!state_formulas::is_normalized(f))
      {
        f = state_formulas::normalize(f);
      }
      assert(state_formulas::is_normalized(f));

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps, T);

      // compute initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(s::is_mu(f) || s::is_nu(f));
      core::identifier_string Xf = name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state(spec.process().process_parameters());
      propositional_variable_instantiation init(Xe, data::sort_real::real_(0) + fi + pi + Par(Xf, data::variable_list(), f));

      pbes<> result(spec.data(), e, spec.global_variables(), init);
      pbes_system::normalize(result);
      assert(pbes_system::is_normalized(result));
      assert(result.is_closed());
      complete_data_specification(result);
      return result;
    }
};

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class pbes_translate_algorithm_untimed: public pbes_translate_algorithm
{
  protected:
    /// \brief The \p RHS function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \param id_generator A set of strings that may not be used for naming a fresh variable
    /// \return The function result
    pbes_expression RHS(state_formulas::state_formula f0,
                        state_formulas::state_formula f,
                        const lps::linear_process& lps,
                        data::set_identifier_generator& id_generator)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<RHS-untimed>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      pbes_expression result = detail::RHS(f0, f, lps, id_generator);
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<RHSresult>" << pbes_system::pp(result) << std::flush;
#endif
      return result;
    }

    /// \brief The \p E function of the translation
    /// \param f0 A modal formula
    /// \param f A modal formula
    /// \param lps A linear process
    /// \return The function result
    atermpp::vector<pbes_equation> E(
      state_formulas::state_formula f0,
      state_formulas::state_formula f,
      const lps::linear_process& lps)
    {
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      std::cerr << "\n" << lps2pbes_indent() << "<E-untimed>" << state_formulas::pp(f) << std::flush;
      lps2pbes_increase_indent();
#endif
      atermpp::vector<pbes_equation> result = detail::E(f0, f, lps);
#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      lps2pbes_decrease_indent();
      std::cerr << "\n" << lps2pbes_indent() << "<Eresult>" << detail::myprint(result) << std::flush;
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
      using namespace state_formulas::detail::accessors;
      using atermpp::detail::operator+;
      lps::linear_process lps = spec.process();

      if (!state_formulas::is_monotonous(formula))
      {
        throw mcrl2::runtime_error(std::string("lps2pbes error: the formula ") + state_formulas::pp(formula) + " is not monotonous!");
      }

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      state_formulas::state_formula f = state_formulas::preprocess_state_formula(formula, spec);

#ifdef MCRL2_PBES_TRANSLATE_DEBUG
      std::cerr << "<preprocessed formula>" << state_formulas::pp(f) << std::endl;
#endif

      // remove occurrences of ! and =>
      if (!state_formulas::is_normalized(f))
      {
        f = state_formulas::normalize(f);
      }
      assert(state_formulas::is_normalized(f));

      // compute the equations
      atermpp::vector<pbes_equation> e = E(f, f, lps);

      // compute the initial state
      assert(e.size() > 0);
      pbes_equation e1 = e.front();
      core::identifier_string Xe(e1.variable().name());
      assert(is_mu(f) || is_nu(f));
      core::identifier_string Xf = name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state(spec.process().process_parameters());
      propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, data::variable_list(), f));

      pbes<> result = pbes<>(spec.data(), e, spec.global_variables(), init);
      assert(is_monotonous(result));
      pbes_system::normalize(result);
      assert(pbes_system::is_normalized(result));
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
  if ((formula.has_time() || spec.process().has_time()) && !timed)
  {
    mCRL2log(log::warning) << "Switch to timed translation because formula has " << (formula.has_time()?"":"no ") << "time, and process has " << (spec.process().has_time()?"":"no ") << "time" << std::endl;
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
