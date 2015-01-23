// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_push_allow.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_ALLOW_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_ALLOW_H

#include <sstream>
#include <utility>
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/process/detail/allow_set.h"
#include "mcrl2/process/detail/alphabet_traverser.h"
#include "mcrl2/process/expand_process_instance_assignments.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/replace.h"
#include "mcrl2/process/utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct push_allow_node: public alphabet_node
{
  push_allow_node()
  {}

  push_allow_node(const multi_action_name_set& alphabet, const process_expression& expression_ = process_expression())
    : alphabet_node(alphabet), expression(expression_)
  {}

  void apply_allow(const allow_set& A)
  {
    std::size_t alphabet_size = alphabet.size();
    alphabet = A.intersect(alphabet);
    bool needs_allow = alphabet.size() < alphabet_size;
    if (needs_allow)
    {
      if (alphabet.size() == 1 && contains_tau(alphabet)) // alphabet == { tau }
      {
        // N.B. This is a tricky case. We can't return allow({tau}, expression),
        // as this is not allowed in mCRL2. We can take an arbitrary element of
        // A instead.
        multi_action_name_set A1;
        A1.insert(*A.A.begin());
        expression = detail::make_allow(A1, expression);
      }
      else
      {
        expression = detail::make_allow(alphabet, expression);
      }
    }
  }

  process_expression expression;
};

struct alphabet_cache
{
  // This attribute denotes the status of the alphabet computation.
  // - unknown: the alphabet computation has not started yet
  // - busy: the alphabet computation is busy
  // - finished: the alphabet computation is finished
  enum alphabet_status { unknown, busy, finished };

  struct alphabet_key
  {
    allow_set A;
    process_instance P;

    bool operator==(const alphabet_key& other) const
    {
      return P == other.P && A == other.A;
    }

    bool operator<(const alphabet_key& other) const
    {
      if (P != other.P)
      {
        return P < other.P;
      }
      return A < other.A;
    }

    alphabet_key(const allow_set& A_, const process_instance& P_)
     : A(A_), P(P_)
    {}
  };

  // Records the alphabet corresponding to an allow set A and a process instance Q.
  // Each alphabet value has a corresponding equation P = push_allow(A, q), where
  // q is the right hand side of the equation corresponding to Q.
  struct alphabet_value
  {
    multi_action_name_set alphabet;
    alphabet_status status;
    process_instance P;

    alphabet_value()
    {}

    alphabet_value(const multi_action_name_set& alphabet_, alphabet_status status_, const process_instance& P_)
     : alphabet(alphabet_), status(status_), P(P_)
    {}
  };

  // An identifier generator that is used for generating new equations
  data::set_identifier_generator& id_generator;

  // The cache of alphabet values
  std::map<alphabet_key, alphabet_value> alphabet_map;

  // The pairs (A, P) for which an equation needs to be generated
  std::set<alphabet_key> unfinished;

  // The push_allow algorithm maintains a set of dependent nodes. It is used
  // to invalidate alphabet values in the cache.
  std::set<alphabet_key> dependent_nodes;

  alphabet_cache(data::set_identifier_generator& id_generator_)
    : id_generator(id_generator_)
  {}

  std::string print_status(alphabet_status status) const
  {
    switch(status)
    {
      case unknown: return "unknown";
      case busy: return "busy";
      case finished: return "finished";
    }
    throw mcrl2::runtime_error("unknown status!");
  }

  // Returns a reference to the alphabet value corresponding to (A, P).
  // If such value does is not yet present in the map, it is inserted.
  alphabet_value& alphabet(const allow_set& A, const process_instance& P)
  {
    alphabet_key key(A, P);
    auto i = alphabet_map.find(key);
    if (i == alphabet_map.end())
    {
      core::identifier_string name = id_generator(P.identifier().name());
      process_identifier P1(name, P.identifier().variables());
      process_instance P1e(P1, P.actual_parameters());
      multi_action_name_set empty_set;
      alphabet_value value(empty_set, unknown, P1e);
      auto p = alphabet_map.insert(std::make_pair(key, value));
      return p.first->second;
    }
    return i->second;
  }

