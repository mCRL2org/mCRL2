// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/lps2pbes.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_LPS2PBES_H
#define MCRL2_PBES_LPS2PBES_H

#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/detail/make_timed_lps.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/preprocess_state_formula.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/lps2pbes_e.h"
#include "mcrl2/pbes/detail/lps2pbes_rhs.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/detail/term_traits_optimized.h"
#include "mcrl2/pbes/is_monotonous.h"
#include "mcrl2/process/merge_action_specifications.h"
#include <string>

namespace mcrl2
{

namespace pbes_system
{

/// \brief Algorithm for translating a state formula and a timed specification to a pbes.
class lps2pbes_algorithm
{
  protected:
    data::set_identifier_generator m_generator;

    template <typename Parameters>
    void run(const state_formulas::state_formula& f, bool structured, bool unoptimized, std::vector<pbes_equation>& equations, Parameters& parameters)
    {
      if (structured)
      {
        if (unoptimized)
        {
          detail::E_structured(f, parameters, m_generator, equations, core::term_traits<pbes_expression>());
        }
        else
        {
          detail::E_structured(f, parameters, m_generator, equations, core::term_traits_optimized<pbes_expression>());
        }
      }
      else
      {
        if (unoptimized)
        {
          detail::E(f, parameters, equations, core::term_traits<pbes_expression>());
        }
        else
        {
          detail::E(f, parameters, equations, core::term_traits_optimized<pbes_expression>());
        }
      }
    }

