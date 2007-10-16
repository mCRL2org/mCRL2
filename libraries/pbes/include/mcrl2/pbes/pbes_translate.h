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

#include <algorithm>
#include "mcrl2/basic/mucalculus.h"
#include "mcrl2/basic/normalize.h"
#include "mcrl2/basic/state_formula_rename.h"
#include "mcrl2/basic/free_variables.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace lps {
  
/// \brief Translates a state_formula and a specification to a pbes
/// \param formula the state formula
/// \param spec the specification
/// \param determines whether the timed or untimed variant of the algorithm is chosen
/// \return the resulting pbes

pbes<> pbes_translate(const state_formula& formula, const specification& spec, bool timed = false)
{
  using namespace detail;
  using namespace state_frm;

  state_formula f = formula;
  std::set<identifier_string> formula_variable_names = find_variable_names(formula);
  std::set<identifier_string> spec_variable_names = find_variable_names(spec);
  std::set<identifier_string> spec_names = identifiers(spec);

  // rename data variables in f, to prevent name clashes with data variables in spec 
  set_identifier_generator generator;
  generator.add_identifiers(spec_variable_names);
  f = rename_data_variables(f, generator);

  // rename predicate variables in f, to prevent name clashes
  xyz_identifier_generator xyz_generator;
  xyz_generator.add_identifiers(spec_names);  
  xyz_generator.add_identifiers(formula_variable_names);  
  f = rename_predicate_variables(f, xyz_generator);

  // remove occurrences of ! and =>
  // f = normalize(f);

  // wrap the formula inside a 'nu' if needed
  if (!is_mu(f) && !is_nu(f))
  {
    aterm_list context = make_list(f, spec);
    identifier_string X = fresh_identifier(context, std::string("X"));
    f = nu(X, data_assignment_list(), f);
  }  
  linear_process lps = spec.process();
  atermpp::vector<pbes_equation> e;

  if (formula.has_time() || spec.process().has_time())
  {
    timed = true;
  }

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
  atermpp::set<data_variable> free_variables(spec.process().free_variables().begin(), spec.process().free_variables().end());

  pbes<> result;
  if (!timed)
  {   
    propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, data_variable_list(), f));
    result = pbes<>(spec.data(), e, free_variables, init);
  }
  else
  {
    propositional_variable_instantiation init(Xe, data_expr::real(0) + fi + pi + Par(Xf, data_variable_list(), f));  
    data_specification data_spec = spec.data();   

    // add sort real to data_spec (if needed)
    if (std::find(spec.data().sorts().begin(), spec.data().sorts().end(), sort_expr::real()) == spec.data().sorts().end())
    {
      data_spec = set_sorts(data_spec, push_front(data_spec.sorts(), sort_expr::real()));
    }

    result = pbes<>(data_spec, e, free_variables, init);
  }
  result.normalize();
  assert(result.is_normalized());
  return result;
}

} // namespace lps

#endif // MCRL2_PBES_PBES_TRANSLATE_H
