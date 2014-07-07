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
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/modal_formula/count_fixpoints.h"
#include "mcrl2/modal_formula/normalize.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/detail/lts2pbes_lts.h"
#include "mcrl2/pbes/detail/lts2pbes_e.h"
#include "mcrl2/utilities/progress_meter.h"
#include "mcrl2/pbes/detail/term_traits_optimized.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class lts2pbes_algorithm
{
  public:
    typedef lts::lts_lts_t::states_size_type state_type;
    typedef pbes_system::detail::lts2pbes_lts::edge_list edge_list;

  protected:
    const lts::lts_lts_t& lts0;
    pbes_system::detail::lts2pbes_lts lts1;
    utilities::progress_meter m_progress_meter;

  public:
    /// \brief Constructor.
    lts2pbes_algorithm(const lts::lts_lts_t& l)
      : lts0(l), lts1(l)
    {}

    /// \brief Runs the translation algorithm
    /// \param formula A modal formula
    /// \return The result of the translation
    pbes run(const state_formulas::state_formula& formula)
    {
      if (!state_formulas::algorithms::is_monotonous(formula))
      {
        throw mcrl2::runtime_error(std::string("lps2pbes error: the formula ") + state_formulas::pp(formula) + " is not monotonous!");
      }

      // resolve name conflicts and wrap the formula in a mu or nu if needed
      mCRL2log(log::debug) << "formula before preprocessing: " << state_formulas::pp(formula) << std::endl;
      lps::specification spec;
      state_formulas::state_formula f = state_formulas::algorithms::preprocess_state_formula(formula, spec);
      mCRL2log(log::debug) << "formula after preprocessing:  " << state_formulas::pp(f) << std::endl;

      // remove occurrences of ! and =>
      if (!state_formulas::algorithms::is_normalized(f))
      {
        f = state_formulas::algorithms::normalize(f);
      }
      mCRL2log(log::debug) << "formula after normalization:  " << state_formulas::pp(f) << std::endl;
      assert(state_formulas::algorithms::is_normalized(f));

      // initialize progress meter
      std::size_t num_fixpoints = state_formulas::count_fixpoints(f);
      std::size_t num_steps = num_fixpoints * lts1.state_count();
      m_progress_meter.set_size(num_steps);
      mCRL2log(log::verbose) << "Generating " << num_steps << " equations." << std::endl;

      // compute the equations
      std::vector<pbes_equation> eqn = detail::E(f, f, lts0, lts1, m_progress_meter, core::term_traits_optimized<pbes_expression>());

      // compute the initial state
      state_type s0 = lts0.initial_state();
      core::identifier_string Xs0 = detail::make_identifier(state_formulas::detail::accessors::name(f), s0);
      data::data_expression_list e = detail::mu_expressions(f);
      propositional_variable_instantiation init(Xs0, e);

      return pbes(lts0.data(), eqn, std::set<data::variable>(), init);
    }
};

/// \brief Translates an LTS and a modal formula into a PBES that represents the corresponding
/// model checking problem.
/// \param l A labelled transition system
/// \param f A modal formula
inline
pbes lts2pbes(const lts::lts_lts_t& l, const state_formulas::state_formula& f)
{
  lts2pbes_algorithm algorithm(l);
  return algorithm.run(f);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LTS2PBES_H