  public:
    /// \brief Runs the translation algorithm
    /// \param formula A modal formula that represents a property about the system modeled by the given specification
    /// \param lpsspec A linear process specification
    /// \param structured use the 'structured' approach of generating equations
    /// \param unoptimized do not optimize the resulting PBES.
    /// \param preprocess_modal_operators insert dummy fixpoints in modal operators, which may lead to smaller PBESs
    /// \param generate_counter_example If true, then the PBES is enhanced with additional equations that are used to extract a counter example.
    /// \param T The time parameter. If T == data::variable() the untimed version of lps2pbes is applied.
    /// \return A PBES that encodes the property applied to the given specification
    pbes run(const state_formulas::state_formula& formula,
             const lps::specification& lpsspec,
             bool structured = false,
             bool unoptimized = false,
             bool preprocess_modal_operators = false,
             bool generate_counter_example = false,
             const data::variable& T = data::undefined_real_variable()
            )
    {
      using atermpp::detail::operator+;
      state_formulas::state_formula f = formula;

      std::set<core::identifier_string> lps_ids = lps::find_identifiers(lpsspec);
      std::set<core::identifier_string> dataspec_ids = data::function_and_mapping_identifiers(lpsspec.data());
      lps_ids.insert(dataspec_ids.begin(), dataspec_ids.end());
      f = state_formulas::preprocess_state_formula(f, lps_ids, preprocess_modal_operators);

      m_generator.clear_context();
      m_generator.add_identifiers(lps::find_identifiers(lpsspec));
      m_generator.add_identifiers(data::function_and_mapping_identifiers(lpsspec.data()));
      m_generator.add_identifiers(state_formulas::find_identifiers(f));

      std::vector<pbes_equation> equations;
      if (generate_counter_example)
      {
        detail::lps2pbes_counter_example_parameters parameters(f, lpsspec.process(), m_generator, T);
        run(f, structured, unoptimized, equations, parameters);
        equations = equations + parameters.equations();
      }
      else
      {
        detail::lps2pbes_parameters parameters(f, lpsspec.process(), m_generator, T);
        run(f, structured, unoptimized, equations, parameters);
      }

      // compute the initial state
      assert(equations.size() > 0);
      pbes_equation e1 = equations.front();
      core::identifier_string Xe(e1.variable().name());
      assert(state_formulas::is_mu(f) || state_formulas::is_nu(f));
      const core::identifier_string& Xf = detail::mu_name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = lpsspec.initial_process().state(lpsspec.process().process_parameters());
      data::data_expression_list e = fi + pi + detail::Par(Xf, data::variable_list(), f);
      if (T != data::undefined_real_variable())
      {
        e = data::sort_real::real_(0) + e;
      }
      propositional_variable_instantiation init(Xe, e);

      pbes result(lpsspec.data(), equations, lpsspec.global_variables(), init);
      assert(is_monotonous(result));
      pbes_system::algorithms::normalize(result);
      assert(pbes_system::algorithms::is_normalized(result));
      assert(result.is_closed());
      complete_data_specification(result);
      return result;
    }
};

/// \brief Translates a linear process specification and a state formula to a PBES. If the solution of the PBES
///        is true, the formula holds for the specification.
/// \param lpsspec A linear process specification.
/// \param formula A modal formula.
/// \param timed determines whether the timed or untimed variant of the algorithm is chosen.
/// \param structured use the 'structured' approach of generating equations.
/// \param unoptimized if true, the resulting PBES is not simplified, if false (default),
///        the PBES is simplified.
/// \param preprocess_modal_operators A boolean indicating that the modal operators can be preprocessed to
///                                   obtain a more compact PBES.
/// \param generate_counter_example A boolean indicating that a counter example must be generated.
/// \return The resulting pbes.
inline
pbes lps2pbes(const lps::specification& lpsspec,
              const state_formulas::state_formula& formula,
              bool timed = false,
              bool structured = false,
              bool unoptimized = false,
              bool preprocess_modal_operators = false,
              bool generate_counter_example = false
             )
{
  if ((formula.has_time() || lpsspec.process().has_time()) && !timed)
  {
    mCRL2log(log::warning) << "Switch to timed translation because formula has "
                           << (formula.has_time()?"":"no ") << "time, and process has "
                           << (lpsspec.process().has_time()?"":"no ") << "time" << std::endl;
    timed = true;
  }

  if (timed)
  {
    lps::specification lpsspec_timed = lpsspec;
    data::set_identifier_generator generator;
    generator.add_identifiers(lps::find_identifiers(lpsspec));
    generator.add_identifiers(state_formulas::find_identifiers(formula));
    generator.add_identifiers(data::function_and_mapping_identifiers(lpsspec.data()));
    data::variable T(generator("T"), data::sort_real::real_());
    lps::detail::make_timed_lps(lpsspec_timed.process(), generator.context());
    return lps2pbes_algorithm().run(formula, lpsspec_timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example, T);
  }
  else
  {
    return lps2pbes_algorithm().run(formula, lpsspec, structured, unoptimized, preprocess_modal_operators, generate_counter_example);
  }
}

/// \brief Translates a linear process specification and a state formula to a PBES. If the solution of the PBES
///        is true, the formula holds for the specification.
/// \param lpsspec A linear process specification.
/// \param formspec A modal formula specification.
/// \param timed determines whether the timed or untimed variant of the algorithm is chosen.
/// \param structured use the 'structured' approach of generating equations.
/// \param unoptimized if true, the resulting PBES is not simplified, if false (default),
///        the PBES is simplified.
/// \param preprocess_modal_operators A boolean indicating that the modal operators can be preprocessed to
///                                   obtain a more compact PBES.
/// \param generate_counter_example A boolean indicating that a counter example must be generated.
/// \return The resulting pbes.
inline
pbes lps2pbes(const lps::specification& lpsspec,
              const state_formulas::state_formula_specification& formspec,
              bool timed = false,
              bool structured = false,
              bool unoptimized = false,
              bool preprocess_modal_operators = false,
              bool generate_counter_example = false
             )
{
  lps::specification lpsspec1 = lpsspec;
  lpsspec1.data() = data::merge_data_specifications(lpsspec1.data(), formspec.data());
  lpsspec1.action_labels() = process::merge_action_specifications(lpsspec1.action_labels(), formspec.action_labels());
  return lps2pbes(lpsspec1, formspec.formula(), timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example);
}

/// \brief Applies the lps2pbes algorithm.
/// \param spec_text A string.
/// \param formula_text A string.
/// \param timed Determines whether the timed or untimed version of the translation algorithm is used.
/// \param structured use the 'structured' approach of generating equations.
/// \param unoptimized if true, the resulting PBES is not simplified, if false (default),
///        the PBES is simplified.
/// \param preprocess_modal_operators A boolean indicating that the modal operators can be preprocessed to
///                                   obtain a more compact PBES.
/// \param generate_counter_example A boolean indicating that a counter example must be generated.
/// \return The result of the algorithm
inline
pbes lps2pbes(const std::string& spec_text,
              const std::string& formula_text,
              bool timed = false,
              bool structured = false,
              bool unoptimized = false,
              bool preprocess_modal_operators = false,
              bool generate_counter_example = false
             )
{
  pbes result;
  lps::specification lpsspec = remove_stochastic_operators(lps::linearise(spec_text));

  state_formulas::state_formula f = state_formulas::algorithms::parse_state_formula(formula_text, lpsspec);
  return lps2pbes(lpsspec, f, timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LPS2PBES_H
