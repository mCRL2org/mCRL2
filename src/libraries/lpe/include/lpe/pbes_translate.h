///////////////////////////////////////////////////////////////////////////////
/// \file pbes.h
/// Contains pbes data structures for the LPE Library.

#ifndef LPE_PBES_TRANSLATE_H
#define LPE_PBES_TRANSLATE_H

#include <string>
#include <iostream>
#include <stdexcept>

#include "atermpp/aterm_access.h"
#include "atermpp/vector.h"
#include "atermpp/algorithm.h"
#include "atermpp/substitute.h"
#include "lpe/data_functional.h"
#include "lpe/data_utility.h"
#include "lpe/data_init.h"
#include "lpe/data_operators.h"
#include "lpe/mucalculus.h"
#include "lpe/mucalculus_init.h"
#include "lpe/pbes.h"
#include "lpe/pbes_init.h"
#include "lpe/specification.h"
#include "lpe/data_utility.h"
#include "lpe/lpe_algorithm.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::make_substitution;
using atermpp::arg1;
using atermpp::arg2;
using atermpp::arg3;
using atermpp::list_arg1;
using atermpp::list_arg2;
using atermpp::list_arg3;

inline
action_formula act_arg1(ATermAppl t)
{
  return action_formula(aterm_appl(t).argument(0));
}

inline
action_formula act_arg2(ATermAppl t)
{
  return action_formula(aterm_appl(t).argument(1));
}

inline
action_formula act_arg3(ATermAppl t)
{
  return action_formula(aterm_appl(t).argument(2));
}

inline
data_variable_list state_formula_variables(state_formula f)
{
  data_variable_init_list l(list_arg2(f));
  data_variable_list result;
  for (data_variable_init_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->to_variable());
  }
  return atermpp::reverse(result);
}

