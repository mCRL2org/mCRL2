// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_H
#define MCRL2_PROCESS_ALPHABET_H

#include <algorithm>
#include <iterator>
#include <sstream>
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/process/detail/alphabet_utility.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace process {

namespace detail {

template <typename Derived>
struct alphabet_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  const atermpp::vector<process_equation>& equations;
  std::set<process_identifier>& W;
  atermpp::vector<multi_action_name_set> result_stack;

  // Push A to result_stack
  void push(const multi_action_name_set& A)
  {
    result_stack.push_back(A);
  }

  // Pop the top element of result_stack and return it
  multi_action_name_set pop()
  {
    multi_action_name_set result = result_stack.back();
    result_stack.pop_back();
    return result;
  }

  // Return the top element of result_stack
  multi_action_name_set& top()
  {
    return result_stack.back();
  }

  // Return the top element of result_stack
  const multi_action_name_set& top() const
  {
    return result_stack.back();
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2)
  void join()
  {
    multi_action_name_set A2 = pop();
    multi_action_name_set A1 = pop();
    push(set_union(A1, A2));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2, A1xA2)
  void join_parallel()
  {
    multi_action_name_set A2 = pop();
    multi_action_name_set A1 = pop();
    push(set_union(set_union(A1, A2), times(A1, A2)));
  }

  alphabet_traverser(const atermpp::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_)
  {}

  void operator()(const lps::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    multi_action_name_set A;
    A.insert(alpha);
    push(A);
  }

  void operator()(const process::process_instance& x)
  {
    if (W.find(x.identifier()) == W.end())
    {
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived()(eqn.expression());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void operator()(const process::process_instance_assignment& x)
  {
    if (W.find(x.identifier()) == W.end())
    {
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived()(eqn.expression());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void operator()(const process::delta& x)
  {
    push(multi_action_name_set());
  }

  void operator()(const process::tau& x)
  {
    multi_action_name_set A;
    A.insert(multi_action_name()); // A = { tau }
    push(A);
  }

  // void operator()(const process::sum& x)

  void leave(const process::block& x)
  {
//    top() = apply_block(x.block_set(), top());
  }

  void leave(const process::hide& x)
  {
    top() = apply_hide(x.hide_set(), top());
  }

  void leave(const process::rename& x)
  {
    top() = apply_rename(x.rename_set(), top());
  }

  void leave(const process::comm& x)
  {
    top() = apply_comm(x.comm_set(), top());
  }

  void leave(const process::allow& x)
  {
    top() = apply_allow(x.allow_set(), top());
  }

  void leave(const process::sync& x)
  {
    join_parallel();
  }

  // void operator()(const process::at& x)

  void leave(const process::seq& x)
  {
    join();
  }

  // void operator()(const process::if_then& x)

  void leave(const process::if_then_else& x)
  {
    join();
  }

  void leave(const process::bounded_init& x)
  {
    join();
  }

  void leave(const process::merge& x)
  {
    join_parallel();
  }

  void leave(const process::left_merge& x)
  {
    join_parallel();
  }

  void leave(const process::choice& x)
  {
    join();
  }
};

struct apply_alphabet_traverser: public alphabet_traverser<apply_alphabet_traverser>
{
  typedef alphabet_traverser<apply_alphabet_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::result_stack;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  apply_alphabet_traverser(const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W)
    : alphabet_traverser(equations, W)
  {}
};

inline
multi_action_name_set alphabet(const process_expression& x, const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  apply_alphabet_traverser f(equations, W);
  f(x);
  return f.result_stack.back();
}

} // detail

inline
multi_action_name_set alphabet(const process_expression& x, const atermpp::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::alphabet(x, equations, W);
}

typedef std::pair<process_expression, multi_action_name_set> alphabet_result;

inline
multi_action_name_set remove_tau(const multi_action_name_set& A)
{
  multi_action_name tau;
  multi_action_name_set result = A;
  result.erase(tau);
  return result;
}

inline
bool contains_tau(const multi_action_name_set& A)
{
  multi_action_name tau;
  return A.find(tau) != A.end();
}

inline
process_expression make_allow(const multi_action_name_set& A, const process_expression& x)
{
  assert(!contains_tau(A));

  // convert A to an action_name_multiset_list B
  atermpp::vector<action_name_multiset> v;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    v.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
  }
  action_name_multiset_list B(v.begin(), v.end());
  return allow(B, x);
}

inline
process_expression make_block(const multi_action_name_set& A, const process_expression& x)
{
  std::set<core::identifier_string> v;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    assert(alpha.size() == 1);
    v.insert(*alpha.begin());
  }
  return block(core::identifier_string_list(v.begin(), v.end()), x);
}

inline
multi_action_name_set set_intersection(const multi_action_name_set& A, const multi_action_name& a)
{
  if (A.find(a) == A.end())
  {
    return multi_action_name_set();
  }
  else
  {
    return make_name_set(a);
  }
}

// contains data structures that are used by the alphabet reduction traversers
struct alphabet_parameters
{
  typedef atermpp::vector<process_equation>::iterator equation_iterator;

  // newly generated equations are added to procspec
  process_specification& procspec;

  // maps proces identifiers to their corresponding equations
  atermpp::map<process_identifier, process_equation*> equation_map;

  // equations generated during alphabet reduction
  atermpp::vector<process_equation> added_equations;

  // used for generating process identifiers
  data::set_identifier_generator generator;

  // Searches for an equation corresponding with the given id and right hand side. If it does
  // not yet exist, a new equation is created.
  process_identifier find_equation(const process_identifier& id, const process_expression& rhs)
  {
//std::cout << "<find_equation>" << process::pp(id) << " = " << process::pp(rhs) << std::endl;
    atermpp::map<process_identifier, process_equation*>::iterator i = equation_map.find(id);
    assert(i != equation_map.end());
    const process_equation& eqn = *(i->second);

    // create new equation
    std::string prefix = std::string(id.name()) + "_";
    core::identifier_string X = generator(prefix);
    process_identifier new_id(X, id.sorts());
    process_equation new_eqn(new_id, eqn.formal_parameters(), rhs);

    // update data structures
    added_equations.push_back(new_eqn);

    // return result
    return new_id;
  }

  // returns the body of the process with the given identifier
  process_expression process_body(const process_identifier& id) const
  {
//std::cout << "<process_body>" << process::pp(id) << std::endl;
//for (atermpp::map<process_identifier, process_equation*>::const_iterator i = equation_map.begin(); i != equation_map.end(); ++i)
//{
//  std::cout << process::pp(i->first) << " -> " << std::endl; // << process::pp(*(i->second)) << std::endl;
//}

    atermpp::map<process_identifier, process_equation*>::const_iterator i = equation_map.find(id);
    assert (i != equation_map.end());
    const process_equation& eqn = *(i->second);
    return eqn.expression();
  }

  alphabet_parameters(process_specification& p)
    : procspec(p)
  {
    atermpp::vector<process_equation>& equations = p.equations();
    for (atermpp::vector<process_equation>::iterator i = equations.begin(); i != equations.end(); ++i)
    {
      equation_map[i->identifier()] = &(*i);
      generator.add_identifier(i->identifier().name());
    }
  }
};

} // namespace process

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{

template<>
struct aterm_traits<mcrl2::process::alphabet_result>
{
  static void protect(const mcrl2::process::alphabet_result& t)
  {
    t.first.protect();
  }
  static void unprotect(const mcrl2::process::alphabet_result& t)
  {
    t.first.unprotect();
  }
  static void mark(const mcrl2::process::alphabet_result& t)
  {
    t.first.mark();
  }
};

} // namespace atermpp
/// \endcond

namespace mcrl2 {

namespace process {

// prototype declarations
alphabet_result push_allow(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, bool A_is_Act, alphabet_parameters& parameters);
alphabet_result push_sub(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters);
alphabet_result push_block(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters);

// implements alphabet reduction for (most) pCRL expressions
template <typename Derived>
struct default_push_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // the parameter A
  const multi_action_name_set& A;

  std::set<process_identifier>& W;

  // algorithm parameters
  alphabet_parameters& parameters;

  // if P = p is an equation and push(P, A, W) = <p', A'_p> then process_instance_map[P] = p'
  atermpp::map<process_identifier, process_expression> process_instance_map;

  // stack with intermediate results
  typedef std::pair<process_expression, multi_action_name_set> alphabet_result;
  atermpp::vector<alphabet_result> result_stack;

  // used for debugging
  mutable std::string indent;

  // Constructor
  default_push_traverser(const multi_action_name_set& A_, std::set<process_identifier>& W_, alphabet_parameters& parameters_)
    : A(A_), W(W_), parameters(parameters_)
  {}

  std::string print_W() const
  {
    std::ostringstream out;
    out << "{ ";
    for (std::set<process_identifier>::const_iterator i = W.begin(); i != W.end(); ++i)
    {
      if (i != W.begin())
      {
        out << ", ";
      }
      out << std::string(i->name());
    }
    out << " }";
    return out.str();
  }

  void enter_expression(const process_expression& x, const std::string& msg) const
  {
    indent += "  ";
    mCRL2log(log::debug) << indent << "entering" << msg << " " << process::pp(x) << ", A = " << lps::pp(A) << ", W = " << print_W() << std::endl;
  }

  // prints the top n elements of the stack
  void leave_expression(const process_expression& x, const std::string& msg = "") const
  {
    mCRL2log(log::debug) << indent << "leaving " << msg << " " << process::pp(x) << ", A = " << lps::pp(A) << ", W = " << print_W() << std::endl;
    const alphabet_result& r = result_stack.back();
    mCRL2log(log::debug) << indent << "result: p = " << process::pp(r.first) << ", A = " << lps::pp(r.second) << std::endl;
    indent.erase(indent.size() - 2, 2);
  }

  // Push (p, A) to result_stack
  void push(const process_expression& p, const multi_action_name_set& A)
  {
    result_stack.push_back(alphabet_result(p, A));
  }

  // Push x to result_stack
  void push(const alphabet_result& x)
  {
    result_stack.push_back(x);
  }

  // Pop the last element of result_stack and return it
  alphabet_result pop()
  {
    alphabet_result result = result_stack.back();
    result_stack.pop_back();
    return result;
  }

  // Return the top element of result_stack
  alphabet_result& top()
  {
    return result_stack.back();
  }

  // Return the top element of result_stack
  const alphabet_result& top() const
  {
    return result_stack.back();
  }

  // function that transforms a process expression using the set A
  // N.B. Override this function in derived classes!
  process_expression f(const process_expression& x, const multi_action_name_set& A)
  {
    return x;
  }

  // Pops two elements (q, Aq) and (p, Ap) from the stack, and pushes back (f(x), union(Ap, Aq))
  void join(const process_expression& x)
  {
    alphabet_result right = pop();
    alphabet_result left = pop();
    push(derived().f(x, A), set_union(left.second, right.second));
  }

  // delta
  void leave(const process::delta& x)
  {
    push(x, multi_action_name_set());
  }

  // P(e1, ..., en)
  void operator()(const process::process_instance& x)
  {
    enter_expression(x, " process_instance[default]");
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      process_expression p = parameters.process_body(x.identifier());
      derived()(p);    // now <p', A'_p> is on the stack
std::cout << "P = " << process::pp(x) << " p = " << process::pp(p) << " p1 = " << process::pp(top().first) << std::endl;
      if (p == top().first)
      {
        top().first = x;
      }
      W.erase(x.identifier());
    }
    else
    {
      push(x, multi_action_name_set());
    }
    leave_expression(x, " process_instance[default]");
  }

  // P(d1 = e1, ..., dn = en)
  void operator()(const process::process_instance_assignment& x)
  {
    enter_expression(x, " process_instance_assignment[default]");
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      process_expression p = parameters.process_body(x.identifier());
      derived()(p);    // now <p', A'_p> is on the stack
std::cout << "P = " << process::pp(x) << " p = " << process::pp(p) << " p1 = " << process::pp(top().first) << std::endl;
      if (p == top().first)
      {
        top().first = x;
      }
      W.erase(x.identifier());
    }
    else
    {
      push(x, multi_action_name_set());
    }
    leave_expression(x, " process_instance_assignment[default]");
  }

  // p1 + p2
  void leave(const process::choice& x)
  {
    enter_expression(x, " choice[default]");
    join(x);
    leave_expression(x, " choice[default]");
  }

  // p1 . p2
  void leave(const process::seq& x)
  {
    enter_expression(x, " seq[default]");
    join(x);
    leave_expression(x, " seq[default]");
  }

  // c -> p
  void leave(const process::if_then& x)
  {
    enter_expression(x, " if_then[default]");
    top().first = derived().f(x, A);
    leave_expression(x, " if_then[default]");
  }

  // c -> p1 <> p2
  void leave(const process::if_then_else& x)
  {
    enter_expression(x, " if_then_else[default]");
    join(x);
    leave_expression(x, " if_then_else[default]");
  }

  // sum d:D . p
  void leave(const process::sum& x)
  {
    enter_expression(x, " sum[default]");
    top().first = derived().f(x, A);
    leave_expression(x, " sum[default]");
  }

  // p @ t
  void leave(const process::at& x)
  {
    enter_expression(x, " at[default]");
    top().first = derived().f(x, A);
    leave_expression(x, " at[default]");
  }

  // p << q
  void operator()(const process::bounded_init& x)
  {
    enter_expression(x, " bounded_init[default]");
    derived()(x.left());
    leave_expression(x, " bounded_init[default]");
  }
};

