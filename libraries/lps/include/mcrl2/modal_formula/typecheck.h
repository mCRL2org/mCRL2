// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TYPECHECK_H
#define MCRL2_MODAL_FORMULA_TYPECHECK_H

#include "mcrl2/core/typecheck.h"
// #include "mcrl2/core/detail/pp_deprecated.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/monotonicity.h"

namespace mcrl2
{

namespace state_formulas
{

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] formula A state formula that has not been type checked.
 *  \post      formula is type checked.
 **/
inline
void type_check(state_formula& formula, const lps::specification& lps_spec, bool check_monotonicity = true)
{
  // TODO: replace all this nonsense code by a proper type check implementation
  atermpp::aterm_appl t = core::type_check_state_frm(formula, specification_to_aterm(lps_spec));
  if (t==atermpp::aterm_appl())
  {
    throw mcrl2::runtime_error("could not type check " + pp(formula));
  }
  formula = atermpp::aterm_appl(t);
  if (check_monotonicity && !is_monotonous(formula))
  {
    throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(formula));
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
