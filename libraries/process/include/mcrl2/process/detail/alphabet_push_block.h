// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_push_block.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H

#include "mcrl2/process/detail/alphabet_push_allow.h"



namespace mcrl2::process {

struct push_block_cache
{
  std::map<process_instance, std::vector<std::pair<std::set<core::identifier_string>, process_instance> > > equation_cache;

  // Caches the alphabet of pCRL equations
  std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache;

  explicit push_block_cache(std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache_)
    : pcrl_equation_cache(pcrl_equation_cache_)
  {}
};

namespace block_operations {

template <typename Container>
std::set<core::identifier_string> set_union(const std::set<core::identifier_string>& S1, const Container& S2)
{
  std::set<core::identifier_string> result = S1;
  result.insert(S2.begin(), S2.end());
  return result;
}

template <typename Container>
std::set<core::identifier_string> set_difference(const std::set<core::identifier_string>& S1, const Container& S2)
{
  std::set<core::identifier_string> result = S1;
  for (typename Container::const_iterator i = S2.begin(); i != S2.end(); ++i)
  {
    result.erase(*i);
  }
  return result;
}

inline
std::set<core::identifier_string> rename_inverse(const rename_expression_list& R, const std::set<core::identifier_string>& B)
{
  alphabet_operations::rename_inverse_map Rinverse = alphabet_operations::rename_inverse(R);
  std::set<core::identifier_string> result;
  for (const core::identifier_string& i: B)
  {
    auto j = Rinverse.find(i);
    if (j != Rinverse.end())
    {
      std::vector<core::identifier_string> s = Rinverse[i];
      result.insert(s.begin(), s.end());
    }
    else
    {
      result.insert(i);
    }
  }
  return result;
}

} // namespace block_operations

namespace detail {

struct push_block_printer
{
  const std::set<core::identifier_string>& B;

  explicit push_block_printer(const std::set<core::identifier_string>& B_)
    : B(B_)
  {}

  std::string print(const std::set<core::identifier_string>& x) const
  {
    return core::detail::print_set(x);
  }

  template <typename T>
  std::string print(const T& x) const
  {
    return process::pp(x);
  }

  std::string print(const process::allow& x, const allow_set& A1) const
  {
    std::ostringstream out;
    out << "push_block(" << print(B) << ", " << print(x) << ") = "
        << "push_allow(" << A1 << ", " << print(x.operand()) << ")" << std::endl;
    return out.str();
  }

  std::string print(const process::comm& x, const process_expression& result) const
  {
    std::ostringstream out;
    out << "push_block(" << print(B) << ", " << print(x) << ") = "
        << print(result) << std::endl;
    return out.str();
  }

  std::string print(const process::block& x, const std::set<core::identifier_string>& B1) const
  {
    std::ostringstream out;
    out << "push_block(" << print(B) << ", " << print(x) << ") = "
        << "push_block(" << print(B1) << ", " << print(x.operand()) << ")" << std::endl;
    return out.str();
  }

  std::string print(const process::hide& x, const std::set<core::identifier_string>& B1) const
  {
    std::ostringstream out;
    out << "push_block(" << print(B) << ", " << print(x) << ") = "
        << "hide(" << print(x.hide_set()) << ", push_block(" << print(B1) << ", " << print(x.operand()) << "))" << std::endl;
    return out.str();
  }

