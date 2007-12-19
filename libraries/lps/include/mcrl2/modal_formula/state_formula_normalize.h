// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_normalize.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_STATE_FORMULA_NORMALIZE_H
#define MCRL2_BASIC_STATE_FORMULA_NORMALIZE_H

#include "mcrl2/modal_formula/state_formula_builder.h"

namespace lps {

/// \internal
/// Puts a logical not around state variables named X
struct state_variable_negation
{
  identifier_string X;
  
  state_variable_negation(identifier_string X_)
    : X(X_)
  {}
  
  aterm_appl operator()(aterm_appl t) const
  {
    using namespace lps::state_frm;

    if (is_var(t) && (var_name(t) == X))
    {
      return not_(t);
    }
    else
    {
      return t;
    }
  }
};

/// \internal
/// The function normalize brings a state formula into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
inline
state_formula normalize(state_formula f)
{
  using namespace lps::state_frm;

  if (is_not(f))
  {
    f = not_arg(f); // remove the not
    if (is_data(f)) {
      return data_expr::not_(f);
    } else if (is_true(f)) {
      return false_();
    } else if (is_false(f)) {
      return true_();
    } else if (is_not(f)) {
      return normalize(not_arg(f));
    } else if (is_and(f)) {
      return or_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_or(f)) {
      return and_(normalize(not_(lhs(f))), normalize(not_(rhs(f))));
    } else if (is_imp(f)) {
      return and_(normalize(lhs(f)), normalize(not_(rhs(f))));
    } else if (is_forall(f)) {
      return exists(quant_vars(f), normalize(not_(quant_form(f))));
    } else if (is_exists(f)) {
      return forall(quant_vars(f), normalize(not_(quant_form(f))));
    } else if (is_must(f)) {
      return may(mod_act(f), normalize(not_(mod_form(f))));
    } else if (is_may(f)) {
      return must(mod_act(f), normalize(not_(mod_form(f))));
    } else if (is_yaled_timed(f)) {
      return delay_timed(time(f));
    } else if (is_yaled(f)) {
      return delay();
    } else if (is_delay_timed(f)) {
      return yaled_timed(time(f));
    } else if (is_delay(f)) {
      return yaled();
    } else if (is_var(f)) {
      throw std::runtime_error(std::string("normalize error: illegal argument ") + f.to_string());
    } else if (is_mu(f)) {
      return nu(mu_name(f), mu_params(f), mu_form(normalize(not_(f.substitute(state_variable_negation(mu_name(f)))))));
    } else if (is_nu(f)) {
      return mu(mu_name(f), mu_params(f), mu_form(normalize(not_(f.substitute(state_variable_negation(mu_name(f)))))));
    }
  }
  else // !is_not(f)
  {
    if (is_data(f)) {
      return f;
    } else if (is_true(f)) {
      return f;
    } else if (is_false(f)) {
      return f;
    //} else if (is_not(f)) {
    // ;
    } else if (is_and(f)) {
      return and_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_or(f)) {
      return or_(normalize(lhs(f)), normalize(rhs(f)));
    } else if (is_imp(f)) {
      return or_(normalize(lhs(f)), normalize(not_(rhs(f))));
    } else if (is_forall(f)) {
      return forall(quant_vars(f), normalize(quant_form(f)));
    } else if (is_exists(f)) {
      return exists(quant_vars(f), normalize(quant_form(f)));
    } else if (is_must(f)) {
      return must(mod_act(f), normalize(mod_form(f)));
    } else if (is_may(f)) {
      return may(mod_act(f), normalize(mod_form(f)));
    } else if (is_yaled_timed(f)) {
      return f;
    } else if (is_yaled(f)) {
      return f;
    } else if (is_delay_timed(f)) {
      return f;
    } else if (is_delay(f)) {
      return f;
    } else if (is_var(f)) {
      return f;
    } else if (is_mu(f)) {
      return mu(mu_name(f), mu_params(f), normalize(mu_form(f)));
    } else if (is_nu(f)) {
      return nu(mu_name(f), mu_params(f), normalize(mu_form(f)));
    }
  }
  throw std::runtime_error(std::string("normalize error: unknown argument ") + f.to_string());
  return state_formula();
}

} // namespace lps

#endif // MCRL2_BASIC_STATE_FORMULA_NORMALIZE_H
