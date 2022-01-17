// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/remove_data_parameters.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REMOVE_DATA_PARAMETERS_H
#define MCRL2_PROCESS_REMOVE_DATA_PARAMETERS_H

#include "mcrl2/process/builder.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct remove_data_parameters_builder: public process_expression_builder<remove_data_parameters_builder>
{
  typedef process_expression_builder<remove_data_parameters_builder> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  void update(process_specification& x)
  {
    x.data() = data::data_specification();
    x.global_variables().clear();
    std::vector<action_label> labels;
    for (const action_label& l: x.action_labels())
    {
      labels.push_back(action_label(l.name(), {}));
    }
    x.action_labels() = action_label_list(labels.begin(), labels.end());
    process_expression init;
    apply(init, x.init());
    x.init() = init;
    update(x.equations());
  }

  template <class T>
  void apply(T& result, const process_equation& x)
  {
    process_expression expression;
    apply(expression, x.expression());
    process_identifier identifier(x.identifier().name(), {});
    result = process_equation(identifier, {}, expression);
  }

  template <class T>
  void apply(T& result, const process::sum& x)
  {
    apply(result, x.operand());
  }

  template <class T>
  void apply(T& result, const process::action& x)
  {
    result = action(action_label(x.label().name(), {}), {});
  }

  template <class T>
  void apply(T& result, const process::at& x)
  {
    apply(result, x.operand());
  }

  template <class T>
  void apply(T& result, const process::if_then& x)
  {
    apply(result, x.then_case());
  }

  template <class T>
  void apply(T& result, const process::if_then_else& x)
  {
    process_expression then_case;
    apply(then_case, x.then_case());
    process_expression else_case;
    apply(else_case, x.else_case());
    make_if_then_else(result, data::sort_bool::true_(), then_case, else_case);
  }

  template <class T>
  void apply(T& result, const process::process_instance& x)
  {
    process_identifier identifier(x.identifier().name(), {});
    result = process_instance(identifier, {});
  }

  template <class T>
  void apply(T& result, const process::process_instance_assignment& x)
  {
    process_identifier identifier(x.identifier().name(), {});
    result = process_instance(identifier, {});
  }
};

} // namespace detail

inline
void remove_data_parameters(process_specification& procspec)
{
  detail::remove_data_parameters_builder f;
  f.update(procspec);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_REMOVE_DATA_PARAMETERS_H
