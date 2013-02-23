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

#include "mcrl2/process/detail/alphabet_push_allow.h"
#include "mcrl2/process/utility.h"

namespace mcrl2 {

namespace process {

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
    std::vector<core::identifier_string> s = Rinverse[*i];
    result.insert(s.begin(), s.end());
  }
  return result;
}

} // namespace block_operations

namespace detail {

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
    out << core::pp(*i);
  }
  out << "}";
  return out.str();
}

process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, const std::vector<process_equation>& equations);

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
  const std::vector<process_equation>& equations;
  std::set<process_identifier>& W;

  // the parameter B
  const std::set<core::identifier_string>& B;

  push_block_builder(const std::vector<process_equation>& equations_, std::set<process_identifier>& W_, const std::set<core::identifier_string>& B_)
    : equations(equations_), W(W_), B(B_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  process::process_expression operator()(const lps::action& x)
  {
    if (B.find(x.label().name()) != B.end())
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
    if (W.find(x.identifier()) != W.end())
    {
    	return x;
    }
    else
    {
      W.insert(x.identifier());
      process_expression p = expand_rhs(x, equations);
      process_expression result = derived()(p);
      W.erase(x.identifier());
      return result;
    }
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    if (W.find(x.identifier()) != W.end())
    {
    	return x;
    }
    else
    {
      W.insert(x.identifier());
      process_expression p = expand_rhs(x, equations);
      process_expression result = derived()(p);
      W.erase(x.identifier());
      return result;
    }
  }

  process::process_expression operator()(const process::block& x)
  {
    return push_block(block_operations::set_union(B, x.block_set()), x.operand(), equations);
  }

  process::process_expression operator()(const process::hide& x)
  {
    core::identifier_string_list I = x.hide_set();
    return detail::make_hide(I, push_block(block_operations::set_difference(B, I), x.operand(), equations));
  }

  process::process_expression operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    return process::rename(R, push_block(block_operations::rename_inverse(R, B), x.operand(), equations));
  }

  bool restrict(const core::identifier_string& b, const std::set<core::identifier_string>& B, const communication_expression_list& C) const
  {
    std::cerr << "restrict b = " << core::pp(b) << " B = " << print_B(B) << " C = " << process::pp(C) << std::endl;
    for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
    {
      core::identifier_string_list gamma = i->action_name().names();
      core::identifier_string c = i->name();
      if (std::find(gamma.begin(), gamma.end(), b) != gamma.end() && B.find(c) == B.end())
      {
        return true;
      }
    }
    return false;
  }

  std::set<core::identifier_string> restrict_block(const std::set<core::identifier_string>& B, const communication_expression_list& C) const
  {
    std::set<core::identifier_string> result;
    for (std::set<core::identifier_string>::const_iterator i = B.begin(); i != B.end(); ++i)
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
    process_expression y = push_block(B1, x.operand(), equations);
    return detail::make_block(core::identifier_string_list(B.begin(), B.end()), detail::make_comm(x.comm_set(), y));
  }

  process::process_expression operator()(const process::allow& x)
  {
    allow_set A(make_name_set(x.allow_set()));
    core::identifier_string_list B1(B.begin(), B.end());
    allow_set A1(alphabet_operations::block(B1, A.A));
    detail::push_allow_node node = detail::push_allow(x.operand(), A1, equations);
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

  apply_push_block_builder(const std::vector<process_equation>& equations, std::set<process_identifier>& W, const std::set<core::identifier_string>& B)
    : super(equations, W, B)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  apply_push_block_builder<push_block_builder> f(equations, W, B);
  return f(x);
}

} // namespace detail

inline
process_expression push_block(const core::identifier_string_list& B, const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<core::identifier_string> B1(B.begin(), B.end());
  return detail::push_block(B1, x, equations);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
