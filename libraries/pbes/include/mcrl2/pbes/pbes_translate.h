// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_translate.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_PBES_TRANSLATE_H
#define MCRL2_PBES_PBES_TRANSLATE_H

#include "mcrl2/basic/mucalculus.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"

namespace lps {
  
/// \brief Translates a state_formula and a specification to a pbes
/// \param formula the state formula
/// \param spec the specification
/// \param determines whether the timed or untimed variant of the algorithm is chosen
/// \return the resulting pbes
inline
pbes pbes_translate(const state_formula& formula, const specification& spec, bool timed = false)
{
  using namespace detail;
  using namespace state_frm;

  // rename variables in f, to prevent name clashes with variables in spec
  state_formula f = remove_name_clashes(spec, formula);

  // remove occurrences of ! and =>
  f = normalize(f);

  // wrap the formula inside a 'nu' if needed
  if (!is_mu(f) && !is_nu(f))
  {
    aterm_list context = make_list(f, spec);
    identifier_string X = fresh_identifier(context, std::string("X"));
    f = nu(X, data_assignment_list(), f);
  }  
  linear_process lps = spec.process();
  equation_system e;

  if (!timed)
  {
    using namespace pbes_untimed;
    e = E(f, f, lps);   
  }
  else
  {
    using namespace pbes_timed;
    data_variable T = fresh_variable(make_list(f, lps), sort_expr::real(), "T");
    aterm_list context = make_list(T, spec.initial_process(), lps, f);
    lps = detail::make_timed_lps(lps, context);
    e = E(f, f, lps, T);
  }

  // create initial state
  assert(e.size() > 0);
  pbes_equation e1 = e.front();
  identifier_string Xe(e1.variable().name());
  assert(is_mu(f) || is_nu(f));
  identifier_string Xf = mu_name(f);
  data_expression_list fi = mu_expressions(f);
  data_expression_list pi = spec.initial_process().state();

  if (!timed)
  {   
    propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, f));
    return pbes(spec.data(), e, init);
  }
  else
  {
    propositional_variable_instantiation init(Xe, data_expr::real(0) + fi + pi + Par(Xf, f));
    return pbes(spec.data(), e, init);
  }
}

} // namespace lps

#endif // MCRL2_PBES_PBES_TRANSLATE_H
