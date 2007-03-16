///////////////////////////////////////////////////////////////////////////////
/// \file pbes.h
/// Contains pbes data structures for the LPS Library.

#ifndef LPS_PBES_TRANSLATE_H
#define LPS_PBES_TRANSLATE_H

#include <string>
#include <iostream>
#include <stdexcept>

#include "atermpp/aterm_access.h"
#include "atermpp/vector.h"
#include "atermpp/algorithm.h"
#include "atermpp/substitute.h"
#include "lps/data_functional.h"
#include "lps/data_utility.h"
#include "lps/data_operators.h"
#include "lps/mucalculus.h"
#include "lps/pbes.h"
#include "lps/specification.h"
#include "lps/data_utility.h"
#include "lps/lps_algorithm.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::make_substitution;

inline
bool equal_name(action a, action b)
{
  return a.name() == b.name();
}

// TODO: sort a and b first!
inline
bool equal_names(action_list a, action_list b)
{
  assert(a.size() == b.size());
  action_list::iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    if (!equal_name(*i, *j))
      return false;
  }
  return true;
}

inline
pbes_expression equal_data_parameters(action_list a, action_list b)
{
  namespace p = lps::pbes_expr;
  atermpp::vector<pbes_expression> v;

  assert(a.size() == b.size());
  action_list::iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    data_expression_list d1 = i->arguments();
    data_expression_list d2 = j->arguments();
    assert(d1.size() == d2.size());
    data_expression_list::iterator i1, i2;
    for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
    {
      v.push_back(p::val(equal_to(*i1, *i2)));
    }
  }
  return p::multi_and(v.begin(), v.end());
}

inline
pbes_expression not_equal_data_parameters(action_list a, action_list b)
{
  namespace p = lps::pbes_expr;
  atermpp::vector<pbes_expression> v;

  assert(a.size() == b.size());
  action_list::iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    data_expression_list d1 = i->arguments();
    data_expression_list d2 = j->arguments();
    assert(d1.size() == d2.size());
    data_expression_list::iterator i1, i2;
    for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
    {
      v.push_back(p::val(equal_to(*i1, *i2)));
    }
  }
  return p::multi_or(v.begin(), v.end());
}

inline
data_variable_list Par(identifier_string x, state_formula f)
{
  using namespace lps::state_frm;

  if (is_data(f)) {
    return data_variable_list();
  } else if (is_true(f)) {
    return data_variable_list();
  } else if (is_false(f)) {
    return data_variable_list();
  } else if (is_and(f)) {
    return Par(x, lhs(f)) + Par(x, rhs(f));
  } else if (is_or(f)) {
    return Par(x, lhs(f)) + Par(x, rhs(f));
  } else if (is_must(f)) {
    return Par(x, mod_form(f));
  } else if (is_may(f)) {
    return Par(x, mod_form(f));
  } else if (is_forall(f)) {
    return quant_vars(f) + Par(x, quant_form(f));
  } else if (is_exists(f)) {
    return quant_vars(f) + Par(x, quant_form(f));
  } else if (is_var(f)) {
    return data_variable_list();
  } else if (is_mu(f) || (is_nu(f))) {
    if (mu_name(f) == x)
    {
      return data_variable_list();
    }
    else
    {
      data_variable_list xf = mu_variables(f);
      state_formula g = arg3(f);
      return xf + Par(x, g);
    }
  } else if (is_yaled_timed(f)) {
    return data_variable_list();
  } else if (is_delay_timed(f)) {
    return data_variable_list();
  }
  assert(false);
  return data_variable_list();
}

namespace pbes_timed
{
  // prototype
  inline
  pbes_expression sat_top(timed_action a, action_formula b);
  
