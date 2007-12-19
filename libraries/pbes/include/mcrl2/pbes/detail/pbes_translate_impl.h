// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_translate_impl.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
#define MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <map>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/lps/detail/sorted_sequence_algorithm.h"

namespace lps {

namespace detail {

using atermpp::aterm_appl;
using atermpp::make_substitution;

inline
std::string pp(std::set<data_variable> s)
{
  return ::pp(data_variable_list(s.begin(), s.end()));
}

inline
std::set<data_variable> compute_free_pbes_expression_variables(const pbes_expression& e)
{
  free_variable_visitor visitor;
  visitor.visit(e);
  return visitor.result;
}

inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const atermpp::vector<pbes_equation>& q)
{
  atermpp::vector<pbes_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}

inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const pbes_equation& e)
{
  atermpp::vector<pbes_equation> result(p);
  result.push_back(e);
  return result;
}

/// \brief Visitor that collects the names of propositional variables + instantiations
struct prop_var_visitor
{
  std::set<identifier_string>& m_identifiers;

  prop_var_visitor(std::set<identifier_string>& identifiers)
    : m_identifiers(identifiers)
  {}

  bool operator()(aterm_appl t)
  {
    bool result = true;
    if (is_propositional_variable(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable(t).name());
    }
    if (is_propositional_variable_instantiation(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable_instantiation(t).name());
    }
    return result;
  }
};

/// \brief Returns the names of all propositional variables that occur in the term t
template <typename Term>
std::set<identifier_string> propositional_variable_names(Term t)
{
  std::set<identifier_string> result;
  atermpp::for_each(t, prop_var_visitor(result));
  return result;
}

/// \brief Returns the variables corresponding to mu_params(f)
inline
data_variable_list mu_variables(state_formula f)
{
  assert(gsIsStateMu(f) || gsIsStateNu(f));
  data_assignment_list l = state_frm::mu_params(f);
  data_variable_list result;
  for(data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->lhs());
  }
  return atermpp::reverse(result);
}

