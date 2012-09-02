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

#include <string>
#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/monotonicity.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/preprocess_state_formula.h"
#include "mcrl2/modal_formula/state_formula_normalize.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/make_timed_lps.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/monotonicity.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/detail/lps2pbes_indenter.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/detail/lps2pbes_rhs.h"
#include "mcrl2/pbes/detail/lps2pbes_e.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Algorithm for translating a state formula and a timed specification to a pbes.
class lps2pbes_algorithm
{
  public:
    /// \brief Runs the translation algorithm
    /// \param formula A modal formula that represents a property about the system modeled by the given specification
    /// \param spec A linear process specification
    /// \param T The time parameter. If T == data::variable() the untimed version of lps2pbes is applied.
    /// \param structured use the 'structured' approach of generating equations
    /// \return A PBES that encodes the property applied to the given specification
    pbes<> run(const state_formulas::state_formula& formula, const lps::specification& spec, bool structured, data::variable T = data::variable())
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

      // remove occurrences of ! and =>
      if (!state_formulas::is_normalized(f))
      {
        f = state_formulas::normalize(f);
      }
      assert(state_formulas::is_normalized(f));

      // compute the equations
      atermpp::vector<pbes_equation> eqn;
      if (structured)
      {
      	data::set_identifier_generator propvar_generator;
        std::set<core::identifier_string> names = state_formulas::find_state_variable_names(f);
        propvar_generator.add_identifiers(names);
        eqn = detail::E_structured(f, f, spec.process(), propvar_generator, T);
      }
      else
      {
        eqn = detail::E(f, f, spec.process(), T);
      }

      // compute the initial state
      assert(eqn.size() > 0);
      pbes_equation e1 = eqn.front();
      core::identifier_string Xe(e1.variable().name());
      assert(state_formulas::is_mu(f) || state_formulas::is_nu(f));
      core::identifier_string Xf = name(f);
      data::data_expression_list fi = detail::mu_expressions(f);
      data::data_expression_list pi = spec.initial_process().state(spec.process().process_parameters());
      data::data_expression_list e = fi + pi + detail::Par(Xf, data::variable_list(), f);
      if (T != data::variable())
      {
        e = data::sort_real::real_(0) + e;
      }
      propositional_variable_instantiation init(Xe, e);

      pbes<> result(spec.data(), eqn, spec.global_variables(), init);
      assert(is_monotonous(result));
      pbes_system::normalize(result);
      assert(pbes_system::is_normalized(result));
      assert(result.is_closed());
      complete_data_specification(result);
      return result;
    }
};

/// \brief Translates a linear process specification and a state formulas to a PBES. If the solution of the PBES
/// is true, the formula holds for the specification.
/// \param formula A modal formula
/// \param spec A linear process specification
/// \param timed determines whether the timed or untimed variant of the algorithm is chosen
/// \param structured use the 'structured' approach of generating equations
/// \return The resulting pbes
inline pbes<> lps2pbes(const lps::specification& spec, const state_formulas::state_formula& formula, bool timed = false, bool structured = false)
{
  if ((formula.has_time() || spec.process().has_time()) && !timed)
  {
    mCRL2log(log::warning) << "Switch to timed translation because formula has " << (formula.has_time()?"":"no ") << "time, and process has " << (spec.process().has_time()?"":"no ") << "time" << std::endl;
    timed = true;
  }

  if (timed)
  {
    lps::specification spec_timed = spec;
    std::set<core::identifier_string> id_generator = lps::find_identifiers(spec);
    std::set<core::identifier_string> fcontext = state_formulas::find_identifiers(formula);
    id_generator.insert(fcontext.begin(), fcontext.end());
    data::variable T = fresh_variable(id_generator, data::sort_real::real_(), "T");
    id_generator.insert(T.name());
    lps::detail::make_timed_lps(spec_timed.process(), id_generator);
    return lps2pbes_algorithm().run(formula, spec_timed, structured, T);
  }
  else
  {
    return lps2pbes_algorithm().run(formula, spec, structured);
  }
}

/// \brief Applies the lps2pbes algorithm
/// \param spec_text A string
/// \param formula_text A string
/// \param timed Determines whether the timed or untimed version of the translation algorithm is used
/// \return The result of the algorithm
inline
pbes<> lps2pbes(const std::string& spec_text, const std::string& formula_text, bool timed, bool structured = false)
{
  pbes<> result;
  lps::specification spec = lps::linearise(spec_text);
  state_formulas::state_formula f = state_formulas::parse_state_formula(formula_text, spec);
  return lps2pbes(spec, f, timed, structured);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LPS2PBES_H