  // Add (A, x) to the set of unfinished nodes
  void set_unfinished(const allow_set& A, const process_instance& x)
  {
    unfinished.insert(alphabet_key(A, x));
  }

  // For each entry (A, P) -> (alpha, finished, P1) it is checked if
  // intersection(alphabet(P), A) == alpha.
  void check_equations(const std::vector<process_equation>& equations) const
  {
    for (auto i = alphabet_map.begin(); i != alphabet_map.end(); ++i)
    {
      multi_action_name_set alphabet_P = process::alphabet(i->first.P, equations);
      multi_action_name_set a = i->first.A.intersect(alphabet_P);
      std::cout << "check (" << i->first.A << ", " << i->first.P << ")";
      if (a != i->second.alphabet)
      {
        std::cout << "Error: left = " << process::pp(a) << " right = " << process::pp(i->second.alphabet);
      }
      std::cout << std::endl;
    }
  }
};

inline
std::ostream& operator<<(std::ostream& out, const alphabet_cache::alphabet_key& x)
{
  return out << "(" << x.A << ", " << x.P << ")";
}

inline
char print_alphabet_status(alphabet_cache::alphabet_status status)
{
  switch (status)
  {
    case alphabet_cache::unknown:  { return 'u'; }
    case alphabet_cache::busy:     { return 'b'; }
    case alphabet_cache::finished: { return 'f'; }
  }
  return '?';
}

inline
std::ostream& operator<<(std::ostream& out, const alphabet_cache::alphabet_value& x)
{
  return out << "(" << process::pp(x.alphabet) << ", " << print_alphabet_status(x.status) << ", " << x.P << ")";
}

