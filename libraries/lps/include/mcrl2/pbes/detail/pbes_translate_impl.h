#ifndef LPS_DETAIL_PBES_TRANSLATE_IMPL_H
#define LPS_DETAIL_PBES_TRANSLATE_IMPL_H

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
#include "mcrl2/lps/data_functional.h"
#include "mcrl2/lps/data_utility.h"
#include "mcrl2/lps/data_operators.h"
#include "mcrl2/lps/data_expression.h"
#include "mcrl2/lps/mucalculus.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace lps {
  
namespace detail {

using atermpp::aterm_appl;
using atermpp::make_substitution;

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

/// Fresh identifier generator that generates the names X, Y, Z, X0, Y0, Z0, X1, ...
class XYZ_identifier_generator
{
  protected:
    atermpp::set<identifier_string> m_identifiers;
    int m_index; // index of last generated identifier
    char m_char; // character of last generated identifier

    /// \brief Returns the next name in the range X, Y, Z, X0, Y0, Z0, X1, ...
    std::string next()
    {
      switch (m_char) {
        case 'X' : {
          m_char = 'Y';
          break;
        }
        case 'Y' : {
          m_char = 'Z';
          break;
        }
        case 'Z' : {
          m_char = 'X';
          m_index++;
          break;
        }
      }
      return m_index < 0 ? std::string(1, m_char) : str(boost::format("%1%%2%") % m_char % m_index);
    }

  public:
    XYZ_identifier_generator()
     : m_index(-2), m_char('Z')
    { }

    template <typename Term>
    XYZ_identifier_generator(Term context)
     : m_index(-2), m_char('Z')
    {
      m_identifiers = propositional_variable_names(context);
    }

    /// Set a new context.
    template <typename Term>
    void set_context(Term context)
    {
      m_identifiers = identifiers(context);
    }

    /// Add term t to the context.
    template <typename Term>
    void add_to_context(Term t)
    {
      std::set<identifier_string> ids = propositional_variable_names(t);
      std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
    }

    /// Add all terms of the sequence [first .. last) to the context.
    template <typename Iter>
    void add_to_context(Iter first, Iter last)
    {
      for (Iter i = first; i != last; ++i)
      {
        std::set<identifier_string> ids = propositional_variable_names(*i);
        std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
      }
    }

    /// \brief Returns the next identifier in the range X, Y, Z, X0, Y0, Z0, X1, ...
    /// and that doesn't appear in the context. The returned variable is added to
    /// the context.
    identifier_string operator()()
    {
      std::string name;
      do {
        name = next();
      } while (m_identifiers.find(name) != m_identifiers.end());
      return identifier_string(name);
    }

    /// \brief Returns the next identifier in the range X, Y, Z, X0, Y0, Z0, X1, ...
    /// and that doesn't appear in the context. The returned variable is added to
    /// the context.
    identifier_string operator()(identifier_string /* dummy */)
    {
      return (*this)();
    }
};

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
    if (i->label().name() != j->label().name())
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
      data_expression_list d = mu_expressions(f);
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
      fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
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
      data_expression_list d = mu_expressions(f);
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
      fixpoint_symbol sigma = is_mu(f) ? fixpoint_symbol::mu() : fixpoint_symbol::nu();
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
template <typename IdentifierGenerator>
state_formula remove_name_clashes_impl(state_formula f, IdentifierGenerator& generator, std::map<identifier_string, identifier_string>& replacements)
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
  XYZ_identifier_generator generator1(make_list(formula, spec));
  return remove_name_clashes_impl(formula, generator1, replacements);
}

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_PBES_TRANSLATE_IMPL_H
