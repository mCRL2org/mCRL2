// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/anonimyze.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ANONIMYZE_H
#define MCRL2_PROCESS_ANONIMYZE_H

#include "mcrl2/data/anonimyze.h"
#include "mcrl2/process/builder.h"

namespace mcrl2 {

namespace process {

namespace detail {

template <typename Derived>
struct anonimyze_builder: public add_sort_expressions<data::detail::anonimyze_builder, Derived>
{
  typedef add_sort_expressions<data::detail::anonimyze_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::add_name;
  using super::derived;

  std::map<core::identifier_string, core::identifier_string> action_label_name_substitution;
  std::map<core::identifier_string, core::identifier_string> process_identifier_name_substitution;

  void update(process_specification& x)
  {
    super::update(x.data());
    super::update(x);
  }

  void enter(const action_label& x)
  {
    add_name(x.name(), action_label_name_substitution, "a");
  }

  action_label apply(const action_label& x)
  {
    derived().enter(x);
    action_label result(action_label_name_substitution[x.name()], derived().apply(x.sorts()));
    derived().leave(x);
    return result;
  }

  action_name_multiset apply(const action_name_multiset& x)
  {
    const auto& names = x.names();
    return action_name_multiset(core::identifier_string_list(names.begin(), names.end(), [&](const core::identifier_string& y) { return action_label_name_substitution[y]; }));
  }

  allow apply(const allow& x)
  {
    derived().enter(x);
    allow result(derived().apply(x.allow_set()), derived().apply(x.operand()));
    derived().leave(x);
    return result;
  }

  hide apply(const hide& x)
  {
    derived().enter(x);
    const auto& H = x.hide_set();
    core::identifier_string_list H1(H.begin(), H.end(), [&](const core::identifier_string& y) { return action_label_name_substitution[y]; });
    hide result(H1, derived().apply(x.operand()));
    derived().leave(x);
    return result;
  }

  block apply(const block& x)
  {
    derived().enter(x);
    const auto& B = x.block_set();
    core::identifier_string_list B1(B.begin(), B.end(), [&](const core::identifier_string& y) { return action_label_name_substitution[y]; });
    block result(B1, derived().apply(x.operand()));
    derived().leave(x);
    return result;
  }

  rename_expression apply(const rename_expression& x)
  {
    return rename_expression(action_label_name_substitution[x.source()], action_label_name_substitution[x.target()]);
  }

  rename apply(const rename& x)
  {
    derived().enter(x);
    rename result(derived().apply(x.rename_set()), derived().apply(x.operand()));
    derived().leave(x);
    return result;
  }

  communication_expression apply(const communication_expression& x)
  {
    return communication_expression(derived().apply(x.action_name()), action_label_name_substitution[x.name()]);
  }

  comm apply(const comm& x)
  {
    derived().enter(x);
    comm result(derived().apply(x.comm_set()), derived().apply(x.operand()));
    derived().leave(x);
    return result;
  }

  void enter(const process_identifier& x)
  {
    add_name(x.name(), process_identifier_name_substitution, "P");
  }

  process_identifier apply(const process_identifier& x)
  {
    derived().enter(x);
    process_identifier result = process_identifier(process_identifier_name_substitution[x.name()], derived().apply(x.variables()));
    derived().leave(x);
    return result;
  }
};

struct anonimyze_builder_instance: public anonimyze_builder<anonimyze_builder_instance>
{
};

} // namespace detail

inline
void anonimyze(process_specification& procspec)
{
  detail::anonimyze_builder_instance f;
  f.update(procspec);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ANONIMYZE_H