  inline
  pbes_expression sat_bot(timed_action a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace p = lps::pbes_expr;
  
    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      if (equal_names(a.actions(), b_actions)) {
        return not_equal_data_parameters(a.actions(), b_actions);
      }
      else
        return p::true_();
    } else if (is_true(b)) {
      return p::false_();
    } else if (is_at(b)) {
      data_expression t = a.time();
      action_formula alpha = at_form(b);
      data_expression t1 = at_time(b);
      return p::or_(sat_bot(a, alpha), p::val(not_equal_to(t, t1)));
    } else if (is_not(b)) {
      return sat_top(a, not_arg(b));
    } else if (is_and(b)) {
      return p::or_(sat_bot(a, lhs(b)), sat_bot(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::exists(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::forall(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
    }
    throw std::runtime_error(std::string("sat_bot[timed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }
  
  inline
  pbes_expression sat_top(timed_action a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace p = lps::pbes_expr;
  
    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      if (equal_names(a.actions(), b_actions)) {
        return equal_data_parameters(a.actions(), b_actions);
      }
      else
        return p::false_();
    } else if (is_true(b)) {
      return p::true_();
    } else if (is_at(b)) {
      data_expression t = a.time();
      action_formula alpha = at_form(b);
      data_expression t1 = at_time(b);
      return p::and_(sat_top(a, alpha), p::val(equal_to(t, t1)));
    } else if (is_not(b)) {
      return sat_bot(a, not_arg(b));
    } else if (is_and(b)) {
      return p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::forall(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::exists(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
    }
    throw std::runtime_error(std::string("sat_top[timed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  inline
  pbes_expression RHS(state_formula f, linear_process lps, data_variable T, std::set<std::string>& context)
  {
    using namespace lps::pbes_expr;
    using lps::summand_list;
    namespace s = lps::state_frm;
  
    if (s::is_data(f)) {
      return pbes_expression(f);
    } else if (s::is_true(f)) {
      return true_();
    } else if (s::is_false(f)) {
      return false_();
    } else if (s::is_and(f)) {
		return and_(RHS(s::lhs(f), lps, T, context), RHS(s::rhs(f), lps, T, context));
    } else if (s::is_or(f)) {
      return or_(RHS(s::lhs(f), lps, T, context), RHS(s::rhs(f), lps, T, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return forall(s::quant_vars(f), RHS(s::quant_form(f), lps, T, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return exists(s::quant_vars(f), RHS(s::quant_form(f), lps, T, context));
    } else if (s::is_must(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha = s::mod_act(f);
      state_formula f1 = s::mod_form(f);
      for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression ci(i->condition());
        data_expression ti(i->time());
        timed_action ai(i->actions(), ti);
        data_assignment_list gi = i->assignments();
        data_variable_list xp(lps.process_parameters());
        data_variable_list yi(i->summation_variables());

        pbes_expression rhs = RHS(f1, lps, T, context);
        std::set<std::string> rhs_context = find_variable_names(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ti = ti.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));

        pbes_expression p1 = sat_bot(ai, alpha);
        pbes_expression p2 = val(data_expr::not_(ci));
        pbes_expression p3 = val(less_equal(ti, T));
        rhs = rhs.substitute(make_substitution(T, ti));
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = or_(or_(or_(p1, p2), p3), rhs);
        if (y.size() > 0) {
          p = forall(y, p);
        }
        v.push_back(p);
      }
      return multi_and(v.begin(), v.end());
    } else if (s::is_may(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha = s::mod_act(f);
      state_formula f1 = s::mod_form(f);
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression ci(i->condition());
        data_expression ti(i->time());
        timed_action ai(i->actions(), ti);
        data_assignment_list gi = i->assignments();
        data_variable_list xp(lps.process_parameters());
        data_variable_list yi(i->summation_variables());

        pbes_expression rhs = RHS(f1, lps, T, context);
        std::set<std::string> rhs_context = find_variable_names(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ti = ti.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));

        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        pbes_expression p3 = val(greater(ti, T));
        rhs = rhs.substitute(make_substitution(T, ti));
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = and_(and_(and_(p1, p2), p3), rhs);
        if (y.size() > 0) {
          p = exists(y, p);
        }
        v.push_back(p);
      }
      return multi_or(v.begin(), v.end());
    } else if (s::is_delay_timed(f)) {
      data_expression t = s::time(f);
      atermpp::vector<pbes_expression> v;
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        data_expression ck(i->condition());
        data_expression tk(i->time());
        data_variable_list yk = i->summation_variables();
        pbes_expression p = exists(yk, and_(val(ck), val(less_equal(t, tk))));
        v.push_back(p);
      }
      return or_(multi_or(v.begin(), v.end()), val(less_equal(t, T)));
    } else if (s::is_yaled_timed(f)) {
      data_expression t = s::time(f);
      atermpp::vector<pbes_expression> v;
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        data_expression ck(i->condition());
        data_expression tk(i->time());
        data_variable_list yk = i->summation_variables();
        pbes_expression p = exists(yk, and_(val(data_expr::not_(ck)), val(greater(t, tk))));
        v.push_back(p);
      }
      return and_(multi_or(v.begin(), v.end()), val(greater(t, T)));
    } else if (s::is_var(f)) {
      identifier_string X = s::var_name(f);
      data_expression_list d = s::var_val(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, T + d + xp + Par(X, f));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = s::mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, T + d + xp + Par(X, f));
    }
    throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  inline
  equation_system E(state_formula f, linear_process lps, data_variable T)
  {
    using namespace lps::state_frm;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(lhs(f), lps, T) + E(rhs(f), lps, T);
    } else if (is_or(f)) {
      return E(lhs(f), lps, T) + E(rhs(f), lps, T);
    } else if (is_forall(f)) {
      return E(quant_form(f), lps, T);
    } else if (is_exists(f)) {
      return E(quant_form(f), lps, T);
    } else if (is_must(f)) {
      return E(mod_form(f), lps, T);
    } else if (is_may(f)) {
      return E(mod_form(f), lps, T);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, T + xf + xp + Par(X, f));
      std::set<std::string> context;
      pbes_expression expr = RHS(g, lps, T, context);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(g, lps, T);
    } else if (is_yaled_timed(f)) {
      return equation_system();
    } else if (is_delay_timed(f)) {
      return equation_system();
    }
    throw std::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
    return equation_system();
  }
} // namespace pbes_timed

namespace pbes_untimed
{
  // prototype
  inline
  pbes_expression sat_top(action_list a, action_formula b);
  
  inline
  pbes_expression sat_bot(action_list a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace p = lps::pbes_expr;
  
    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      if (equal_names(a, b_actions)) {
        return not_equal_data_parameters(a, b_actions);
      }
      else
        return p::true_();
    } else if (is_true(b)) {
      return p::false_();
    } else if (is_not(b)) {
      return sat_top(a, not_arg(b));
    } else if (is_and(b)) {
      return p::or_(sat_bot(a, lhs(b)), sat_bot(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::exists(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
      }
      else
        return sat_bot(a, alpha);
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::forall(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
      }
      else
        return sat_bot(a, alpha);
    }
    throw std::runtime_error(std::string("sat_bot[untimed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }
  
  inline
  pbes_expression sat_top(action_list a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace p = lps::pbes_expr;
  
    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      if (equal_names(a, b_actions)) {
        return equal_data_parameters(a, b_actions);
      }
      else
        return p::false_();
    } else if (is_true(b)) {
      return p::true_();
    } else if (is_not(b)) {
      return sat_bot(a, not_arg(b));
    } else if (is_and(b)) {
      return p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::forall(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
      }
      else
        return sat_top(a, alpha);
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::exists(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
      }
      else
        return sat_top(a, alpha);
    }
    throw std::runtime_error(std::string("sat_top[untimed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  inline
  pbes_expression RHS(state_formula f, linear_process lps, std::set<std::string>& context)
  {
    using namespace lps::pbes_expr;
    using lps::summand_list;
    namespace s = lps::state_frm;
  
    if (s::is_data(f)) {
      return pbes_expression(f);
    } else if (s::is_true(f)) {
      return true_();
    } else if (s::is_false(f)) {
      return false_();
    } else if (s::is_and(f)) {
      return and_(RHS(s::lhs(f), lps, context), RHS(s::rhs(f), lps, context));
    } else if (s::is_or(f)) {
      return or_(RHS(s::lhs(f), lps, context), RHS(s::rhs(f), lps, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return forall(s::quant_vars(f), RHS(s::quant_form(f), lps, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return exists(s::quant_vars(f), RHS(s::quant_form(f), lps, context));
    } else if (s::is_must(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(s::mod_act(f));
      state_formula f1(s::mod_form(f));
      for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression ci(i->condition());
        action_list ai(i->actions());
        data_assignment_list gi = i->assignments();
        data_variable_list xp(lps.process_parameters());
        data_variable_list yi(i->summation_variables());

        pbes_expression rhs = RHS(f1, lps, context);
        std::set<std::string> rhs_context = find_variable_names(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));
        pbes_expression p1 = sat_bot(ai, alpha);
        pbes_expression p2 = val(data_expr::not_(ci));
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = or_(or_(p1, p2), rhs);
        if (y.size() > 0) {
          p = forall(y, p);
        }
        v.push_back(p);
      }
      return multi_and(v.begin(), v.end());
    } else if (s::is_may(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(s::mod_act(f));
      state_formula f1(s::mod_form(f));
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression ci(i->condition());
        action_list ai(i->actions());
        data_assignment_list gi = i->assignments();
        data_variable_list xp(lps.process_parameters());
        data_variable_list yi(i->summation_variables());

        pbes_expression rhs = RHS(f1, lps, context);
        std::set<std::string> rhs_context = find_variable_names(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));
        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = and_(and_(p1, p2), rhs);
        if (y.size() > 0) {
          p = exists(y, p);
        }
        v.push_back(p);
      }
      return multi_or(v.begin(), v.end());
    } else if (s::is_var(f)) {
      identifier_string X = s::var_name(f);
      data_expression_list d = s::var_val(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, d + xp + Par(X, f));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = s::mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, d + xp + Par(X, f));
    }
    throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  inline
  equation_system E(state_formula f, linear_process lps)
  {
    using namespace lps::state_frm;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(lhs(f), lps) + E(rhs(f), lps);
    } else if (is_or(f)) {
      return E(lhs(f), lps) + E(rhs(f), lps);
    } else if (is_forall(f)) {
      return E(quant_form(f), lps);
    } else if (is_exists(f)) {
      return E(quant_form(f), lps);
    } else if (is_must(f)) {
      return E(mod_form(f), lps);
    } else if (is_may(f)) {
      return E(mod_form(f), lps);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, xf + xp + Par(X, f));
      std::set<std::string> context;
      pbes_expression expr = RHS(g, lps, context);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(g, lps);
    } else if (is_yaled_timed(f)) {
      return equation_system();
    } else if (is_delay_timed(f)) {
      return equation_system();
    }
    throw std::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
    return equation_system();
  }
} // namespace pbes_untimed

/// Returns a formula that is equivalent to f and uses no variables occurring in spec.
inline
state_formula remove_name_clashes(specification spec, state_formula f)
{
  // find all data variables in spec
  std::set<data_variable> variables = lps::find_variables(spec);
   
  // find all data variables in f
  std::set<data_variable> formula_variables = lps::find_variables(f);

  // compute the intersection and put it in x
  std::vector<data_variable> x;
  std::set_intersection(variables.begin(),
                        variables.end(),
                        formula_variables.begin(),
                        formula_variables.end(),
                        std::back_inserter(x)
                       );

  // generate a vector y with replacements
  fresh_variable_generator generator;
  for (std::set<data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
  {
    generator.add_to_context(*i);
  }
  for (std::set<data_variable>::iterator i = formula_variables.begin(); i != formula_variables.end(); ++i)
  {
    generator.add_to_context(*i);
  }
  std::vector<data_variable> y;
  for (std::vector<data_variable>::iterator i = x.begin(); i != x.end(); ++i)
  {
    y.push_back(generator(*i));
  }
  
  return f.substitute(make_list_substitution(x, y));
}

// translate a state_formula and an LPS to a pbes
inline
pbes pbes_translate(state_formula f, specification spec, bool timed = false)
{
  using namespace state_frm;

  // rename variables in f, to prevent name clashes with variables in spec
  f = remove_name_clashes(spec, f);

  // wrap the formula inside a 'nu' if needed
  if (!is_mu(f) && !is_nu(f))
  {
    aterm_list context = make_list(f, spec);
    identifier_string X = fresh_identifier("X", context);
    f = nu(X, data_assignment_list(), f);
  }  
  linear_process lps = spec.process();
  equation_system e;

  if (!timed)
  {
    using namespace pbes_untimed;
    e = E(f, lps);   
  }
  else
  {
    using namespace pbes_timed;
    data_variable T = fresh_variable("T", make_list(f, lps));
    aterm_list context = make_list(T, spec.initial_process(), lps, f);
    lps = make_timed_lps(lps, context);
    e = E(f, lps, T);
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

#endif // LPS_PBES_TRANSLATE_H
