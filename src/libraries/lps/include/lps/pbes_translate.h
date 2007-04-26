///////////////////////////////////////////////////////////////////////////////
/// \file pbes.h
/// Contains pbes data structures for the LPS Library.

#ifndef LPS_PBES_TRANSLATE_H
#define LPS_PBES_TRANSLATE_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <map>

#include "atermpp/aterm_access.h"
#include "atermpp/vector.h"
#include "atermpp/algorithm.h"
#include "atermpp/substitute.h"
#include "lps/data_functional.h"
#include "lps/data_utility.h"
#include "lps/data_operators.h"
#include "lps/data_expression.h"
#include "lps/mucalculus.h"
#include "lps/pbes.h"
#include "lps/pbes_utility.h"
#include "lps/specification.h"
#include "lps/lps_algorithm.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::make_substitution;

/// Precondition: The range [first, last[ contains sorted arrays.
/// Visits all permutations of the arrays, and calls f for
/// each instance.
template <typename Iter, typename Function>
void forall_permutations(Iter first, Iter last, Function f)
{
  if (first == last)
  {
    f();
    return;
  }
  Iter next = first;
  ++next;
  forall_permutations(next, last, f);
  while (std::next_permutation(first->first, first->second))
  {
    forall_permutations(next, last, f);
  }
}

// TODO: check if this assumption is correct: Precondition: a and b are sorted
inline
bool equal_names(const std::vector<action>& a, const std::vector<action>& b)
{
  if(a.size() != b.size())
  {
    return false;
  }
  std::vector<action>::const_iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    if (i->name() != j->name())
      return false;
  }
  return true;
}

struct compare_action_name
{
  bool operator()(const action& a, const action& b) const
  {
    return a.name() < b.name();
  }
};

/// Used for building an expression for the comparison of data parameters.
struct equal_data_parameters_builder
{
  const std::vector<action>& a;
  const std::vector<action>& b;
  atermpp::vector<pbes_expression>& result;
  
  equal_data_parameters_builder(const std::vector<action>& a_,
                                const std::vector<action>& b_,
                                atermpp::vector<pbes_expression>& result_
                               )
    : a(a_),
      b(b_),
      result(result_)
  {}

  /// Adds the expression 'a == b' to result.
  void operator()()
  {
    namespace d = lps::data_expr;
    namespace p = lps::pbes_expr;

    atermpp::vector<pbes_expression> v;
    std::vector<action>::const_iterator i, j;
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
    result.push_back(p::multi_and(v.begin(), v.end()));
  }
};

inline
pbes_expression equal_data_parameters(action_list a, action_list b)
{
  namespace p = lps::pbes_expr;

  // make copies of a and b and sort them
  std::vector<action> va(a.begin(), a.end()); // protection not needed
  std::vector<action> vb(b.begin(), b.end()); // protection not needed
  std::sort(va.begin(), va.end(), compare_action_name());
  std::sort(vb.begin(), vb.end(), compare_action_name());

  if (!equal_names(va, vb))
    return p::false_();

  // compute the intervals of a with equal names
  typedef std::vector<action>::iterator action_iterator;
  std::vector<std::pair<action_iterator, action_iterator> > intervals;
  action_iterator first = va.begin();
  while (first != va.end())
  {
    action_iterator next = std::upper_bound(first, va.end(), *first, compare_action_name());
    intervals.push_back(std::make_pair(first, next));
    first = next;
  }
  atermpp::vector<pbes_expression> z;
  equal_data_parameters_builder f(va, vb, z);
  forall_permutations(intervals.begin(), intervals.end(), f); 
  pbes_expression result = p::multi_or(z.begin(), z.end());
  return result;
}

/// Used for building an expression for the comparison of data parameters.
struct not_equal_data_parameters_builder
{
  const std::vector<action>& a;
  const std::vector<action>& b;
  atermpp::vector<pbes_expression>& result;
  
  not_equal_data_parameters_builder(const std::vector<action>& a_,
                                    const std::vector<action>& b_,
                                    atermpp::vector<pbes_expression>& result_
                                   )
    : a(a_),
      b(b_),
      result(result_)
  {}
  
  /// Adds the expression 'a == b' to result.
  void operator()()
  {
    namespace d = lps::data_expr;
    namespace p = lps::pbes_expr;

    atermpp::vector<pbes_expression> v;
    std::vector<action>::const_iterator i, j;
    for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
    {
      data_expression_list d1 = i->arguments();
      data_expression_list d2 = j->arguments();
      assert(d1.size() == d2.size());
      data_expression_list::iterator i1, i2;
      for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
      {
        v.push_back(p::val(not_equal_to(*i1, *i2)));
      }
    }
    result.push_back(p::multi_or(v.begin(), v.end()));
  }
};