/// \brief Returns the data expressions corresponding to mu_params(f)
inline
data_expression_list mu_expressions(state_formula f)
{
  assert(gsIsStateMu(f) || gsIsStateNu(f));
  data_assignment_list l = state_frm::mu_params(f);
  data_expression_list result;
  for(data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

///////////////////////////////////////////////////////////////////////////////
// timed_action
/// \brief multi action with time
///
class timed_action
{
  protected:
    action_list     m_actions;
    data_expression m_time;

  public:
    timed_action(action_list actions, data_expression time)
      : m_actions(actions), m_time(time)
    {}

    /// Returns true if time is available.
    ///
    bool has_time() const
    {
      return !data_expr::is_nil(m_time);
    }

    /// Returns the sequence of actions. Returns an empty list if is_delta() holds.
    ///
    action_list actions() const
    {
      return m_actions;
    }

    /// Returns the time expression.
    ///
    data_expression time() const
    {
      return m_time;
    }

    /// Returns a term representing the name of the first action.
    ///
    identifier_string name() const
    {
      return front(m_actions).label().name();
    }

    /// Returns the argument of the multi action.
    data_expression_list arguments() const
    {
      return front(m_actions).arguments();
    }

    /// Applies a substitution to this action and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    timed_action substitute(Substitution f)
    {
      return timed_action(m_actions.substitute(f), m_time.substitute(f));
    }
};

inline
std::ostream& operator<<(std::ostream& to, const timed_action& a)
{
  to << "TimedAction(" << a.actions();
  if (a.has_time())
    to << "," << a.time();
  to << ")";
  return to;
}

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

/// Returns true if the actions in a and b have the same names, and the same sorts.
/// \pre a and b are sorted w.r.t. to the names of the actions.
inline
bool equal_action_signatures(const std::vector<action>& a, const std::vector<action>& b)
{
  if(a.size() != b.size())
  {
    return false;
  }
  std::vector<action>::const_iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    if (i->label().name() != j->label().name() || i->label().sorts() != j->label().sorts())
      return false;
  }
  return true;
}

struct compare_action_name
{
  bool operator()(const action& a, const action& b) const
  {
    return a.label().name() < b.label().name();
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
        v.push_back(p::val(d::equal_to(*i1, *i2)));
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

  if (!equal_action_signatures(va, vb))
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
        v.push_back(p::val(d::not_equal_to(*i1, *i2)));
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

  if (!equal_action_signatures(va, vb))
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
data_variable_list Par(identifier_string x, data_variable_list l, state_formula f)
{
  using namespace lps::state_frm;

  if (is_data(f)) {
    return data_variable_list();
  } else if (is_true(f)) {
    return data_variable_list();
  } else if (is_false(f)) {
    return data_variable_list();
  } else if (is_not(f)) {
    return Par(x, l, not_arg(f));
  } else if (is_and(f)) {
    return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
  } else if (is_or(f)) {
    return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
  } else if (is_imp(f)) {
    return Par(x, l, lhs(f)) + Par(x, l, rhs(f));
  } else if (is_must(f)) {
    return Par(x, l, mod_form(f));
  } else if (is_may(f)) {
    return Par(x, l, mod_form(f));
  } else if (is_forall(f)) {
    return Par(x, l + quant_vars(f), quant_form(f));
  } else if (is_exists(f)) {
    return Par(x, l + quant_vars(f), quant_form(f));
  } else if (is_var(f)) {
    return data_variable_list();
  } else if (is_mu(f) || (is_nu(f))) {
    if (mu_name(f) == x)
    {
      return l;
    }
    else
    {
      data_variable_list xf = mu_variables(f);
      state_formula g = arg3(f);
      return xf + Par(x, l + xf, g);
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
  inline
  pbes_expression sat_top(timed_action a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace d = lps::data_expr;
    namespace p = lps::pbes_expr;

    pbes_expression result;

    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      result = equal_data_parameters(a.actions(), b_actions);
    } else if (is_true(b)) {
      result = p::true_();
    } else if (is_at(b)) {
      data_expression t = a.time();
      action_formula alpha = at_form(b);
      data_expression t1 = at_time(b);
      result = p::and_(sat_top(a, alpha), p::val(d::equal_to(t, t1)));
    } else if (is_not(b)) {
      result = p::not_(sat_top(a, not_arg(b)));
    } else if (is_and(b)) {
      result = p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_or(b)) {
      result = p::or_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_imp(b)) {
      result = p::imp(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_name_strings(make_list(a.actions(), a.time(), b)));
      result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      assert(x.size() > 0);
      action_formula alpha = quant_form(b);
      data_variable_list y = fresh_variables(x, find_variable_name_strings(make_list(a.actions(), a.time(), b)));
      result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
    } else {
      throw std::runtime_error(std::string("sat_top[timed] error: unknown action formula ") + b.to_string());
    }
    return result;
  }

  /// f0 is the original formula
  inline
  pbes_expression RHS(state_formula f0, state_formula f, linear_process lps, data_variable T, std::set<std::string>& context)
  {
    using namespace lps::pbes_expr;
    using lps::summand_list;
    namespace s = lps::state_frm;
    namespace d = lps::data_expr;

    pbes_expression result;

    if (s::is_data(f)) {
      result = pbes_expression(f);
    } else if (s::is_true(f)) {
      result = true_();
    } else if (s::is_false(f)) {
      result = false_();
    } else if (s::is_not(f)) {
		  result = not_(RHS(f0, s::not_arg(f), lps, T, context));
    } else if (s::is_and(f)) {
		  result = and_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
    } else if (s::is_or(f)) {
      result = or_(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
    } else if (s::is_imp(f)) {
		  result = imp(RHS(f0, s::lhs(f), lps, T, context), RHS(f0, s::rhs(f), lps, T, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_name_strings(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      result = forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_name_strings(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      result = exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, T, context));
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
        std::set<std::string> rhs_context = find_variable_name_strings(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ti = ti.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));

        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        pbes_expression p3 = val(d::greater(ti, T));
        rhs = rhs.substitute(make_substitution(T, ti));
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = forall(y, imp(and_(and_(p1, p2), p3), rhs));
        v.push_back(p);
      }
      result = multi_and(v.begin(), v.end());
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
        std::set<std::string> rhs_context = find_variable_name_strings(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ti = ti.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));

        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        pbes_expression p3 = val(d::greater(ti, T));
        rhs = rhs.substitute(make_substitution(T, ti));
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = exists(y, and_(and_(and_(p1, p2), p3), rhs));
        v.push_back(p);
      }
      result = multi_or(v.begin(), v.end());
    } else if (s::is_delay_timed(f)) {
      data_expression t = s::time(f);
      atermpp::vector<pbes_expression> v;
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        data_expression ck(i->condition());
        data_expression tk(i->time());
        data_variable_list yk = i->summation_variables();
        pbes_expression p = exists(yk, and_(val(ck), val(d::less_equal(t, tk))));
        v.push_back(p);
      }
      result = or_(multi_or(v.begin(), v.end()), val(d::less_equal(t, T)));
    } else if (s::is_yaled_timed(f)) {
      data_expression t = s::time(f);
      atermpp::vector<pbes_expression> v;
      for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
      {
        data_expression ck(i->condition());
        data_expression tk(i->time());
        data_variable_list yk = i->summation_variables();
        pbes_expression p = exists(yk, and_(val(d::not_(ck)), val(d::greater(t, tk))));
        v.push_back(p);
      }
      result = and_(multi_or(v.begin(), v.end()), val(d::greater(t, T)));
    } else if (s::is_var(f)) {
      identifier_string X = s::var_name(f);
      data_expression_list d = s::var_val(f);
      data_variable_list xp = lps.process_parameters();
      result = propositional_variable_instantiation(X, T + d + xp + Par(X, data_variable_list(), f0));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      result = propositional_variable_instantiation(X, T + d + xp + Par(X, data_variable_list(), f0));
    } else {
      throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
    }
    return result;
  }

  /// f0 is the original formula
  inline
  atermpp::vector<pbes_equation> E(state_formula f0, state_formula f, linear_process lps, data_variable T)
  {
    using namespace lps::state_frm;
    atermpp::vector<pbes_equation> result;

    if (is_data(f)) {
      // do nothing
    } else if (is_true(f)) {
      // do nothing
    } else if (is_false(f)) {
      // do nothing
    } else if (is_not(f)) {
      result = E(f0, not_arg(f), lps, T);
    } else if (is_and(f)) {
      result = E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
    } else if (is_or(f)) {
      result = E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
    } else if (is_imp(f)) {
      result = E(f0, lhs(f), lps, T) + E(f0, rhs(f), lps, T);
    } else if (is_forall(f)) {
      result = E(f0, quant_form(f), lps, T);
    } else if (is_exists(f)) {
      result = E(f0, quant_form(f), lps, T);
    } else if (is_must(f)) {
      result = E(f0, mod_form(f), lps, T);
    } else if (is_may(f)) {
      result = E(f0, mod_form(f), lps, T);
    } else if (is_var(f)) {
      // do nothing
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
      propositional_variable v(X, T + xf + xp + Par(X, data_variable_list(), f0));
      std::set<std::string> context;
      pbes_expression expr = RHS(f0, g, lps, T, context);
      pbes_equation e(sigma, v, expr);
      result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps, T);
    } else if (is_yaled_timed(f)) {
      // do nothing
    } else if (is_delay_timed(f)) {
      // do nothing
    } else {
      throw std::runtime_error(std::string("E[timed] error: unknown state formula ") + f.to_string());
    }
    return result;
  }

} // namespace pbes_timed

namespace pbes_untimed
{
  inline
  pbes_expression sat_top(action_list a, action_formula b)
  {
    using namespace lps::act_frm;
    namespace p = lps::pbes_expr;

    pbes_expression result;

    if (is_mult_act(b)) {
      action_list b_actions = mult_params(b);
      result = equal_data_parameters(a, b_actions);
    } else if (is_true(b)) {
      result = p::true_();
    } else if (is_not(b)) {
      result = p::not_(sat_top(a, not_arg(b)));
    } else if (is_and(b)) {
      result = p::and_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_or(b)) {
      result = p::or_(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_imp(b)) {
      result = p::imp(sat_top(a, lhs(b)), sat_top(a, rhs(b)));
    } else if (is_forall(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_name_strings(make_list(a, b)));
        result = p::forall(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      }
      else
        result = sat_top(a, alpha);
    } else if (is_exists(b)) {
      data_variable_list x = quant_vars(b);
      action_formula alpha = quant_form(b);
      if (x.size() > 0)
      {
        data_variable_list y = fresh_variables(x, find_variable_name_strings(make_list(a, b)));
        result = p::exists(y, sat_top(a, alpha.substitute(make_list_substitution(x, y))));
      }
      else
        result = sat_top(a, alpha);
    } else {
      throw std::runtime_error(std::string("sat_top[untimed] error: unknown action formula ") + b.to_string());
    }
    return result;
  }

  /// f0 is the original formula
  inline
  pbes_expression RHS(state_formula f0, state_formula f, linear_process lps, std::set<std::string>& context)
  {
    using namespace lps::pbes_expr;
    using lps::summand_list;
    namespace s = lps::state_frm;

    pbes_expression result;

    if (s::is_data(f)) {
      result = pbes_expression(f);
    } else if (s::is_true(f)) {
      result = true_();
    } else if (s::is_false(f)) {
      result = false_();
    } else if (s::is_not(f)) {
      result = not_(RHS(f0, s::not_arg(f), lps, context));
    } else if (s::is_and(f)) {
      result = and_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
    } else if (s::is_or(f)) {
      result = or_(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
    } else if (s::is_imp(f)) {
      result = imp(RHS(f0, s::lhs(f), lps, context), RHS(f0, s::rhs(f), lps, context));
    } else if (s::is_forall(f)) {
      std::set<std::string> names = find_variable_name_strings(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      result = forall(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
    } else if (s::is_exists(f)) {
      std::set<std::string> names = find_variable_name_strings(s::quant_vars(f));
      context.insert(names.begin(), names.end());
      result = exists(s::quant_vars(f), RHS(f0, s::quant_form(f), lps, context));
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
        std::set<std::string> rhs_context = find_variable_name_strings(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));
        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = forall(y, imp(and_(p1, p2), rhs));
        v.push_back(p);
      }
      result = multi_and(v.begin(), v.end());
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
        std::set<std::string> rhs_context = find_variable_name_strings(rhs);
        context.insert(rhs_context.begin(), rhs_context.end());
        data_variable_list y = fresh_variables(yi, context);
        ci = ci.substitute(make_list_substitution(yi, y));
        ai = ai.substitute(make_list_substitution(yi, y));
        gi = gi.substitute(make_list_substitution(yi, y));
        pbes_expression p1 = sat_top(ai, alpha);
        pbes_expression p2 = val(ci);
        rhs = rhs.substitute(assignment_list_substitution(gi));

        pbes_expression p = exists(y, and_(and_(p1, p2), rhs));
        v.push_back(p);
      }
      result = multi_or(v.begin(), v.end());
    } else if (s::is_var(f)) {
      identifier_string X = s::var_name(f);
      data_expression_list d = s::var_val(f);
      data_variable_list xp = lps.process_parameters();
      result = propositional_variable_instantiation(X, d + xp + Par(X, data_variable_list(), f0));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      identifier_string X = s::mu_name(f);
      data_expression_list d = mu_expressions(f);
      data_variable_list xp = lps.process_parameters();
      result = propositional_variable_instantiation(X, d + xp + Par(X, data_variable_list(), f0));
    } else {
      throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
    }
    return result;
  }

  /// f0 is the original formula
  inline
  atermpp::vector<pbes_equation> E(state_formula f0, state_formula f, linear_process lps)
  {
    using namespace lps::state_frm;
    atermpp::vector<pbes_equation> result;

    if (is_data(f)) {
      // do nothing
    } else if (is_true(f)) {
      // do nothing
    } else if (is_false(f)) {
      // do nothing
    } else if (is_not(f)) {
      result = E(f0, not_arg(f), lps);
    } else if (is_and(f)) {
      result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
    } else if (is_or(f)) {
      result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
    } else if (is_imp(f)) {
      result = E(f0, lhs(f), lps) + E(f0, rhs(f), lps);
    } else if (is_forall(f)) {
      result = E(f0, quant_form(f), lps);
    } else if (is_exists(f)) {
      result = E(f0, quant_form(f), lps);
    } else if (is_must(f)) {
      result = E(f0, mod_form(f), lps);
    } else if (is_may(f)) {
      result = E(f0, mod_form(f), lps);
    } else if (is_var(f)) {
      // do nothing
    } else if (is_mu(f) || (is_nu(f))) {
      identifier_string X = mu_name(f);
      data_variable_list xf = mu_variables(f);
      data_variable_list xp = lps.process_parameters();
      state_formula g = mu_form(f);
      fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
      propositional_variable v(X, xf + xp + Par(X, data_variable_list(), f0));
      std::set<std::string> context;
      pbes_expression expr = RHS(f0, g, lps, context);
      pbes_equation e(sigma, v, expr);
      result = atermpp::vector<pbes_equation>() + e + E(f0, g, lps);
    } else if (is_yaled_timed(f)) {
      // do nothing
    } else if (is_delay_timed(f)) {
      // do nothing
    } else {
      throw std::runtime_error(std::string("E[untimed] error: unknown state formula ") + f.to_string());
    }
    return result;
  }

} // namespace pbes_untimed

} // namespace detail

} // namespace lps

#endif // MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