inline
std::ostream& operator<<(std::ostream& out, const alphabet_cache& W)
{
  out << "map: {";
  for (auto i = W.alphabet_map.begin(); i != W.alphabet_map.end(); ++i)
  {
    if (i != W.alphabet_map.begin())
    {
      out << ", ";
    }
    out << i->first << " -> " << i->second;
  }
  out << "}";
  out << " dependent: {";
  for (auto i = W.dependent_nodes.begin(); i != W.dependent_nodes.end(); ++i)
  {
    if (i != W.dependent_nodes.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << "]";
  out << " unfinished: [";
  for (auto i = W.unfinished.begin(); i != W.unfinished.end(); ++i)
  {
    if (i != W.unfinished.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << "]";
  return out;
}

inline
std::ostream& operator<<(std::ostream& out, const push_allow_node& x)
{
  return out << "Node(" << pp(x.alphabet) << ", " << process::pp(x.expression) << ")";
}

push_allow_node push_allow(const process_expression& x, const allow_set& A, std::vector<process_equation>& equations, alphabet_cache& W, bool generate_missing_equations = false);

template <typename Derived, typename Node = push_allow_node>
struct push_allow_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  // used for computing the alphabet
  std::vector<process_equation>& equations;
  alphabet_cache& W;

  // the parameter A
  const allow_set& A;

  std::vector<Node> node_stack;

  push_allow_traverser(std::vector<process_equation>& equations_, alphabet_cache& W_, const allow_set& A_)
    : equations(equations_), W(W_), A(A_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // Push a node to node_stack
  void push(const Node& node)
  {
    node_stack.push_back(node);
  }

  // Pop the top element of node_stack and return it
  Node pop()
  {
    Node result = node_stack.back();
    node_stack.pop_back();
    return result;
  }

  // Return the top element of node_stack
  Node& top()
  {
    return node_stack.back();
  }

  // Return the top element of node_stack
  const Node& top() const
  {
    return node_stack.back();
  }

  void log_push_result(const process_expression& x, const allow_set& A, const alphabet_cache& W, const push_allow_node& result, const std::string& msg = "", const std::string& text = "")
  {
    std::string text1 = text;
    if (!text1.empty())
    {
      text1 = text1 + " = ";
    }
    mCRL2log(log::debug) << msg << "push_allow(" << A << ", " << process::pp(x) << ") = "
      << text1
      << process::pp(result.expression) << " with alphabet(" << process::pp(result.expression) << ") = " << pp(result.alphabet) << std::endl
      << " W = " << W << std::endl;
  }

  void log(const process_expression& x, const std::string& text = "")
  {
    log_push_result(x, A, W, top(), "", text);
  }

  void leave(const process::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    if (A.contains(alpha))
    {
      multi_action_name_set A1;
      A1.insert(alpha);
      push(push_allow_node(A1, x));
    }
    else
    {
      multi_action_name_set A1;
      push(push_allow_node(A1, process::delta()));
    }
    log(x);
  }

  void leave(const process::process_instance& x)
  {
    alphabet_cache::alphabet_key key(A, x);
    alphabet_cache::alphabet_value& alpha = W.alphabet(A, x);
    const multi_action_name_set& alphabet = alpha.alphabet;
    alphabet_cache::alphabet_status status = alpha.status;
    const process_instance& P = alpha.P;

    if (status == alphabet_cache::finished)
    {
      // we already know the result for (A, x)
      push_allow_node node(alphabet, P);
      push(node);
      log(x);
      return;
    }
    else if (status == alphabet_cache::busy)
    {
      // the alphabet of (A, x) is currently being computed; it suffices to return (emptyset, P)
      W.dependent_nodes.insert(key);
      multi_action_name_set empty_set;
      push_allow_node node(empty_set, P);
      push(node);
      log(x);
      return;
    }

    if (status == alphabet_cache::unknown)
    {
      alpha.status = alphabet_cache::busy;

      // N.B. A copy is made, because a call to push_allow may invalidate a reference.
      process_equation eqn = find_equation(equations, x.identifier());
      const data::variable_list& d = eqn.formal_parameters();
      const process_expression& p = eqn.expression();

      // compute the alphabet for (A, x)
      push_allow_node node = push_allow(p, A, equations, W);

      W.dependent_nodes.erase(key);
      if (W.dependent_nodes.empty())
      {
        // create a new equation P(d) = p1
        const process_expression& p1 = node.expression;
        process_equation eqn1(P.identifier(), d, p1);
        equations.push_back(eqn1);

        alpha.alphabet = node.alphabet;
        alpha.status = alphabet_cache::finished;

        node.apply_allow(A);
      }
      else
      {
        alpha.status = alphabet_cache::unknown;
        W.set_unfinished(A, x);
      }

      node.expression = P;
      push(node);
      log(x);
    }
  }

  void leave(const process::process_instance_assignment& x)
  {
    process_instance x1 = expand_assignments(x, equations);
    derived().apply(x1);
  }

  void leave(const process::delta& x)
  {
    push(push_allow_node(process::alphabet(x, equations), x));
    log(x);
  }

  void leave(const process::tau& x)
  {
    push(push_allow_node(process::alphabet(x, equations), x));
    log(x);
  }

  void leave(const process::sum& x)
  {
    top().expression = process::sum(x.variables(), top().expression);
    log(x);
  }

  void leave(const process::at& x)
  {
    top().expression = process::at(top().expression, x.time_stamp());
    log(x);
  }

  void leave(const process::seq& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::seq(left.expression, right.expression)));
    log(x);
  }

  void leave(const process::if_then& x)
  {
    top().expression = process::if_then(x.condition(), top().expression);
    log(x);
  }

  void leave(const process::if_then_else& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::if_then_else(x.condition(), left.expression, right.expression)));
    log(x);
  }

  void leave(const process::bounded_init& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::bounded_init(left.expression, right.expression)));
    log(x);
  }

  void leave(const process::choice& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::choice(left.expression, right.expression)));
    log(x);
  }

  std::string log_hide(const process::hide& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "hide({" << core::pp(x.hide_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void apply(const process::hide& x)
  {
    core::identifier_string_list I = x.hide_set();
    allow_set A1 = allow_set_operations::hide_inverse(I, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W);
    push(push_allow_node(alphabet_operations::hide(I, node.alphabet), process::hide(I, node.expression)));
    log(x, log_hide(x, A1));
  }

  std::string log_block(const process::block& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "block({" << core::pp(x.block_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void apply(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    allow_set A1 = allow_set_operations::block(B, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W);
    push(node);
    log(x, log_block(x, A1));
  }

  std::string log_rename(const process::rename& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "rename({" << process::pp(x.rename_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void apply(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    allow_set A1 = allow_set_operations::rename_inverse(R, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W);
    push(push_allow_node(alphabet_operations::rename(R, node.alphabet), process::rename(R, node.expression)));
    log(x, log_rename(x, A1));
  }

  std::string log_comm(const process::comm& x, const allow_set& A, const allow_set& A1)
  {
    std::ostringstream out;
    out << "allow(" << A << ", comm({" << process::pp(x.comm_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << ")))";
    return out.str();
  }

  void apply(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    allow_set A1 = allow_set_operations::comm_inverse(C, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W);
    communication_expression_list C1 = filter_comm_set(x.comm_set(), node.alphabet);
    push(push_allow_node(alphabet_operations::comm(C1, node.alphabet), detail::make_comm(C1, node.expression)));
    top().apply_allow(A);
    log(x, log_comm(x, A, A1));
  }

  std::string log_allow(const process::allow& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "allow({" << process::pp(x.allow_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void apply(const process::allow& x)
  {
    action_name_multiset_list V = x.allow_set();
    allow_set A1 = allow_set_operations::allow(V, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W);
    push(node);
    log(x, log_allow(x, A1));
  }

  std::string log_merge(const process::merge& x, const allow_set& A, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "allow(" << A << ", merge(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << ")))";
    return out.str();
  }

  void apply(const process::merge& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W);
    push(push_allow_node(alphabet_operations::merge(p1.alphabet, q1.alphabet), detail::make_merge(p1.expression, q1.expression)));
    top().apply_allow(A);
    log(x, log_merge(x, A, A_sub, A_arrow));
  }

  std::string log_left_merge(const process::left_merge& x, const allow_set& A, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "allow(" << A << ", left_merge(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << ")))";
    return out.str();
  }

  void apply(const process::left_merge& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W);
    push(push_allow_node(alphabet_operations::left_merge(p1.alphabet, q1.alphabet), detail::make_left_merge(p1.expression, q1.expression)));
    top().apply_allow(A);
    log(x, log_left_merge(x, A, A_sub, A_arrow));
  }

  std::string log_sync(const process::sync& x, const allow_set& A, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "allow(" << A << ", sync(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << ")))";
    return out.str();
  }

  void apply(const process::sync& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W);
    push(push_allow_node(alphabet_operations::sync(p1.alphabet, q1.alphabet), detail::make_sync(p1.expression, q1.expression)));
    top().apply_allow(A);
    log(x, log_sync(x, A, A_sub, A_arrow));
  }
};