inline
pbes_expression not_equal_data_parameters(action_list a, action_list b)
{
  namespace p = lps::pbes_expr;

  // make copies of a and b and sort them
  std::vector<action> va(a.begin(), a.end());
  std::vector<action> vb(b.begin(), b.end());
  std::sort(va.begin(), va.end(), compare_action_name());
  std::sort(vb.begin(), vb.end(), compare_action_name());

  if (!equal_names(va, vb))
    return p::true_();

  // compute the intervals of a with equal names
  typedef std::vector<action>::iterator action_iterator;
  std::vector<std::pair<action_iterator, action_iterator> > intervals;
  action_iterator first = va.begin();
  while (first != va.end())
  {
    action_iterator next = std::upper_bound(first, va.end(), *first, compare_action_name());
    intervals.push_back(std::make_pair(first, next));
    first = next;
  }
  atermpp::vector<pbes_expression> z;
  not_equal_data_parameters_builder f(va, vb, z);
  forall_permutations(intervals.begin(), intervals.end(), f); 
  pbes_expression result = p::multi_and(z.begin(), z.end());
  return result;
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

/// The function normalize brings a state formula into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
inline
state_formula normalize(state_formula f)
{
  using namespace lps::state_frm;

  if (is_imp(f)) {
    return or_(normalize(not_(lhs(f))), normalize(rhs(f)));
  } 
  if (!is_not(f)) {
    return f;
  }
  f = not_arg(f);
  if (is_data(f)) {
    return not_(f);
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
    return nu(mu_name(f), mu_params(f), mu_form(not_(f.substitute(state_variable_negation(mu_name(f))))));
  } else if (is_nu(f)) {
    return mu(mu_name(f), mu_params(f), mu_form(not_(f.substitute(state_variable_negation(mu_name(f))))));
  }
  assert(false);
  return state_formula();
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
      return not_equal_data_parameters(a.actions(), b_actions);
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
      return p::exists(y, sat_bot(a, alpha.substitute(make_list_substitution(x, y))));
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::forall(y, sat_bot(a, alpha.substitute(make_list_substitution(x, y))));
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
      return equal_data_parameters(a.actions(), b_actions);
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
      return p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_names(make_list(a.actions(), a.time(), b)));
      return p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
    }
    throw std::runtime_error(std::string("sat_top[timed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  /// f0 is the original formula
  inline
  pbes_expression RHS(state_formula f0, state_formula f, linear_process lps, data_variable T, std::set<std::string>& context)
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
		return and_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
    } else if (s::is_or(f)) {
      return or_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
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

        pbes_expression rhs = RHS(f0, f1, lps, T, context);
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

        pbes_expression rhs = RHS(f0, f1, lps, T, context);
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
      return propositional_variable_instantiation(X, T + d + xp + Par(X, f0));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = s::mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, T + d + xp + Par(X, f0));
    }
    throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  /// f0 is the original formula
  inline
  equation_system E(state_formula f0, state_formula f, linear_process lps, data_variable T)
  {
    using namespace lps::state_frm;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
    } else if (is_or(f)) {
      return E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
    } else if (is_forall(f)) {
      return E(f0, quant_form(f), lps, T);
    } else if (is_exists(f)) {
      return E(f0, quant_form(f), lps, T);
    } else if (is_must(f)) {
      return E(f0, mod_form(f), lps, T);
    } else if (is_may(f)) {
      return E(f0, mod_form(f), lps, T);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, T + xf + xp + Par(X, f0));
      std::set<std::string> context;
      pbes_expression expr = RHS(f0, g, lps, T, context);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(f0, g, lps, T);
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
      return not_equal_data_parameters(a, b_actions);
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
        return p::exists(y, sat_bot(a, alpha.substitute(make_list_substitution(x, y))));
      }
      else
        return sat_bot(a, alpha);
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::forall(y, sat_bot(a, alpha.substitute(make_list_substitution(x, y))));
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
      return equal_data_parameters(a, b_actions);
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
        return p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      }
      else
        return sat_top(a, alpha);
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_names(make_list(a, b)));
        return p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      }
      else
        return sat_top(a, alpha);
    }
    throw std::runtime_error(std::string("sat_top[untimed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  /// f0 is the original formula
  inline
  pbes_expression RHS(state_formula f0, state_formula f, linear_process lps, std::set<std::string>& context)
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
      return and_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
    } else if (s::is_or(f)) {
      return or_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_names(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      return exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
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

        pbes_expression rhs = RHS(f0, f1, lps, context);
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

        pbes_expression rhs = RHS(f0, f1, lps, context);
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
      return propositional_variable_instantiation(X, d + xp + Par(X, f0));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = s::mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      return propositional_variable_instantiation(X, d + xp + Par(X, f0));
    }
    throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  /// f0 is the original formula
  inline
  equation_system E(state_formula f0, state_formula f, linear_process lps)
  {
    using namespace lps::state_frm;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
    } else if (is_or(f)) {
      return E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
    } else if (is_forall(f)) {
      return E(f0, quant_form(f), lps);
    } else if (is_exists(f)) {
      return E(f0, quant_form(f), lps);
    } else if (is_must(f)) {
      return E(f0, mod_form(f), lps);
    } else if (is_may(f)) {
      return E(f0, mod_form(f), lps);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, xf + xp + Par(X, f0));
      std::set<std::string> context;
      pbes_expression expr = RHS(f0, g, lps, context);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(f0, g, lps);
    } else if (is_yaled_timed(f)) {
      return equation_system();
    } else if (is_delay_timed(f)) {
      return equation_system();
    }
    throw std::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
    return equation_system();
  }
} // namespace pbes_untimed

