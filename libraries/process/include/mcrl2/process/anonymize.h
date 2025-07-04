// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/anonymize.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ANONYMIZE_H
#define MCRL2_PROCESS_ANONYMIZE_H

#include "mcrl2/data/anonymize.h"
#include "mcrl2/process/builder.h"

namespace mcrl2::process
{

namespace detail {

template <typename Derived>
struct anonymize_builder: public add_sort_expressions<data::detail::anonymize_builder, Derived>
{
  using super = add_sort_expressions<data::detail::anonymize_builder, Derived>;
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

  template <class T>
  void apply(T& result, const action_label& x)
  {
    derived().enter(x);
    data::sort_expression sort;
    derived().apply(sort, x.sorts());
    make_action_label(result,action_label_name_substitution[x.name()], sort);
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const action_name_multiset& x)
  {
    const auto& names = x.names();
    result = action_name_multiset(core::identifier_string_list(
                                          names.begin(), 
                                          names.end(), 
                                          [&](const core::identifier_string& y) { return action_label_name_substitution[y]; }));
  }

  template <class T>
  void apply(T& result, const allow& x)
  {
    derived().enter(x);
    process_expression body;
    derived().apply(body, x.operand());
    action_name_multiset_list allow_set;
    derived().apply(allow_set, x.allow_set());
    make_allow(result, allow_set, body);
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const hide& x)
  {
    derived().enter(x);
    const auto& H = x.hide_set();
    core::identifier_string_list H1(H.begin(), H.end(), [&](const core::identifier_string& y) { return action_label_name_substitution[y]; });
    process_expression body;
    derived().apply(body, x.operand());
    make_hide(result,H1, body);
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const block& x)
  {
    derived().enter(x);
    const auto& B = x.block_set();
    core::identifier_string_list B1(B.begin(), B.end(), [&](const core::identifier_string& y) { return action_label_name_substitution[y]; });
    process_expression body;
    derived().apply(body, x.operand());
    make_block(result,B1, body);
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const rename_expression& x)
  {
    result = rename_expression(action_label_name_substitution[x.source()], action_label_name_substitution[x.target()]);
  }

  template <class T>
  void apply(T& result, const rename& x)
  {
    derived().enter(x);
    process_expression body;
    derived().apply(body, x.operand());
    rename_expression_list rename_set;
    derived().apply(rename_set, x.rename_set());
    make_rename(result,rename_set, body);
    derived().leave(x);
  }

  template <class T>
  void apply(T& result, const communication_expression& x)
  {
    action_name_multiset ms;
    derived().apply(ms, x.action_name());
    make_communication_expression(result, ms, action_label_name_substitution[x.name()]);
  }

  template <class T>
  void apply(T& result, const comm& x)
  {
    derived().enter(x);
    communication_expression_list l;
    derived().apply(l, x.comm_set());
    process_expression body;
    derived().apply(body, x.operand());
    make_comm(result,l, body);
    derived().leave(x);
  }

  void enter(const process_identifier& x)
  {
    add_name(x.name(), process_identifier_name_substitution, "P");
  }

  template <class T>
  void apply(T& result, const process_identifier& x)
  {
    derived().enter(x);
    data::variable_list vars;
    derived().apply(vars, x.variables());
    make_process_identifier(result,process_identifier_name_substitution[x.name()], vars);
    derived().leave(x);
  }
};

struct anonymize_builder_instance: public anonymize_builder<anonymize_builder_instance>
{
};

} // namespace detail

inline
void anonymize(process_specification& procspec)
{
  detail::anonymize_builder_instance f;
  f.update(procspec);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ANONYMIZE_H
