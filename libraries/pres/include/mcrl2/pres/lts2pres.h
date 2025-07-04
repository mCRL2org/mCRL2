// Author(s): Jan Friso Groote. Based on lts2pbes by Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/lts2pres.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_LTS2PRES_H
#define MCRL2_PRES_LTS2PRES_H

#include "mcrl2/modal_formula/count_fixpoints.h"
#include "mcrl2/pres/lps2pres.h"
#include "mcrl2/pres/detail/lts2pres_e.h"

namespace mcrl2::pres_system
{

/// \brief Algorithm for translating a state formula and an untimed specification to a pres.
class lts2pres_algorithm
{
  public:
    using state_type = lts::probabilistic_lts_lts_t::states_size_type;
    using probabilistic_state_type = lts::probabilistic_lts_lts_t::probabilistic_state_t;
    using edge_list = pres_system::detail::lts2pres_lts::edge_list;

  protected:
    const lts::probabilistic_lts_lts_t& lts0;
    pres_system::detail::lts2pres_lts lts1;
    utilities::progress_meter m_progress_meter;
    data::set_identifier_generator m_id_generator;

    template <typename Parameters>
    void run(const state_formulas::state_formula& f, std::vector<pres_equation>& equations, Parameters& parameters)
    {
      detail::E_lts2pres(f, parameters, equations, core::term_traits_optimized<pres_expression>());
    }

  public:
    /// \brief Constructor.
    explicit lts2pres_algorithm(const lts::probabilistic_lts_lts_t& l)
      : lts0(l), lts1(l)
    {}

    /// \brief Runs the translation algorithm
    /// \param formspec A state formula specification.
    /// \param preprocess_modal_operators A boolean indicating that the modal operators can be preprocessed
    //                                    for a more compact translation.
    /// \return The result of the translation
    pres run(const state_formulas::state_formula_specification& formspec,
             bool preprocess_modal_operators = false
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
      std::vector<pres_equation> equations;
      detail::lts2pres_parameters parameters(f, lts0, lts1, m_id_generator, m_progress_meter);
      run(f, equations, parameters);

      // compute the initial state
      probabilistic_state_type s0 = lts0.initial_probabilistic_state();
      data::data_expression_list e = detail::mu_expressions(f);
      propositional_variable_instantiation init;
      const std::string& Xs0 = static_cast<const std::string&>(detail::mu_name(f));
      if (s0.size()<=1)
      {
        init = propositional_variable_instantiation(Xs0 + "'" + std::to_string(s0.get()), e);
      }
      else
      {
        pres_expression initial_state;
        bool defined=false;
        for(probabilistic_state_type::const_reverse_iterator i=s0.rbegin(); i!=s0.rend(); ++i)
        {
          if (!defined)
          {
            initial_state = pres_system::const_multiply(i->probability(), propositional_variable_instantiation(Xs0 + "'" + std::to_string(i->state()),e));
            defined=true;
          }
          else
          {
            initial_state = plus(pres_system::const_multiply(i->probability(), 
                                                             propositional_variable_instantiation(Xs0 + "'" + std::to_string(i->state()), e)), 
                                 initial_state);
          }
        }
        init = propositional_variable_instantiation(detail::mu_name(f),data::data_expression_list());
        equations.emplace_back(fixpoint_symbol::mu(),
            propositional_variable(detail::mu_name(f), data::variable_list()),
            initial_state);
      }

      return pres(lts0.data(), equations, init);
    }
};

/// \brief Translates an LTS and a modal formula into a PRES that represents the corresponding
/// model checking problem.
/// \param l A labelled transition system.
/// \param formspec A modal formula specification.
/// \param preprocess_modal_operators A boolean indicating that modal operators must be preprocessed.
inline
pres lts2pres(const lts::probabilistic_lts_lts_t& l, const state_formulas::state_formula_specification& formspec, bool preprocess_modal_operators = false)
{
  lts2pres_algorithm algorithm(l);
  return algorithm.run(formspec, preprocess_modal_operators);
}

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_LTS2PRES_H
