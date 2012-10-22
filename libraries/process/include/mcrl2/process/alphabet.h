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
#include "mcrl2/process/find.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/detail/alphabet_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace process {

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
  std::map<process_identifier, equation_iterator> equation_map;

  // used for searching equations
  std::map<process_identifier, std::map<process_expression, process_identifier> > equation_index;

  // used for generating process identifiers
  data::set_identifier_generator generator;

  // Searches for an equation corresponding with the given id and right hand side. If it does
  // not yet exist, a new equation is created.
  process_identifier find_equation(const process_identifier& id, const process_expression& rhs)
  {
    std::map<process_expression, process_identifier>& index = equation_index[id];
    const process_equation& eqn = *equation_map[id];

    std::map<process_expression, process_identifier>::iterator i = index.find(rhs);
    if (i == index.end()) // the equation does not yet exist
    {
      // create new equation
      std::string prefix = std::string(id.name()) + "_";
      core::identifier_string X = generator(prefix);
      process_identifier new_id(X, id.sorts());
      process_equation new_eqn(new_id, eqn.formal_parameters(), rhs);

      // update data structures
      procspec.equations().push_back(new_eqn);
      equation_iterator j = --procspec.equations().end();
      equation_map[new_id] = j;
      equation_index[id][rhs] = new_id;

      // return result
      return new_id;
    }
    else
    {
      return i->second;
    }
  }

  // returns the body of the process with the given identifier
  process_expression process_body(const process_identifier& id) const
  {
    std::map<process_identifier, equation_iterator>::const_iterator i = equation_map.find(id);
    assert (i != equation_map.end());
    const process_equation& eqn = *i->second;
    return eqn.expression();
  }

  alphabet_parameters(process_specification& p)
    : procspec(p)
  {
    atermpp::vector<process_equation>& equations = p.equations();
    for (atermpp::vector<process_equation>::iterator i = equations.begin(); i != equations.end(); ++i)
    {
      equation_map[i->identifier()] = i;
      equation_index[i->identifier()][i->expression()] = i->identifier();
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

  // if a new equation Q = p was introduced for the equation P = p, then generated_equations[P] = Q
  atermpp::map<process_identifier, process_identifier> generated_equations;

  // stack with intermediate results
  typedef std::pair<process_expression, multi_action_name_set> alphabet_result;
  atermpp::vector<alphabet_result> result_stack;

  // Constructor
  default_push_traverser(const multi_action_name_set& A_, std::set<process_identifier>& W_, alphabet_parameters& parameters_)
    : A(A_), W(W_), parameters(parameters_)
  {}

  void print(const alphabet_result& r) const
  {
    mCRL2log(log::debug) << "p = " << process::pp(r.first) << " A = " << lps::pp(r.second) << std::endl;
  }

  void print_expression(const process_expression& x)
  {
    mCRL2log(log::debug) << "entering " << process::pp(x) << " A = " << lps::pp(A) << std::endl;
  }

  // prints the top n elements of the stack
  void print_stack(std::size_t n, const std::string& msg = "") const
  {
    mCRL2log(log::debug) << "--- top of stack " << n << " --- " << msg << std::endl;
    n = (std::min)(n, result_stack.size());
    for (atermpp::vector<alphabet_result>::const_reverse_iterator i = result_stack.rbegin(); i != result_stack.rbegin() + n; ++i)
    {
      print(*i);
    }
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

  std::string print_W()
  {
    std::ostringstream out;
    out << "{ ";
    for (std::set<process_identifier>::const_iterator i = W.begin(); i != W.end(); ++i)
    {
      if (i != W.begin())
      {
        out << ", ";
      }
      out << process::pp(i->name());
    }
    out << " }";
    return out.str();
  }

  // delta
  void leave(const process::delta& x)
  {
    push(x, multi_action_name_set());
  }

  // P(e1, ..., en)
  void operator()(const process::process_instance& x)
  {
    print_expression(x);
    process_expression p = parameters.process_body(x.identifier());
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      derived()(p);    // now <p', A'_p> is on the stack
      // process_expression p1 = top().first;
      // process_identifier id = parameters.find_equation(x.identifier(), p1);
      // generated_equations[x.identifier()] = id;
      // process_instance Q(id, x.actual_parameters());
      // top().first = Q; // now <Q, A'_p> is on the stack
      W.erase(x.identifier());
    }
    else
    {
      process_identifier id = generated_equations[x.identifier()];
      process_instance Q(id, x.actual_parameters());
      push(Q, multi_action_name_set());
    }
    print_stack(1, " process_instance[default]");
  }

  // P(d1 = e1, ..., dn = en)
  void operator()(const process::process_instance_assignment& x)
  {
    process_expression p = parameters.process_body(x.identifier());
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      derived()(p);    // now <p', A'_p> is on the stack
      process_expression p1 = top().first;
      process_identifier id = parameters.find_equation(x.identifier(), p1);
      process_instance_assignment Q(id, x.assignments());
      top().first = Q; // now <Q, A'_p> is on the stack
      W.erase(x.identifier());
    }
    else
    {
      process_identifier id = generated_equations[x.identifier()];
      process_instance_assignment Q(id, x.assignments());
      push(Q, multi_action_name_set());
    }
    print_stack(1, " process_instance_assignment[default]");
  }

  // p1 + p2
  void leave(const process::choice& x)
  {
    join(x);
    print_stack(1, " choice[default]");
  }

  // p1 . p2
  void leave(const process::seq& x)
  {
    join(x);
    print_stack(1, " seq[default]");
  }

  // c -> p
  void leave(const process::if_then& x)
  {
    top().first = derived().f(x, A);
    print_stack(1, " if_then[default]");
  }

  // c -> p1 <> p2
  void leave(const process::if_then_else& x)
  {
    join(x);
    print_stack(1, " if_then_else[default]");
  }

  // sum d:D . p
  void leave(const process::sum& x)
  {
    top().first = derived().f(x, A);
    print_stack(1, " sum[default]");
  }

  // p @ t
  void leave(const process::at& x)
  {
    top().first = derived().f(x, A);
    print_stack(1, " at[default]");
  }

  // p << q
  void operator()(const process::bounded_init& x)
  {
    derived()(x.left());
    print_stack(1, " bounded_init[default]");
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
  using super::print_stack;

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
    print_stack(1, " action[allow]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    multi_action_name tau;
    multi_action_name_set A1 = set_intersection(A, tau);
    push(make_allow(A, x), A1);
    print_stack(1, " tau[allow]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
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
    print_stack(1, " merge[allow]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
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
    print_stack(1, " left_merge[allow]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
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
    print_stack(1, " sync[allow]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
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
    print_stack(1, " rename[allow]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
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
    print_stack(1, " block[allow]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
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
    print_stack(1, " hide[allow]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    if (A_is_Act)
    {
      // TODO
      throw std::runtime_error("not implemented yet");
    }
    else
    {
      process_expression p = x.operand();
      alphabet_result r = push_allow(p, set_union(A, apply_communication_inverse(C, A)), W, true, parameters);
      const process_expression& p1 = r.first;
      const multi_action_name_set& Ap1 = r.second;
      multi_action_name_set A1 = set_intersection(A, apply_communication(C, Ap1));
      push(make_allow(remove_tau(A), comm(C, p1)), A1);
    }
    print_stack(1, " comm[allow]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
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
    print_stack(1, " allow[allow]");
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
  using super::print_stack;

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
    multi_action_name a = name(x);
    multi_action_name_set A1 = set_intersection(A, a);
    push(make_allow(A1, x), A1);
    print_stack(1, " action[sub]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    multi_action_name_set A1;
    A1.insert(multi_action_name()); // A1 = { tau }
    push(tau(), A1);
    print_stack(1, " tau[sub]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
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
    print_stack(1, " merge[sub]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
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
    print_stack(1, " left_merge[sub]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
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
    print_stack(1, " sync[sub]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    process_expression p = x.operand();
    derived()(p); // now <p', A'_p> is on the stack
    const process_expression& p1 = top().first;
    const multi_action_name_set& Ap1 = top().second;
    top().first = rename(R, p1);
    top().second = apply_rename(R, Ap1);
    print_stack(1, " rename[sub]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    multi_action_name_set A1 = apply_block(B, A);
    push(push_allow(x.operand(), A1, W, false, parameters));
    print_stack(1, " block[sub]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    process_expression p = x.operand();
    alphabet_result r = push_allow(p, A, W, true, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    core::identifier_string_list I = x.hide_set();
    multi_action_name_set A1 = set_intersection(A, apply_hide(I, Ap1));
    push(make_allow(remove_tau(A1), hide(I, p1)), A1);
    print_stack(1, " hide[sub]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    process_expression p = x.operand();
    alphabet_result r = push_sub(p, set_union(A, apply_communication_inverse(C, A)), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = set_intersection(A, apply_communication(C, Ap1));
    multi_action_name_set A2 = subset_intersection(A, apply_communication(C, Ap1));
    push(make_allow(remove_tau(A1), comm(C, p1)), A2);
    print_stack(1, " comm[sub]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    multi_action_name_set V = make_name_set(x.allow_set());
    process_expression p = x.operand();
    multi_action_name_set A1 = subset_intersection(A, V);
    push(push_sub(p, A1, W, parameters));
    print_stack(1, " allow[sub]");
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
  using super::print_stack;

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
    multi_action_name a = name(x);
    multi_action_name_set A1 = set_intersection(A, a);
    push(make_block(A, x), A1);
    print_stack(1, " action[block]");
  }

  // tau
  void operator()(const process::tau& x)
  {
    multi_action_name tau;
    multi_action_name_set A1;
    A1.insert(tau);
    push(x, A1);
    print_stack(1, " tau[block]");
  }

  // p1 || p2
  void operator()(const process::merge& x)
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
    push(merge(p1, q1), A1);
    print_stack(1, " merge[block]");
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
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
    push(left_merge(p1, q1), A1);
    print_stack(1, " left_merge[block]");
  }

  // p1 | p2
  void operator()(const process::sync& x)
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
    push(sync(p1, q1), A1);
    print_stack(1, " sync[block]");
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    process_expression p = x.operand();
    alphabet_result r = push_sub(p, apply_rename(R, apply_rename_inverse(R, A)), W, parameters);
    process_expression p1 = r.first;
    multi_action_name_set Ap1 = r.second;
    multi_action_name_set A1 = apply_rename(R, Ap1);
    push(rename(R, p1), A1);
    print_stack(1, " rename[block]");
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    multi_action_name_set B = make_name_set(x.block_set());
    multi_action_name_set A1 = set_union(A, B);
    process_expression p = x.operand();
    push(push_block(p, A1, W, parameters));
    print_stack(1, " block[block]");
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    core::identifier_string_list I = x.hide_set();
    process_expression p = x.operand();
    alphabet_result r = push_block(p, set_difference(A, make_name_set(I)), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = apply_hide(I, Ap1);
    push(hide(I, p1), A1);
    print_stack(1, " hide[block]");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    process_expression p = x.operand();
    multi_action_name_set A2 = apply_communication_bar(C, A);
    alphabet_result r = push_block(p, set_difference(A, A2), W, parameters);
    const process_expression& p1 = r.first;
    const multi_action_name_set& Ap1 = r.second;
    multi_action_name_set A1 = apply_block(A, apply_communication(C, Ap1));
    communication_expression_list C1; // = ???
    push(make_block(A2, comm(C1, p1)), A1);
    print_stack(1, " comm[block]");
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    multi_action_name_set V = make_name_set(x.allow_set());
    process_expression p = x.operand();
    multi_action_name_set A1 = subset_intersection(A, V);
    push(push_sub(p, A1, W, parameters));
    print_stack(1, " allow[block]");
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
  procspec.init() = r.first;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
