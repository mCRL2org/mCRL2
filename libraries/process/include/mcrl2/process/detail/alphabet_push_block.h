// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_push_block.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H

#include <map>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/process/detail/alphabet_push_allow.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace process {

typedef std::map<process_instance, std::vector<std::pair<std::set<core::identifier_string>, process_instance> > > push_block_map;

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
  process::detail::rename_inverse_map Rinverse = process::detail::rename_inverse(R);
  std::set<core::identifier_string> result;
  for (std::set<core::identifier_string>::const_iterator i = B.begin(); i != B.end(); ++i)
  {
    auto j = Rinverse.find(*i);
    if (j != Rinverse.end())
    {
      std::vector<core::identifier_string> s = Rinverse[*i];
      result.insert(s.begin(), s.end());
    }
    else
    {
      result.insert(*i);
    }
  }
  return result;
}

} // namespace block_operations

namespace detail {

struct push_block_printer
{
  const std::set<core::identifier_string>& B;

  push_block_printer(const std::set<core::identifier_string>& B_)
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
    auto R = x.rename_set();
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
  for (std::set<core::identifier_string>::const_iterator i = B.begin(); i != B.end(); ++i)
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

process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, std::vector<process_equation>& equations, push_block_map& W, data::set_identifier_generator& id_generator);

template <typename Derived>
struct push_block_builder: public process_expression_builder<Derived>
{
  typedef process_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // used for computing the alphabet
  std::vector<process_equation>& equations;
  push_block_map& W;

  // the parameter B
  const std::set<core::identifier_string>& B;

  // used for generating process identifiers
  data::set_identifier_generator& id_generator;

  push_block_builder(std::vector<process_equation>& equations_, push_block_map& W_, const std::set<core::identifier_string>& B_, data::set_identifier_generator& id_generator_)
    : equations(equations_), W(W_), B(B_), id_generator(id_generator_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  process::process_expression operator()(const process::action& x)
  {
    using utilities::detail::contains;
    if (contains(B, x.label().name()))
    {
      return delta();
    }
    else
    {
      return x;
    }
  }

  process::process_expression operator()(const process::process_instance& x)
  {
    // Let x = P(e)
    // The corresponding equation is P(d) = p
    auto i = W.find(x);
    if (i != W.end())
    {
      const std::vector<std::pair<std::set<core::identifier_string>, process_instance> >& v = i->second;
      for (auto j = v.begin(); j != v.end(); ++j)
      {
        if (B == j->first)
        {
          return j->second;
        }
      }
    }

    const process_equation& eqn = find_equation(equations, x.identifier());
    data::variable_list d = eqn.formal_parameters();
    core::identifier_string name = id_generator(x.identifier().name());
    process_identifier P1(name, x.identifier().variables());
    const process_expression& p = eqn.expression();

    // Add (P(e), B, P1(e)) to W
    W[x].push_back(std::make_pair(B, process_instance(P1, x.actual_parameters())));

    process_expression p1 = push_block(B, p, equations, W, id_generator);

    // create a new equation P1(d) = p1
    process_equation eqn1(P1, d, p1);
    equations.push_back(eqn1);

    process_instance result(P1, x.actual_parameters());
    return result;
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    process_instance x1 = expand_assignments(x, equations);
    return derived()(x1);
  }

  process::process_expression operator()(const process::block& x)
  {
    std::set<core::identifier_string> B1 = block_operations::set_union(B, x.block_set());
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    return push_block(B1, x.operand(), equations, W, id_generator);
  }

  process::process_expression operator()(const process::hide& x)
  {
    core::identifier_string_list I = x.hide_set();
    std::set<core::identifier_string> B1 = block_operations::set_difference(B, I);
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    return detail::make_hide(I, push_block(B1, x.operand(), equations, W, id_generator));
  }

  process::process_expression operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    std::set<core::identifier_string> B1 = block_operations::rename_inverse(R, B);
    mCRL2log(log::debug) << push_block_printer(B).print(x, B1);
    return process::rename(R, push_block(B1, x.operand(), equations, W, id_generator));
  }

  bool restrict(const core::identifier_string& b, const std::set<core::identifier_string>& B, const communication_expression_list& C) const
  {
    using utilities::detail::contains;
    for (auto i = C.begin(); i != C.end(); ++i)
    {
      core::identifier_string_list gamma = i->action_name().names();
      core::identifier_string c = i->name();
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
      if (!restrict(*i, B, C))
      {
        result.insert(*i);
      }
    }
    return result;
  }

  process::process_expression operator()(const process::comm& x)
  {
    std::set<core::identifier_string> B1 = restrict_block(B, x.comm_set());
    process_expression y = push_block(B1, x.operand(), equations, W, id_generator);
    process_expression result = detail::make_block(core::identifier_string_list(B.begin(), B.end()), detail::make_comm(x.comm_set(), y));
    mCRL2log(log::debug) << push_block_printer(B).print(x, result);
    return result;
  }

  process::process_expression operator()(const process::allow& x)
  {
    allow_set A(make_name_set(x.allow_set()));
    core::identifier_string_list B1(B.begin(), B.end());
    allow_set A1(alphabet_operations::block(B1, A.A));
    detail::push_allow_node node = detail::push_allow(x.operand(), A1, equations, id_generator);
    mCRL2log(log::debug) << push_block_printer(B).print(x, A1);
    return node.m_expression;
  }

  // This function is needed because the linearization algorithm does not handle the case 'delta | delta'.
  process::process_expression operator()(const process::sync& x)
  {
    process_expression left = derived()(x.left());
    process_expression right = derived()(x.right());
    return detail::make_sync(left, right);
  }
};

template <template <class> class Traverser>
struct apply_push_block_builder: public Traverser<apply_push_block_builder<Traverser> >
{
  typedef Traverser<apply_push_block_builder<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_block_builder(std::vector<process_equation>& equations, push_block_map& W, const std::set<core::identifier_string>& B, data::set_identifier_generator& id_generator)
    : super(equations, W, B, id_generator)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, std::vector<process_equation>& equations, push_block_map& W, data::set_identifier_generator& id_generator)
{
  apply_push_block_builder<push_block_builder> f(equations, W, B, id_generator);
  return f(x);
}

} // namespace detail

inline
process_expression push_block(const core::identifier_string_list& B, const process_expression& x, std::vector<process_equation>& equations, data::set_identifier_generator& id_generator)
{
  std::set<core::identifier_string> B1(B.begin(), B.end());
  push_block_map W;
  return detail::push_block(B1, x, equations, W, id_generator);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
