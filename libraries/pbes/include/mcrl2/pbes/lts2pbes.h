// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/lts2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_LTS2PBES_H
#define MCRL2_PBES_LTS2PBES_H

#include "mcrl2/modal_formula/count_fixpoints.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/detail/lts2pbes_e.h"

namespace mcrl2::pbes_system {

/// \brief Algorithm for translating a state formula and an untimed specification to a pbes.
class lts2pbes_algorithm
{
  public:
    using state_type = lts::lts_lts_t::states_size_type;
    using edge_list = pbes_system::detail::lts2pbes_lts::edge_list;

  protected:
    const lts::lts_lts_t& lts0;
    pbes_system::detail::lts2pbes_lts lts1;
    utilities::progress_meter m_progress_meter;
    data::set_identifier_generator m_id_generator;

    template <typename Parameters>
    void run(const state_formulas::state_formula& f, std::vector<pbes_equation>& equations, Parameters& parameters)
    {
      detail::E_lts2pbes(f, parameters, equations, core::term_traits_optimized<pbes_expression>());
    }

  public:
    /// \brief Constructor.
    explicit lts2pbes_algorithm(const lts::lts_lts_t& l)
      : lts0(l), lts1(l)
    {}

    /// \brief Runs the translation algorithm
    /// \param formspec A state formula specification.
    /// \param preprocess_modal_operators A boolean indicating that the modal operators can be preprocessed
    //                                    for a more compact translation.
    /// \param generate_counter_example A boolean indicating whether a counterexample must be generated.
    /// \return The result of the translation
    pbes run(const state_formulas::state_formula_specification& formspec,
             bool preprocess_modal_operators = false,
             bool generate_counter_example = false
            )
    {
      // TODO: extract identifiers from the LTS(?)
      std::set<core::identifier_string> lts_ids;
      state_formulas::state_formula f = state_formulas::preprocess_state_formula(formspec.formula(), lts_ids, preprocess_modal_operators);

      // initialize progress meter
      std::size_t num_fixpoints = state_formulas::count_fixpoints(f);
      std::size_t num_steps = num_fixpoints * lts1.state_count();
      m_progress_meter.set_size(num_steps);
      mCRL2log(log::verbose) << "Generating " << num_steps << " equations." << std::endl;

      // compute the equations
      std::vector<pbes_equation> equations;
      if (generate_counter_example)
      {
        detail::lts2pbes_counter_example_parameters parameters(f, lts0, lts1, m_id_generator, m_progress_meter);
        run(f, equations, parameters);
        equations = equations + parameters.equations();
      }
      else
      {
        detail::lts2pbes_parameters parameters(f, lts0, lts1, m_id_generator, m_progress_meter);
        run(f, equations, parameters);
      }

      // compute the initial state
      state_type s0 = lts0.initial_state();
      core::identifier_string Xs0 = detail::make_identifier(detail::mu_name(f), s0);
      data::data_expression_list e = detail::mu_expressions(f);
      propositional_variable_instantiation init(Xs0, e);

      return pbes(lts0.data(), equations, init);
    }
};

/// \brief Translates an LTS and a modal formula into a PBES that represents the corresponding
/// model checking problem.
/// \param l A labelled transition system.
/// \param formspec A modal formula specification.
/// \param preprocess_modal_operators A boolean indicating that modal operators must be preprocessed.
/// \param generate_counter_example A boolean indicating that a counter example must be generated.
inline
pbes lts2pbes(const lts::lts_lts_t& l, const state_formulas::state_formula_specification& formspec, bool preprocess_modal_operators = false, bool generate_counter_example = false)
{
  lts2pbes_algorithm algorithm(l);
  return algorithm.run(formspec, preprocess_modal_operators, generate_counter_example);
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_LTS2PBES_H
