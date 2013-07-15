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
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/process/detail/allow_set.h"
#include "mcrl2/process/detail/alphabet_traverser.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/replace.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct push_allow_node: public alphabet_node
{
  push_allow_node()
  {}

  push_allow_node(const multi_action_name_set& alphabet, const process_expression& expression = process_expression())
    : alphabet_node(alphabet), m_expression(expression)
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
        // N.B. This is a tricky case. We can't return allow({tau}, m_expression),
        // as this is not allowed in mCRL2. We can take an arbitrary element of
        // A instead.
        multi_action_name_set A1;
        A1.insert(*A.A.begin());
        m_expression = detail::make_allow(A1, m_expression);
      }
      else
      {
        m_expression = detail::make_allow(alphabet, m_expression);
      }
    }
  }

  process_expression m_expression;
};

struct wnode
{
  wnode(const allow_set& A_, const process_instance& Q_)
    : A(A_), Q(Q_)
  {}

  wnode(const allow_set& A_, const process_instance& Q_, const multi_action_name_set& alphabet_)
    : A(A_), Q(Q_), alphabet(alphabet_)
  {}

  allow_set A;
  process_instance Q;
  multi_action_name_set alphabet;
};

struct push_allow_map
{
  std::map<process_instance, std::vector<wnode> > data;

  // Returns true if (x, A) is in the map. In that case result and alphabet are being set.
  // Returns false if (x, A) is not in the map. In that case a new entry for (x, A) is created, and result and alphabet are being set.
  bool find(const process_instance& x, const allow_set& A, push_allow_node& node) const
  {
    bool result = false;

    auto i = data.find(x);
    if (i != data.end())
    {
      const std::vector<wnode>& v = i->second;
      for (auto j = v.begin(); j != v.end(); ++j)
      {
        if (A == j->A)
        {
          node.m_expression = j->Q;
          node.alphabet = j->alphabet;
          result = true;
          break;
        }
      }
    }
    return result;
  }

  void insert(const process_expression& x, const allow_set& A, const process_instance& Q)
  {
    data[atermpp::aterm_cast<process_instance>(x)].push_back(wnode(A, Q));
  }

  void set_alphabet(const process_instance& x, const allow_set& A, const multi_action_name_set& alphabet)
  {
    auto i = data.find(x);
    if (i != data.end())
    {
      std::vector<wnode>& v = i->second;
      for (auto j = v.begin(); j != v.end(); ++j)
      {
        if (A == j->A)
        {
          j->alphabet = alphabet;
          return;
        }
      }
    }
    throw mcrl2::runtime_error("push_allow_map: entry not found!");
  }
};

inline
std::ostream& operator<<(std::ostream& out, const push_allow_map& W)
{
  out << "W = {";
  for (auto i = W.data.begin(); i != W.data.end(); ++i)
  {
    if (i != W.data.begin())
    {
      out << ", ";
    }
    const process_instance& P = i->first;
    const std::vector<wnode>& v = i->second;
    for (auto j = v.begin(); j != v.end(); ++j)
    {
      if (j != v.begin())
      {
        out << ", ";
      }
      out << "(P = " << process::pp(P) << ", " << "A = " << j->A << ", Q = " << process::pp(j->Q) << ")";
    }
  }
  out << "}";
  return out;
}

inline
std::ostream& operator<<(std::ostream& out, const push_allow_node& x)
{
  return out << "alphabet = " << lps::pp(x.alphabet) << " expression = " << process::pp(x.m_expression) << std::endl;
}

push_allow_node push_allow(const process_expression& x, const allow_set& A, std::vector<process_equation>& equations, push_allow_map& W, data::set_identifier_generator& id_generator);