template <typename Derived>
struct push_allow_traverser: public default_push_traverser<Derived>
{
  typedef default_push_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::A;
  using super::W;
  using super::push;
  using super::pop;
  using super::top;
  using super::parameters;
  using super::f;
  using super::enter_expression;
  using super::leave_expression;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // if true, A represents the set of all multi action names
  bool A_is_Act;

  // function that transforms a process expression using the set A
  process_expression f(const process_expression& x, const multi_action_name_set& A)
  {
    return make_allow(remove_tau(A), x);
  }

  // Constructor
  push_allow_traverser(const multi_action_name_set& A, std::set<process_identifier>& W, bool A_is_Act_, alphabet_parameters& parameters)
    : super(A, W, parameters), A_is_Act(A_is_Act_)
  {}

  // a(e1, ..., en)
  void operator()(const lps::action& x)
  {
    enter_expression(x, " action[allow]");
    multi_action_name a = name(x);
    if (A.find(a) != A.end())
    {
      multi_action_name_set A1;
      A1.insert(a);
      push(x, A1);
    }
    else
    {
      push(delta(), multi_action_name_set());
    }
    leave_expression(x, " action[allow]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    enter_expression(x, " tau[allow]");
    multi_action_name tau;
    multi_action_name_set A1 = set_intersection(A, tau);
    push(make_allow(A, x), A1);
    leave_expression(x, " tau[allow]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
    enter_expression(x, " merge[allow]");
    if (A_is_Act)
    {
      derived()(x.left());
      derived()(x.right());
      alphabet_result right = pop();
      alphabet_result left = pop();
      const process_expression& p1 = left.first;
      const multi_action_name_set& Ap1 = left.second;
      const process_expression& q1 = right.first;
      const multi_action_name_set& Aq1 = right.second;
      multi_action_name_set A1 = set_union(Ap1, set_union(Aq1, times(Ap1, Aq1)));
      push(make_allow(remove_tau(A1), merge(p1, q1)), A1);
    }
    else
    {
      process_expression p = x.left();
      process_expression q = x.right();
      alphabet_result r = push_sub(p, A, W, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      alphabet_result s = push_allow(q, set_union(A, left_arrow(A, Ap1)), W, false, parameters);
      const process_expression& q1 = s.first;
      const multi_action_name_set& Aq1 = s.second;
      multi_action_name_set A1 = set_intersection(A, set_union(Ap1, set_union(Aq1, times(Ap1, Aq1))));;
      push(make_allow(remove_tau(A1), merge(p1, q1)), A1);
    }
    leave_expression(x, " merge[allow]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    enter_expression(x, " left_merge[allow]");
    if (A_is_Act)
    {
      derived()(x.left());
      derived()(x.right());
      alphabet_result right = pop();
      alphabet_result left = pop();
      const process_expression& p1 = left.first;
      const multi_action_name_set& Ap1 = left.second;
      const process_expression& q1 = right.first;
      const multi_action_name_set& Aq1 = right.second;
      multi_action_name_set A1 = set_union(Ap1, set_union(Aq1, times(Ap1, Aq1)));
      push(make_allow(remove_tau(A1), left_merge(p1, q1)), A1);
    }
    else
    {
      process_expression p = x.left();
      process_expression q = x.right();
      alphabet_result r = push_sub(p, A, W, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      alphabet_result s = push_allow(q, set_union(A, left_arrow(A, Ap1)), W, false, parameters);
      const process_expression& q1 = s.first;
      const multi_action_name_set& Aq1 = s.second;
      multi_action_name_set A1 = set_intersection(A, set_union(Ap1, set_union(Aq1, times(Ap1, Aq1))));;
      push(make_allow(remove_tau(A1), left_merge(p1, q1)), A1);
    }
    leave_expression(x, " left_merge[allow]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    enter_expression(x, " sync[allow]");
    if (A_is_Act)
    {
      derived()(x.left());
      derived()(x.right());
      alphabet_result right = pop();
      alphabet_result left = pop();
      process_expression p1 = left.first;
      multi_action_name_set Ap1 = left.second;
      process_expression q1 = right.first;
      multi_action_name_set Aq1 = right.second;
      multi_action_name_set A1 = times(Ap1, Aq1);
      push(make_allow(remove_tau(A1), sync(p1, q1)), A1);
    }
    else
    {
      process_expression p = x.left();
      process_expression q = x.right();
      alphabet_result r = push_sub(p, A, W, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      alphabet_result s = push_allow(q, set_union(A, left_arrow(A, Ap1)), W, false, parameters);
      const process_expression& q1 = s.first;
      const multi_action_name_set& Aq1 = s.second;
      multi_action_name_set A1 = set_intersection(A, times(Ap1, Aq1));
      push(make_allow(remove_tau(A1), sync(p1, q1)), A1);
    }
    leave_expression(x, " sync[allow]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    enter_expression(x, " rename[allow]");
    rename_expression_list R = x.rename_set();
    if (A_is_Act)
    {
      // TODO
      throw std::runtime_error("not implemented yet");
    }
    else
    {
      process_expression p = x.operand();
      derived()(p); // now <p', A'_p> is on the stack
      const process_expression& p1 = top().first;
      const multi_action_name_set& Ap1 = top().second;
      top().first = rename(R, p1);
      top().second = apply_rename(R, Ap1);
    }
    leave_expression(x, " rename[allow]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    enter_expression(x, " block[allow]");
    core::identifier_string_list B = x.block_set();
    if (A_is_Act)
    {
      multi_action_name_set A1 = make_name_set(B);
      push(push_block(x.operand(), A1, W, parameters));
    }
    else
    {
      multi_action_name_set A1 = apply_block(B, A);
      push(push_allow(x.operand(), A1, W, false, parameters));
    }
    leave_expression(x, " block[allow]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    enter_expression(x, " hide[allow]");
    core::identifier_string_list I = x.hide_set();
    if (A_is_Act)
    {
      process_expression p = x.operand();
      alphabet_result r = push_allow(p, A, W, true, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      multi_action_name_set A1 = apply_hide(I, Ap1);
      push(make_allow(remove_tau(A), hide(I, p1)), A1);
    }
    else
    {
      process_expression p = x.operand();
      alphabet_result r = push_allow(p, A, W, true, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      multi_action_name_set A1 = set_intersection(A, apply_hide(I, Ap1));
      push(make_allow(remove_tau(A), hide(I, p1)), A1);
    }
    leave_expression(x, " hide[allow]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    enter_expression(x, " comm[allow]");
    communication_expression_list C = x.comm_set();
    if (A_is_Act)
    {
      // TODO
      throw std::runtime_error("not implemented yet");
    }
    else
    {
      process_expression p = x.operand();
      alphabet_result r = push_allow(p, set_union(A, apply_comm_inverse(C, A)), W, true, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      multi_action_name_set A1 = set_intersection(A, apply_comm(C, Ap1));
      push(make_allow(remove_tau(A), comm(C, p1)), A1);
    }
    leave_expression(x, " comm[allow]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    enter_expression(x, " allow[allow]");
    multi_action_name_set V = make_name_set(x.allow_set());
    process_expression p = x.operand();
    if (A_is_Act)
    {
      push(push_allow(p, V, W, false, parameters));
    }
    else
    {
      push(push_allow(p, set_intersection(A, V), W, false, parameters));
    }
    leave_expression(x, " allow[allow]");
  }
};

template <typename Derived>
struct push_sub_traverser: public default_push_traverser<Derived>
{
  typedef default_push_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::A;
  using super::W;
  using super::push;
  using super::pop;
  using super::top;
  using super::parameters;
  using super::f;
  using super::enter_expression;
  using super::leave_expression;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // function that transforms a process expression using the set A
  process_expression f(const process_expression& x, const multi_action_name_set& A)
  {
    return make_allow(remove_tau(A), x);
  }

  // Constructor
  push_sub_traverser(const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters)
    : super(A, W, parameters)
  {}

  // a(e1, ..., en)
  void operator()(const lps::action& x)
  {
    enter_expression(x, " action[sub]");
    multi_action_name a = name(x);
    multi_action_name_set A1 = set_intersection(A, a);
    push(make_allow(A1, x), A1);
    leave_expression(x, " action[sub]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    enter_expression(x, " tau[sub]");
    multi_action_name_set A1;
    A1.insert(multi_action_name()); // A1 = { tau }
    push(tau(), A1);
    leave_expression(x, " tau[sub]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
    enter_expression(x, " merge[sub]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    const process_expression& p1 = left.first;
    const multi_action_name_set& Ap1 = left.second;
    const process_expression& q1 = right.first;
    const multi_action_name_set& Aq1 = right.second;
    multi_action_name_set A1 = subset_intersection(A, set_union(Ap1, set_union(Aq1, times(Ap1, Aq1))));;
    push(make_allow(remove_tau(A1), merge(p1, q1)), A1);
    leave_expression(x, " merge[sub]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    enter_expression(x, " left_merge[sub]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    const process_expression& p1 = left.first;
    const multi_action_name_set& Ap1 = left.second;
    const process_expression& q1 = right.first;
    const multi_action_name_set& Aq1 = right.second;
    multi_action_name_set A1 = subset_intersection(A, set_union(Ap1, set_union(Aq1, times(Ap1, Aq1))));;
    push(make_allow(remove_tau(A1), left_merge(p1, q1)), A1);
    leave_expression(x, " left_merge[sub]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    enter_expression(x, " sync[sub]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    process_expression p1 = left.first;
    multi_action_name_set Ap1 = left.second;
    process_expression q1 = right.first;
    multi_action_name_set Aq1 = right.second;
    multi_action_name_set A1 = subset_intersection(A, times(Ap1, Aq1));
    push(make_allow(remove_tau(A1), sync(p1, q1)), A1);
    leave_expression(x, " sync[sub]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    enter_expression(x, " rename[sub]");
    rename_expression_list R = x.rename_set();
    process_expression p = x.operand();
    derived()(p); // now <p', A'_p> is on the stack
    const process_expression& p1 = top().first;
    const multi_action_name_set& Ap1 = top().second;
    top().first = rename(R, p1);
    top().second = apply_rename(R, Ap1);
    leave_expression(x, " rename[sub]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    enter_expression(x, " block[sub]");
    core::identifier_string_list B = x.block_set();
    multi_action_name_set A1 = apply_block(B, A);
    push(push_allow(x.operand(), A1, W, false, parameters));
    leave_expression(x, " block[sub]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    enter_expression(x, " hide[sub]");
    process_expression p = x.operand();
    alphabet_result r = push_allow(p, A, W, true, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    core::identifier_string_list I = x.hide_set();
    multi_action_name_set A1 = set_intersection(A, apply_hide(I, Ap1));
    push(make_allow(remove_tau(A1), hide(I, p1)), A1);
    leave_expression(x, " hide[sub]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    enter_expression(x, " comm[sub]");
    communication_expression_list C = x.comm_set();
    process_expression p = x.operand();
    alphabet_result r = push_sub(p, set_union(A, apply_comm_inverse(C, A)), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = set_intersection(A, apply_comm(C, Ap1));
    multi_action_name_set A2 = subset_intersection(A, apply_comm(C, Ap1));
    push(make_allow(remove_tau(A1), comm(C, p1)), A2);
    leave_expression(x, " comm[sub]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    enter_expression(x, " allow[sub]");
    multi_action_name_set V = make_name_set(x.allow_set());
    process_expression p = x.operand();
    multi_action_name_set A1 = subset_intersection(A, V);
    push(push_sub(p, A1, W, parameters));
    leave_expression(x, " allow[sub]");
  }
};

template <typename Derived>
struct push_block_traverser: public default_push_traverser<Derived>
{
  typedef default_push_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::A;
  using super::W;
  using super::push;
  using super::pop;
  using super::top;
  using super::parameters;
  using super::f;
  using super::enter_expression;
  using super::leave_expression;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // function that transforms a process expression using the set A
  process_expression f(const process_expression& x, const multi_action_name_set& A)
  {
    return make_block(A, x);
  }

  // Constructor
  push_block_traverser(const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters)
    : super(A, W, parameters)
  {}

  // a(e1, ..., en)
  void operator()(const lps::action& x)
  {
    enter_expression(x, " action[block]");
    multi_action_name a = name(x);
    multi_action_name_set A1 = set_intersection(A, a);
    push(make_block(A, x), A1);
    leave_expression(x, " action[block]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    enter_expression(x, " merge[block]");
    multi_action_name tau;
    multi_action_name_set A1;
    A1.insert(tau);
    push(x, A1);
    leave_expression(x, " tau[block]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
    enter_expression(x, " merge[block]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    const process_expression& p1 = left.first;
    const multi_action_name_set& Ap1 = left.second;
    const process_expression& q1 = right.first;
    const multi_action_name_set& Aq1 = right.second;
    multi_action_name_set A1 = set_union(Ap1, set_union(Aq1, times(Ap1, Aq1)));
    push(merge(p1, q1), A1);
    leave_expression(x, " merge[block]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    enter_expression(x, " left_merge[block]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    const process_expression& p1 = left.first;
    const multi_action_name_set& Ap1 = left.second;
    const process_expression& q1 = right.first;
    const multi_action_name_set& Aq1 = right.second;
    multi_action_name_set A1 = set_union(Ap1, set_union(Aq1, times(Ap1, Aq1)));
    push(left_merge(p1, q1), A1);
    leave_expression(x, " left_merge[block]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    enter_expression(x, " sync[block]");
    derived()(x.left());
    derived()(x.right());
    alphabet_result right = pop();
    alphabet_result left = pop();
    process_expression p1 = left.first;
    multi_action_name_set Ap1 = left.second;
    process_expression q1 = right.first;
    multi_action_name_set Aq1 = right.second;
    multi_action_name_set A1 = times(Ap1, Aq1);
    push(sync(p1, q1), A1);
    leave_expression(x, " sync[block]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    enter_expression(x, " rename[block]");
    rename_expression_list R = x.rename_set();
    process_expression p = x.operand();
    alphabet_result r = push_sub(p, apply_rename(R, apply_rename_inverse(R, A)), W, parameters);
    process_expression p1 = r.first;
    multi_action_name_set Ap1 = r.second;
    multi_action_name_set A1 = apply_rename(R, Ap1);
    push(rename(R, p1), A1);
    leave_expression(x, " rename[block]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    enter_expression(x, " block[block]");
    multi_action_name_set B = make_name_set(x.block_set());
    multi_action_name_set A1 = set_union(A, B);
    process_expression p = x.operand();
    push(push_block(p, A1, W, parameters));
    leave_expression(x, " block[block]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    enter_expression(x, " hide[block]");
    core::identifier_string_list I = x.hide_set();
    process_expression p = x.operand();
    alphabet_result r = push_block(p, set_difference(A, make_name_set(I)), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = apply_hide(I, Ap1);
    push(hide(I, p1), A1);
    leave_expression(x, " hide[block]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    enter_expression(x, " comm[block]");
    communication_expression_list C = x.comm_set();
    process_expression p = x.operand();
    multi_action_name_set A2 = apply_comm_bar(C, A);
    alphabet_result r = push_block(p, set_difference(A, A2), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = detail::apply_block(A, apply_comm(C, Ap1));
    communication_expression_list C1; // = ???
    push(make_block(A2, comm(C1, p1)), A1);
    leave_expression(x, " comm[block]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    enter_expression(x, " allow[block]");
    multi_action_name_set V = make_name_set(x.allow_set());
    process_expression p = x.operand();
    multi_action_name_set A1 = subset_intersection(A, V);
    push(push_sub(p, A1, W, parameters));
    leave_expression(x, " allow[block]");
  }
};

template <template <class> class Traverser>
struct apply_push_traverser_all: public Traverser<apply_push_traverser_all<Traverser> >
{
  typedef Traverser<apply_push_traverser_all<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_traverser_all(const multi_action_name_set& A, std::set<process_identifier>& W, bool A_is_Act, alphabet_parameters& parameters)
    : super(A, W, A_is_Act, parameters)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser>
struct apply_push_traverser: public Traverser<apply_push_traverser<Traverser> >
{
  typedef Traverser<apply_push_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_traverser(const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters)
    : super(A, W, parameters)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
alphabet_result push_allow(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, bool A_is_Act, alphabet_parameters& parameters)
{
  apply_push_traverser_all<push_allow_traverser> f(A, W, A_is_Act, parameters);
  f(x);
  return f.result_stack.back();
}

inline
alphabet_result push_allow(const process_expression& x, const multi_action_name_set& A, bool A_is_Act, process_specification& procspec)
{
  alphabet_parameters parameters(procspec);
  std::set<process_identifier> W;
  return push_allow(x, A, W, A_is_Act, parameters);
}

inline
alphabet_result push_sub(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters)
{
  apply_push_traverser<push_sub_traverser> f(A, W, parameters);
  f(x);
  return f.result_stack.back();
}

inline
alphabet_result push_sub(const process_expression& x, const multi_action_name_set& A, process_specification& procspec)
{
  alphabet_parameters parameters(procspec);
  std::set<process_identifier> W;
  return push_sub(x, A, W, parameters);
}

inline
alphabet_result push_block(const process_expression& x, const multi_action_name_set& A, std::set<process_identifier>& W, alphabet_parameters& parameters)
{
  apply_push_traverser<push_block_traverser> f(A, W, parameters);
  f(x);
  return f.result_stack.back();
}

inline
alphabet_result push_block(const process_expression& x, const multi_action_name_set& A, process_specification& procspec)
{
  alphabet_parameters parameters(procspec);
  std::set<process_identifier> W;
  return push_block(x, A, W, parameters);
}

inline
multi_action_name_set multi_action_names(const process_specification& procspec)
{
  multi_action_name_set result;
  std::set<core::identifier_string> names = process::find_action_names(procspec.equations());
  std::set<core::identifier_string> names1 = process::find_action_names(procspec.init());
  names.insert(names1.begin(), names1.end());
  for (std::set<core::identifier_string>::const_iterator i = names.begin(); i != names.end(); ++i)
  {
    multi_action_name alpha;
    alpha.insert(*i);
    result.insert(alpha);
  }
  return result;
}

inline
void alphabet_reduce(process_specification& procspec)
{
  alphabet_parameters parameters(procspec);
  std::set<process_identifier> W;
  multi_action_name_set A = multi_action_names(procspec);
  alphabet_result r = push_allow(procspec.init(), A, W, false, parameters);
  procspec.equations().insert(procspec.equations().end(), parameters.added_equations.begin(), parameters.added_equations.end());
  procspec.init() = r.first;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