inline
data_expression_list state_formula_variable_expressions(state_formula f)
{
  data_variable_init_list l(list_arg2(f));
  data_expression_list result;
  for (data_variable_init_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->to_expression());
  }
  return atermpp::reverse(result);
}

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
  namespace p = lpe::pbes_init;
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
  namespace p = lpe::pbes_init;
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
data_variable_list Par(aterm_string x, state_formula f)
{
  using namespace lpe::state_init;

  if (is_data(f)) {
    return data_variable_list();
  } else if (is_true(f)) {
    return data_variable_list();
  } else if (is_false(f)) {
    return data_variable_list();
  } else if (is_and(f)) {
    return Par(x, arg1(f)) + Par(x, arg2(f));
  } else if (is_or(f)) {
    return Par(x, arg1(f)) + Par(x, arg2(f));
  } else if (is_must(f)) {
    return Par(x, arg2(f));
  } else if (is_may(f)) {
    return Par(x, arg2(f));
  } else if (is_forall(f)) {
    return data_variable(arg1(f)) + Par(x, arg2(f));
  } else if (is_exists(f)) {
    return data_variable(arg1(f)) + Par(x, arg2(f));
  } else if (is_var(f)) {
    return data_variable_list();
  } else if (is_mu(f) || (is_nu(f))) {
    aterm_string name(arg1(f));
    if (name == x)
    {
      return data_variable_list();
    }
    else
    {
      data_variable_list xf = state_formula_variables(f);
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
    using namespace lpe::act_init;
    namespace p = lpe::pbes_init;
  
    if (is_mult_act(b)) {
      action_list b_actions(list_arg1(b));
      if (equal_names(a.actions(), b_actions)) {
        return not_equal_data_parameters(a.actions(), b_actions);
      }
      else
        return p::true_();
    } else if (is_true(b)) {
      return p::false_();
    } else if (is_at(b)) {
      data_expression t = a.time();
      action_formula alpha(act_arg1(b));
      data_expression t1(arg2(b));
      return p::or_(sat_bot(a, alpha), p::val(not_equal_to(t, t1)));
    } else if (is_not(b)) {
      return sat_top(a, act_arg1(b));
    } else if (is_and(b)) {
      return p::or_(sat_bot(a, act_arg1(b)), sat_bot(a, act_arg2(b)));
    } else if (is_forall(b)) {
      data_expression_list x(list_arg1(b));
      action_formula alpha(act_arg2(b));
      data_variable_list y = fresh_variable_list(x.size(), make_list(a.actions(), a.time(), b), "y");
      return p::exists(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
    }
    throw std::runtime_error(std::string("sat_bot[timed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }
  
  inline
  pbes_expression sat_top(timed_action a, action_formula b)
  {
    using namespace lpe::act_init;
    namespace p = lpe::pbes_init;
    namespace q = lpe::act_init;
  
    if (is_mult_act(b)) {
      action_list b_actions(list_arg1(b));
      if (equal_names(a.actions(), b_actions)) {
        return equal_data_parameters(a.actions(), b_actions);
      }
      else
        return p::false_();
    } else if (is_true(b)) {
      return p::true_();
    } else if (is_at(b)) {
      data_expression t = a.time();
      action_formula alpha(act_arg1(b));
      data_expression t1(arg2(b));
      return p::and_(sat_top(a, alpha), p::val(equal_to(t, t1)));
    } else if (is_not(b)) {
      return sat_bot(a, act_arg1(b));
    } else if (is_and(b)) {
      return p::and_(sat_top(a, act_arg1(b)), sat_top(a, act_arg2(b)));
    } else if (is_forall(b)) {
      data_expression_list x(list_arg1(b));
      action_formula alpha(act_arg2(b));
      data_variable_list y = fresh_variable_list(x.size(), make_list(a.actions(), a.time(), b), "y");
      return p::forall(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
    }
    throw std::runtime_error(std::string("sat_top[timed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  inline
  pbes_expression RHS(state_formula f, LPE lpe, data_variable T)
  {
    using namespace lpe::pbes_init;
    using lpe::summand_list;
    namespace s = lpe::state_init;
  
    if (s::is_data(f)) {
      return pbes_expression(aterm_appl(f));
    } else if (s::is_true(f)) {
      return true_();
    } else if (s::is_false(f)) {
      return false_();
    } else if (s::is_and(f)) {
      return and_(RHS(arg1(f), lpe, T), RHS(arg2(f), lpe, T));
    } else if (s::is_or(f)) {
      return or_(RHS(arg1(f), lpe, T), RHS(arg2(f), lpe, T));
    } else if (s::is_forall(f)) {
      return forall(list_arg1(f), RHS(arg2(f), lpe, T));
    } else if (s::is_exists(f)) {
      return exists(list_arg1(f), RHS(arg2(f), lpe, T));
    } else if (s::is_must(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(act_arg1(f));
      state_formula f1(arg2(f));
      for (lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression c(i->condition());
        data_expression t(i->time());
        timed_action a(i->actions(), t);
        data_assignment_list g = i->assignments();
        data_variable_list xp(lpe.process_parameters());
        data_variable_list e(i->summation_variables());
        pbes_expression p1 = sat_bot(a, alpha);
        pbes_expression p2 = val(data_init::not_(c));
        pbes_expression p3 = val(less_equal(t, T));
        pbes_expression p4 = RHS(f1, lpe, T);
        p4 = p4.substitute(make_substitution(T, t));
        p4 = p4.substitute(assignment_list_substitution(g));
        pbes_expression p = forall(e, or_(or_(or_(p1, p2), p3), p4));
        v.push_back(p);
      }
      return multi_and(v.begin(), v.end());
    } else if (s::is_may(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(act_arg1(f));
      state_formula f1(arg2(f));
      for (summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression c(i->condition());
        data_expression t(i->time());
        timed_action a(i->actions(), t);
        data_assignment_list g = i->assignments();
        data_variable_list xp(lpe.process_parameters());
        data_variable_list e(i->summation_variables());
        pbes_expression p1 = sat_top(a, alpha);
        pbes_expression p2 = val(c);
        pbes_expression p3 = val(greater(t, T));
        pbes_expression p4 = RHS(f1, lpe, T);
        p4 = p4.substitute(make_substitution(T, t));
        p4 = p4.substitute(assignment_list_substitution(g));
        pbes_expression p = exists(e, and_(and_(and_(p1, p2), p3), p4));
        v.push_back(p);
      }
      return multi_or(v.begin(), v.end());
    } else if (s::is_delay_timed(f)) {
      data_expression t1(arg1(f));
      atermpp::vector<pbes_expression> v;
      for (summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
      {
        data_expression c(i->condition());
        data_expression t(i->time());
        data_variable_list e = i->summation_variables();
        pbes_expression p = exists(e, and_(val(c), val(less_equal(t1, t))));
        v.push_back(p);
      }
      return or_(multi_or(v.begin(), v.end()), val(less_equal(t1, T)));
    } else if (s::is_var(f)) {
      aterm_string X(arg1(f));
      data_expression_list d = list_arg2(f);
      data_variable_list xp = lpe.process_parameters();
      return propvar(X, T + d + xp + Par(X, f));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      aterm_string X(arg1(f));
      data_expression_list d = state_formula_variable_expressions(f);
      data_variable_list xp = lpe.process_parameters();
      return propvar(X, T + d + xp + Par(X, f));
    }
    throw std::runtime_error(std::string("RHS[timed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  inline
  equation_system E(state_formula f, LPE lpe, data_variable T)
  {
    using namespace lpe::state_init;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(arg1(f), lpe, T) + E(arg2(f), lpe, T);
    } else if (is_or(f)) {
      return E(arg1(f), lpe, T) + E(arg2(f), lpe, T);
    } else if (is_forall(f)) {
      return E(arg2(f), lpe, T);
    } else if (is_exists(f)) {
      return E(arg2(f), lpe, T);
    } else if (is_must(f)) {
      return E(arg2(f), lpe, T);
    } else if (is_may(f)) {
      return E(arg2(f), lpe, T);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      aterm_string X(arg1(f));
      data_variable_list xf = state_formula_variables(f);
      data_variable_list xp = lpe.process_parameters();
      state_formula g = arg3(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, T + xf + xp + Par(X, f));
      pbes_expression expr = RHS(g, lpe, T);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(g, lpe, T);
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
//std::cout << "sat_bot(" << pp(b) << ")" << std::endl;
    using namespace lpe::act_init;
    namespace p = lpe::pbes_init;
    namespace q = lpe::act_init;
  
    if (is_mult_act(b)) {
      action_list b_actions(list_arg1(b));
      if (equal_names(a, b_actions)) {
        return not_equal_data_parameters(a, b_actions);
      }
      else
        return p::true_();
    } else if (is_true(b)) {
      return p::false_();
    } else if (is_not(b)) {
      return sat_top(a, act_arg1(b));
    } else if (is_and(b)) {
      return p::or_(sat_bot(a, act_arg1(b)), sat_bot(a, act_arg2(b)));
    } else if (is_forall(b)) {
      data_expression_list x(list_arg1(b));
      action_formula alpha(act_arg2(b));
      data_variable_list y = fresh_variable_list(x.size(), make_list(a, b), "y");
      if (y.size() > 0)
        return p::exists(y, sat_bot(a, alpha.substitute(make_substitution(x, y))));
      else
        return sat_bot(a, alpha.substitute(make_substitution(x, y)));
    }
    throw std::runtime_error(std::string("sat_bot[untimed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }
  
  inline
  pbes_expression sat_top(action_list a, action_formula b)
  {
//std::cout << "sat_top(" << pp(b) << ")" << std::endl;
    using namespace lpe::act_init;
    namespace p = lpe::pbes_init;
  
    if (is_mult_act(b)) {
      action_list b_actions(list_arg1(b));
      if (equal_names(a, b_actions)) {
        return equal_data_parameters(a, b_actions);
      }
      else
        return p::false_();
    } else if (is_true(b)) {
      return p::true_();
    } else if (is_not(b)) {
      return sat_bot(a, act_arg1(b));
    } else if (is_and(b)) {
      return p::and_(sat_top(a, act_arg1(b)), sat_top(a, act_arg2(b)));
    } else if (is_forall(b)) {
      data_expression_list x(list_arg1(b));
      action_formula alpha(act_arg2(b));
      data_variable_list y = fresh_variable_list(x.size(), make_list(a, b), "y");
      if (y.size() > 0)      
        return p::forall(y, sat_top(a, alpha.substitute(make_substitution(x, y))));
      else
        return sat_top(a, alpha.substitute(make_substitution(x, y)));
    }
    throw std::runtime_error(std::string("sat_top[untimed] error: unknown action formula ") + b.to_string());
    return pbes_expression();
  }

  inline
  pbes_expression RHS(state_formula f, LPE lpe)
  {
//std::cout << "RHS(" << pp(f) << ")" << std::endl;
    using namespace lpe::pbes_init;
    using lpe::summand_list;
    namespace s = lpe::state_init;
  
    if (s::is_data(f)) {
      return pbes_expression(aterm_appl(f));
    } else if (s::is_true(f)) {
      return true_();
    } else if (s::is_false(f)) {
      return false_();
    } else if (s::is_and(f)) {
      return and_(RHS(arg1(f), lpe), RHS(arg2(f), lpe));
    } else if (s::is_or(f)) {
      return or_(RHS(arg1(f), lpe), RHS(arg2(f), lpe));
    } else if (s::is_forall(f)) {
      return forall(list_arg1(f), RHS(arg2(f), lpe));
    } else if (s::is_exists(f)) {
      return exists(list_arg1(f), RHS(arg2(f), lpe));
    } else if (s::is_must(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(act_arg1(f));
      state_formula f1(arg2(f));
      for (lpe::summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression c(i->condition());
        action_list a(i->actions());
        data_assignment_list g = i->assignments();
        data_variable_list xp(lpe.process_parameters());
        data_variable_list e(i->summation_variables());
        pbes_expression p1 = sat_bot(a, alpha);
        pbes_expression p2 = val(data_init::not_(c));
        pbes_expression p4 = RHS(f1, lpe);
        p4 = p4.substitute(assignment_list_substitution(g));
        if (e.size() > 0) {
          pbes_expression p = forall(e, or_(or_(p1, p2), p4));
          v.push_back(p);
        } else {
          pbes_expression p = or_(or_(p1, p2), p4);
          v.push_back(p);
        }
      }
      return multi_and(v.begin(), v.end());
    } else if (s::is_may(f)) {
      atermpp::vector<pbes_expression> v;
      action_formula alpha(act_arg1(f));
      state_formula f1(arg2(f));
      for (summand_list::iterator i = lpe.summands().begin(); i != lpe.summands().end(); ++i)
      {
        if (i->is_delta())
          continue;
        data_expression c(i->condition());
        action_list a(i->actions());
        data_assignment_list g = i->assignments();
        data_variable_list xp(lpe.process_parameters());
        data_variable_list e(i->summation_variables());
        pbes_expression p1 = sat_top(a, alpha);
        pbes_expression p2 = val(c);
        pbes_expression p4 = RHS(f1, lpe);
        p4 = p4.substitute(assignment_list_substitution(g));
        if (e.size() > 0) {
          pbes_expression p = exists(e, and_(and_(p1, p2), p4));
          v.push_back(p);
        } else {
          pbes_expression p = and_(and_(p1, p2), p4);
          v.push_back(p);
        }
      }
      return multi_or(v.begin(), v.end());
    } else if (s::is_var(f)) {
      aterm_string X(arg1(f));
      data_expression_list d = list_arg2(f);
      data_variable_list xp = lpe.process_parameters();
      return propvar(X, d + xp + Par(X, f));
    } else if (s::is_mu(f) || (s::is_nu(f))) {
      aterm_string X(arg1(f));
      data_expression_list d = state_formula_variable_expressions(f);
      data_variable_list xp = lpe.process_parameters();
      return propvar(X, d + xp + Par(X, f));
    }
    throw std::runtime_error(std::string("RHS[untimed] error: unknown state formula ") + f.to_string());
    return pbes_expression();
  }

  inline
  equation_system E(state_formula f, LPE lpe)
  {
//std::cout << "E(" << pp(f) << ")" << std::endl;
    using namespace lpe::state_init;
  
    if (is_data(f)) {
      return equation_system();
    } else if (is_true(f)) {
      return equation_system();
    } else if (is_false(f)) {
      return equation_system();
    } else if (is_and(f)) {
      return E(arg1(f), lpe) + E(arg2(f), lpe);
    } else if (is_or(f)) {
      return E(arg1(f), lpe) + E(arg2(f), lpe);
    } else if (is_forall(f)) {
      return E(arg2(f), lpe);
    } else if (is_exists(f)) {
      return E(arg2(f), lpe);
    } else if (is_must(f)) {
      return E(arg2(f), lpe);
    } else if (is_may(f)) {
      return E(arg2(f), lpe);
    } else if (is_var(f)) {
      return equation_system();
    } else if (is_mu(f) || (is_nu(f))) {
      aterm_string X(arg1(f));
      data_variable_list xf = state_formula_variables(f);
      data_variable_list xp = lpe.process_parameters();
      state_formula g = arg3(f);
      pbes_fixpoint_symbol sigma = is_mu(f) ? pbes_equation::mu() : pbes_equation::nu();
      propositional_variable v(X, xf + xp + Par(X, f));
      pbes_expression expr = RHS(g, lpe);
      pbes_equation e(sigma, v, expr);
      return equation_system(e) + E(g, lpe);
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
  std::set<data_variable> variables = lpe::find_variables(spec);
   
  // find all data variables in f
  std::set<data_variable> formula_variables = lpe::find_variables(f);

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

// translate a state_formula and an LPE to a pbes
inline
pbes pbes_translate(state_formula f, specification spec, bool untimed = true)
{
  using namespace state_init;

  // rename variables in f, to prevent name clashes with variables in spec
  f = remove_name_clashes(spec, f);
//std::cout << "formula (rewritten) " << pp(f) << std::endl;

  // wrap the formula inside a 'nu' if needed
  if (!is_mu(f) && !is_nu(f))
  {
    aterm_list context = make_list(f, spec);
    aterm_string X = fresh_identifier("X", context);
    f = nu(X, data_variable_init_list(), f);
  }  
  LPE lpe = spec.lpe();
  equation_system e;

  if (untimed)
  {
    using namespace pbes_untimed;
    e = E(f, lpe);   
  }
  else
  {
    using namespace pbes_timed;
    data_variable T = fresh_variable("T", make_list(f, lpe));
    aterm_list context = make_list(T, spec.initial_state(), lpe, f);
    lpe = make_timed_lpe(lpe, context);
    e = E(f, lpe, T);
  }

  // create initial state
  assert(e.size() > 0);
  pbes_equation e1 = e.front();
  aterm_string Xe(e1.variable().name());
  assert(is_mu(f) || is_nu(f));
  aterm_string Xf(arg1(f));
  data_expression_list fi = state_formula_variable_expressions(f);
  data_expression_list pi = spec.initial_state();

  if (untimed)
  {   
    propositional_variable_instantiation init(Xe, fi + pi + Par(Xf, f));
    return pbes(spec.data(), e, init);
  }
  else
  {
    propositional_variable_instantiation init(Xe, data_init::real(0) + fi + pi + Par(Xf, f));
    return pbes(spec.data(), e, init);
  }
}

} // namespace lpe

#endif // LPE_PBES_TRANSLATE_H