template <typename Derived, typename Node = push_allow_node>
struct push_allow_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // used for computing the alphabet
  std::vector<process_equation>& equations;
  push_allow_map& W;

  // the parameter A
  const allow_set& A;

  // used for generating process identifiers
  data::set_identifier_generator& id_generator;

  std::vector<Node> node_stack;

  push_allow_traverser(std::vector<process_equation>& equations_, push_allow_map& W_, const allow_set& A_, data::set_identifier_generator& id_generator_)
    : equations(equations_), W(W_), A(A_), id_generator(id_generator_)
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

  void log_push_result(const process_expression& x, const allow_set& A, const push_allow_map& W, const push_allow_node& result, const std::string& msg = "", const std::string& text = "")
  {
    std::string text1 = text;
    if (!text1.empty())
    {
      text1 = text1 + " = ";
    }
    mCRL2log(log::debug) << msg << "push(" << A << ", " << process::pp(x) << ", " << W << ") = "
      << text1
      << process::pp(result.m_expression) << " with alphabet(" << process::pp(result.m_expression) << ") = " << lps::pp(result.alphabet) << std::endl;
  }

  void log(const process_expression& x, const std::string& text = "")
  {
    log_push_result(x, A, W, top(), "", text);
  }

  bool is_pcrl(const process_expression& x) const
  {
    return !is_merge(x) && !is_left_merge(x) && !is_sync(x) && !is_hide(x) && !is_rename(x) && !is_block(x) && !is_allow(x) && !is_comm(x);
  }

  void leave(const lps::action& x)
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
    // Let x = P(e)
    // The corresponding equation is P(d) = p
    push_allow_node node;
    if (W.find(x, A, node))
    {
      push(node);
      return;
    }

    const process_equation& eqn = find_equation(equations, x.identifier());
    data::variable_list d = eqn.formal_parameters();
    core::identifier_string name = id_generator(x.identifier().name());
#ifndef MCRL2_NEW_PROCESS_IDENTIFIER
    process_identifier P1(name, x.identifier().sorts());
#else
    process_identifier P1(name, x.identifier().variables());