template <template <class, class> class Traverser, typename Node = push_allow_node>
struct apply_push_allow_traverser: public Traverser<apply_push_allow_traverser<Traverser, Node>, Node>
{
  typedef Traverser<apply_push_allow_traverser<Traverser, Node>, Node> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_push_allow_traverser(std::vector<process_equation>& equations, alphabet_cache& W, const allow_set& A)
    : super(equations, W, A)
  {}
};

inline
push_allow_node push_allow(const process_expression& x, const allow_set& A, std::vector<process_equation>& equations, alphabet_cache& W, bool generate_missing_equations)
{
  apply_push_allow_traverser<push_allow_traverser> f(equations, W, A);
  f.apply(x);
  push_allow_node result = f.node_stack.back();

  if (generate_missing_equations)
  {
    while (!W.unfinished.empty())
    {
      detail::alphabet_cache::alphabet_key key = *W.unfinished.begin();
      W.unfinished.erase(W.unfinished.begin());
      detail::alphabet_cache::alphabet_value& value = W.alphabet(key.A, key.P);
      if (value.status != detail::alphabet_cache::finished)
      {
        mCRL2log(log::debug) << "generating unfinished equation for " << key << " -> " << value << std::endl;
        push_allow(key.P, key.A, equations, W);
      }
    }
  }
  // W.check_equations(equations);

  return result;
}

} // namespace detail

inline
process_expression push_allow(const process_expression& x, const action_name_multiset_list& V, std::vector<process_equation>& equations, data::set_identifier_generator& id_generator)
{
  allow_set A(make_name_set(V));
  detail::alphabet_cache W(id_generator);
  detail::push_allow_node node = detail::push_allow(x, A, equations, W, true);
  return node.expression;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_ALLOW_H
