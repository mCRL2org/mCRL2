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

#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>
#include "mcrl2/process/detail/alphabet_traverser.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/replace.h"
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

process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, const atermpp::vector<process_equation>& equations);

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

  // the parameter B
  const std::set<core::identifier_string>& B;

  // used for computing the alphabet
  const atermpp::vector<process_equation>& equations;

  push_block_builder(const std::set<core::identifier_string>& B_, const atermpp::vector<process_equation>& equations_)
    : B(B_), equations(equations_)
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
    const process_equation& eqn = find_equation(equations, x.identifier());
    process_expression p = eqn.expression();
    data::mutable_map_substitution<> sigma;
    data::variable_list d = eqn.formal_parameters();
    data::data_expression_list e = x.actual_parameters();
    data::variable_list::iterator di = d.begin();
    data::data_expression_list::iterator ei = e.begin();
    for (; di != d.end(); ++di, ++ei)
    {
      sigma[*di] = *ei;
    }
    p = process::replace_free_variables(p, sigma);
    return derived()(p);
  }

  process::process_expression operator()(const process::process_instance_assignment& x)
  {
    const process_equation& eqn = find_equation(equations, x.identifier());
    process_expression p = eqn.expression();
    data::mutable_map_substitution<> sigma;
    data::assignment_list a = x.assignments();
    for (data::assignment_list::iterator i = a.begin(); i != a.end(); ++i)
    {
      sigma[i->lhs()] = i->rhs();
    }
    p = process::replace_free_variables(p, sigma);
    return derived()(p);
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

  process::process_expression operator()(const process::comm& x)
  {
    process::process_expression result = process::comm(x.comm_set(), static_cast<Derived&>(*this)(x.operand()));
    return result;
  }

  process::process_expression operator()(const process::allow& x)
  {
    process::process_expression result = process::allow(x.allow_set(), static_cast<Derived&>(*this)(x.operand()));
    return result;
  }
};

template <template <class> class Traverser>
struct apply_push_block_builder: public Traverser<apply_push_block_builder<Traverser> >
{
  typedef Traverser<apply_push_block_builder<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_block_builder(const std::set<core::identifier_string>& B, const atermpp::vector<process_equation>& equations)
    : super(B, equations)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
process_expression push_block(const std::set<core::identifier_string>& B, const process_expression& x, const atermpp::vector<process_equation>& equations)
{
  apply_push_block_builder<push_block_builder> f(B, equations);
  return f(x);
}

} // namespace detail

inline
process_expression push_block(const core::identifier_string_list& B, const process_expression& x, const atermpp::vector<process_equation>& equations)
{
  std::set<core::identifier_string> B1(B.begin(), B.end());
  return detail::push_block(B1, x, equations);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PUSH_BLOCK_H