#endif
    const process_expression& p = eqn.expression();

    // Add (P(e), A, P1(e)) to W
    process_instance P1e(P1, x.actual_parameters());
    W.insert(x, A, P1e);

    node = push_allow(p, A, equations, W, id_generator);

    // create a new equation P1(d) = p1
    process_equation eqn1(P1, d, node.m_expression);
    equations.push_back(eqn1);

    node.m_expression = P1e;
    node.apply_allow(A);
    push(node);

    // put the alphabet of P1e in W
    W.set_alphabet(x, A, node.alphabet);

    log(x);
  }

  void leave(const process::process_instance_assignment& x)
  {
    process_instance x1 = expand_assignments(x, equations);
    derived()(x1);
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
    top().m_expression = process::sum(x.bound_variables(), top().m_expression);
    log(x);
  }

  void leave(const process::at& x)
  {
    top().m_expression = process::at(top().m_expression, x.time_stamp());
    log(x);
  }

  void leave(const process::seq& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::seq(left.m_expression, right.m_expression)));
    log(x);
  }

  void leave(const process::if_then& x)
  {
    top().m_expression = process::if_then(x.condition(), top().m_expression);
    log(x);
  }

  void leave(const process::if_then_else& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::if_then_else(x.condition(), left.m_expression, right.m_expression)));
    log(x);
  }

  void leave(const process::bounded_init& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::bounded_init(left.m_expression, right.m_expression)));
    log(x);
  }

  void leave(const process::choice& x)
  {
    Node right = pop();
    Node left = pop();
    push(push_allow_node(set_union(left.alphabet, right.alphabet), process::choice(left.m_expression, right.m_expression)));
    log(x);
  }

  std::string log_hide(const process::hide& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "hide({" << core::pp(x.hide_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void operator()(const process::hide& x)
  {
    core::identifier_string_list I = x.hide_set();
    allow_set A1 = allow_set_operations::hide_inverse(I, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W, id_generator);
    push(push_allow_node(alphabet_operations::hide(I, node.alphabet), process::hide(I, node.m_expression)));
    log(x, log_hide(x, A1));
  }

  std::string log_block(const process::block& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "block({" << core::pp(x.block_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void operator()(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    allow_set A1 = allow_set_operations::block(B, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W, id_generator);
    push(node);
    log(x, log_block(x, A1));
  }

  std::string log_rename(const process::rename& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "rename({" << process::pp(x.rename_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    allow_set A1 = allow_set_operations::rename_inverse(R, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W, id_generator);
    push(push_allow_node(alphabet_operations::rename(R, node.alphabet), process::rename(R, node.m_expression)));
    log(x, log_rename(x, A1));
  }

  std::string log_comm(const process::comm& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "comm({" << process::pp(x.comm_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    allow_set A1 = allow_set_operations::comm_inverse(C, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W, id_generator);
    communication_expression_list C1 = filter_comm_set(x.comm_set(), node.alphabet);
    push(push_allow_node(alphabet_operations::comm(C1, node.alphabet), detail::make_comm(C1, node.m_expression)));
    top().apply_allow(A);
    log(x, log_comm(x, A1));
  }

  std::string log_allow(const process::allow& x, const allow_set& A1)
  {
    std::ostringstream out;
    out << "allow({" << process::pp(x.allow_set()) << "}, push(" << A1 << ", " << process::pp(x.operand()) << "))";
    return out.str();
  }

  void operator()(const process::allow& x)
  {
    action_name_multiset_list V = x.allow_set();
    allow_set A1 = allow_set_operations::allow(V, A);
    push_allow_node node = push_allow(x.operand(), A1, equations, W, id_generator);
    push(node);
    log(x, log_allow(x, A1));
  }

  std::string log_merge(const process::merge& x, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "merge(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << "))";
    return out.str();
  }

  void operator()(const process::merge& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W, id_generator);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W, id_generator);
    push(push_allow_node(alphabet_operations::merge(p1.alphabet, q1.alphabet), detail::make_merge(p1.m_expression, q1.m_expression)));
    top().apply_allow(A);
    log(x, log_merge(x, A_sub, A_arrow));
  }

  std::string log_left_merge(const process::left_merge& x, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "left_merge(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << "))";
    return out.str();
  }

  void operator()(const process::left_merge& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W, id_generator);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W, id_generator);
    push(push_allow_node(alphabet_operations::left_merge(p1.alphabet, q1.alphabet), detail::make_left_merge(p1.m_expression, q1.m_expression)));
    top().apply_allow(A);
    log(x, log_left_merge(x, A_sub, A_arrow));
  }

  std::string log_sync(const process::sync& x, const allow_set& A_sub, const allow_set& A_arrow)
  {
    std::ostringstream out;
    out << "sync(push(" << A_sub << ", " << process::pp(x.left()) << "), push(" << A_arrow << ", " << process::pp(x.right()) << "))";
    return out.str();
  }

  void operator()(const process::sync& x)
  {
    allow_set A_sub = allow_set_operations::subsets(A);
    push_allow_node p1 = push_allow(x.left(), A_sub, equations, W, id_generator);
    allow_set A_arrow = allow_set_operations::left_arrow(A, p1.alphabet);
    push_allow_node q1 = push_allow(x.right(), A_arrow, equations, W, id_generator);
    push(push_allow_node(alphabet_operations::sync(p1.alphabet, q1.alphabet), detail::make_sync(p1.m_expression, q1.m_expression)));
    top().apply_allow(A);
    log(x, log_sync(x, A_sub, A_arrow));
  }
};

template <template <class, class> class Traverser, typename Node = push_allow_node>
struct apply_push_allow_traverser: public Traverser<apply_push_allow_traverser<Traverser, Node>, Node>
{
  typedef Traverser<apply_push_allow_traverser<Traverser, Node>, Node> super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_allow_traverser(std::vector<process_equation>& equations, push_allow_map& W, const allow_set& A, data::set_identifier_generator& id_generator)
    : super(equations, W, A, id_generator)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
push_allow_node push_allow(const process_expression& x, const allow_set& A, std::vector<process_equation>& equations, push_allow_map& W, data::set_identifier_generator& id_generator)
{
  apply_push_allow_traverser<push_allow_traverser> f(equations, W, A, id_generator);
  f(x);
  return f.node_stack.back();
}

inline
push_allow_node push_allow(const process_expression& x, const allow_set& A, std::vector<process_equation>& equations, data::set_identifier_generator& id_generator)
{
  push_allow_map W;
  return push_allow(x, A, equations, W, id_generator);
}

} // namespace detail

inline
process_expression push_allow(const process_expression& x, const action_name_multiset_list& V, std::vector<process_equation>& equations, data::set_identifier_generator& id_generator)
{
  allow_set A(make_name_set(V));
  detail::push_allow_node node = detail::push_allow(x, A, equations, id_generator);
  return node.m_expression;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_ALLOW_H