/// Removes name clashes in nested fix point variables like in the formula mu X. mu X. ...
/// @param generator Generates names for fresh variables.
/// @param replacements replacements for previous occurrences of propositional variables.
inline
state_formula remove_name_clashes_impl(state_formula f, fresh_identifier_generator& generator, std::map<identifier_string, identifier_string>& replacements)
{
  using namespace lps::state_frm;

  if (is_data(f)) {
    return f;
  } else if (is_true(f)) {
    return f;
  } else if (is_false(f)) {
    return f;
  } else if (is_not(f)) {
    return not_(remove_name_clashes_impl(not_arg(f), generator, replacements));
  } else if (is_and(f)) {
    return and_(remove_name_clashes_impl(lhs(f), generator, replacements), remove_name_clashes_impl(rhs(f), generator, replacements));
  } else if (is_or(f)) {
    return or_(remove_name_clashes_impl(lhs(f), generator, replacements), remove_name_clashes_impl(rhs(f), generator, replacements));
  } else if (is_imp(f)) {
    return imp(remove_name_clashes_impl(lhs(f), generator, replacements), remove_name_clashes_impl(rhs(f), generator, replacements));
  } else if (is_forall(f)) {
    return forall(quant_vars(f), remove_name_clashes_impl(quant_form(f), generator, replacements));
  } else if (is_exists(f)) {
    return exists(quant_vars(f), remove_name_clashes_impl(quant_form(f), generator, replacements));
  } else if (is_must(f)) {
    return must(mod_act(f), remove_name_clashes_impl(mod_form(f), generator, replacements));
  } else if (is_may(f)) {
    return may(mod_act(f), remove_name_clashes_impl(mod_form(f), generator, replacements));
  } else if (is_yaled(f)) {
    return f;
  } else if (is_delay(f)) {
    return f;
  } else if (is_yaled_timed(f)) {
    return f;
  } else if (is_delay_timed(f)) {
    return f;
  } else if (is_var(f)) {
    assert(replacements.find(var_name(f)) != replacements.end());
    return var(replacements[var_name(f)], var_val(f));
  } else if (is_mu(f) || is_nu(f)) {
    identifier_string X = mu_name(f);
    identifier_string X1 = (replacements.find(X) == replacements.end() ? X : generator(X));
    replacements[X] = X1;
    state_formula f1 = remove_name_clashes_impl(mu_form(f), generator, replacements);
    data_assignment_list p1 = mu_params(f);
    return is_mu(f) ? mu(X1, p1, f1) : nu(X1, p1, f1);
  }
  assert(false);
  return f;  
}

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

  state_formula formula = f.substitute(make_list_substitution(x, y));
  std::map<identifier_string, identifier_string> replacements;
  fresh_identifier_generator generator1(make_list(formula, spec));
  return remove_name_clashes_impl(formula, generator1, replacements);
}

// Translates a state_formula and an LPS to a pbes.
inline
pbes pbes_translate(state_formula f, specification spec, bool timed = false)
{
  using namespace state_frm;

  // rename variables in f, to prevent name clashes with variables in spec
  f = remove_name_clashes(spec, f);

  // remove occurrences of ! and =>
  f = normalize(f);

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
    e = E(f, f, lps);   
  }
  else
  {
    using namespace pbes_timed;
    data_variable T = fresh_variable("T", make_list(f, lps));
    aterm_list context = make_list(T, spec.initial_process(), lps, f);
    lps = make_timed_lps(lps, context);
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

#endif // LPS_PBES_TRANSLATE_H