  std::string print(const process::rename& x, const std::set<core::identifier_string>& B1) const
  {
    std::ostringstream out;
    const auto& R = x.rename_set();
    out << "push_block(" << print(B) << ", rename(" << print(R) << ", " << print(x.operand()) << ")) = "
        << "rename(" << print(R) << ", push_block(" << print(B1) << ", " << print(x.operand()) << "))" << std::endl;
    return out.str();
  }
};

inline
std::string print_B(const std::set<core::identifier_string>& B)
{
  std::ostringstream out;
  out << "{";
  for (auto i = B.begin(); i != B.end(); ++i)
  {
    if (i != B.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << "}";
  return out.str();
}

process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, std::vector<process_equation>& equations, push_block_cache& W, data::set_identifier_generator& id_generator);

template <typename Derived>
struct push_block_builder: public process_expression_builder<Derived>
{
  typedef process_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  // used for computing the alphabet
  std::vector<process_equation>& equations;
  push_block_cache& W;

  // the parameter B
  const std::set<core::identifier_string>& B;

  // used for generating process identifiers
  data::set_identifier_generator& id_generator;

  push_block_builder(std::vector<process_equation>& equations_, push_block_cache& W_, const std::set<core::identifier_string>& B_, data::set_identifier_generator& id_generator_)
    : equations(equations_), W(W_), B(B_), id_generator(id_generator_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <class T>
  void apply(T& result, const process::action& x)
  {
    using utilities::detail::contains;
    if (contains(B, x.label().name()))
    {
      result = delta();
    }
    else
    {
      result = x;
    }
  }

  template <class T>
  void apply(T& result, const process::process_instance& x)
  {
    // Let x = P(e)
    // The corresponding equation is P(d) = p
    auto i = W.equation_cache.find(x);
    if (i != W.equation_cache.end())
    {
      const std::vector<std::pair<std::set<core::identifier_string>, process_instance> >& v = i->second;
      for (const auto& j: v)
      {
        if (B == j.first)
        {
          result = j.second;
          return;
        }
      }
    }

    const process_equation& eqn = find_equation(equations, x.identifier());
    const data::variable_list& d = eqn.formal_parameters();
    core::identifier_string name = id_generator(x.identifier().name());
    process_identifier P1(name, x.identifier().variables());
    const process_expression& p = eqn.expression();

    // Add (P(e), B, P1(e)) to W
    W.equation_cache[x].push_back(std::make_pair(B, process_instance(P1, x.actual_parameters())));

    process_expression p1 = push_block(B, p, equations, W, id_generator);

    // create a new equation P1(d) = p1
    process_equation eqn1(P1, d, p1);
    equations.push_back(eqn1);

    result = process_instance(P1, x.actual_parameters());
  }

  template <class T>
  void apply(T& result, const process::process_instance_assignment& x)
  {
    process_instance x1 = expand_assignments(x, equations);
    derived().apply(result, x1);
  }

  template <class T>
  void apply(T& result, const process::block& x)
  {
    std::set<core::identifier_string> B1 = block_operations::set_union(B, x.block_set());
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    result = push_block(B1, x.operand(), equations, W, id_generator);
  }

  template <class T>
  void apply(T& result, const process::hide& x)
  {
    const core::identifier_string_list& I = x.hide_set();
    std::set<core::identifier_string> B1 = block_operations::set_difference(B, I);
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    make_hide(result, I, push_block(B1, x.operand(), equations, W, id_generator));
  }

  template <class T>
  void apply(T& result, const process::rename& x)
  {
    const rename_expression_list& R = x.rename_set();
    std::set<core::identifier_string> B1 = block_operations::rename_inverse(R, B);
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    process::make_rename(result, R, push_block(B1, x.operand(), equations, W, id_generator));
  }

  bool restrict_(const core::identifier_string& b, const std::set<core::identifier_string>& B, const communication_expression_list& C) const
  {
    using utilities::detail::contains;
    for (const communication_expression& i: C)
    {
      core::identifier_string_list gamma = i.action_name().names();
      const core::identifier_string& c = i.name();
      if (contains(gamma, b) && !contains(B, c))
      {
        return true;
      }
    }
    return false;
  }

  std::set<core::identifier_string> restrict_block(const std::set<core::identifier_string>& B, const communication_expression_list& C) const
  {
    std::set<core::identifier_string> result;
    for (auto i = B.begin(); i != B.end(); ++i)
    {
      if (!restrict_(*i, B, C))
      {
        result.insert(*i);
      }
    }
    return result;
  }

  template <class T>
  void apply(T& result, const process::comm& x)
  {
    std::set<core::identifier_string> B1 = restrict_block(B, x.comm_set());
    process_expression y = push_block(B1, x.operand(), equations, W, id_generator);
    result = make_block(core::identifier_string_list(B.begin(), B.end()), make_comm(x.comm_set(), y));
    mCRL2log(log::debug) << push_block_printer(B).print(x, result);
  }

  template <class T>
  void apply(T& result, const process::allow& x)
  {
    allow_set A(alphabet_operations::make_name_set(x.allow_set()));
    core::identifier_string_list B1(B.begin(), B.end());
    allow_set A1(alphabet_operations::block(B1, A.A));
    detail::push_allow_cache W_allow(id_generator, W.pcrl_equation_cache);
    detail::push_allow_node node = detail::push_allow(x.operand(), A1, equations, W_allow, true);
    mCRL2log(log::debug) << push_block_printer(B).print(x, A1);
    result = node.expression;
  }

  // This function is needed because the linearization algorithm does not handle the case 'delta | delta'.
  template <class T>
  void apply(T& result, const process::sync& x)
  {
    process_expression left;
    derived().apply(left, x.left());
    process_expression right;
    derived().apply(right, x.right());
    result = make_sync(left, right);
  }
};

template <template <class> class Traverser>
struct apply_push_block_builder: public Traverser<apply_push_block_builder<Traverser> >
{
  typedef Traverser<apply_push_block_builder<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  apply_push_block_builder(std::vector<process_equation>& equations, push_block_cache& W, const std::set<core::identifier_string>& B, data::set_identifier_generator& id_generator)
    : super(equations, W, B, id_generator)
  {}
};

inline
process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, std::vector<process_equation>& equations, push_block_cache& W, data::set_identifier_generator& id_generator)
{
  apply_push_block_builder<push_block_builder> f(equations, W, B, id_generator);
  process_expression result;
  f.apply(result, x);
  return result;
}

} // namespace detail

inline
process_expression push_block(const core::identifier_string_list& B,
                              const process_expression& x,
                              std::vector<process_equation>& equations,
                              data::set_identifier_generator& id_generator,
                              std::map<process_identifier, multi_action_name_set>& pcrl_equation_cache
                             )
{
  std::set<core::identifier_string> B1(B.begin(), B.end());
  push_block_cache W(pcrl_equation_cache);
  return detail::push_block(B1, x, equations, W, id_generator);
}

} // namespace mcrl2::process



#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
