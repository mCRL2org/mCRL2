// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_ADD_BINDING_H
#define MCRL2_LPS_ADD_BINDING_H

#include "mcrl2/lps/stochastic_specification.h"

namespace mcrl2::lps
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public data::add_data_variable_binding<Builder, Derived>
{
  using super = data::add_data_variable_binding<Builder, Derived>;
  using super::apply;
  using super::enter;
  using super::leave;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void enter(const action_summand& x)
  {
    increase_bind_count(x.summation_variables());
  }

  void leave(const action_summand& x)
  {
    decrease_bind_count(x.summation_variables());
  }

  void enter(const stochastic_action_summand& x)
  {
    increase_bind_count(x.summation_variables());
    increase_bind_count(x.distribution().variables());
  }

  void leave(const stochastic_action_summand& x)
  {
    decrease_bind_count(x.summation_variables());
    decrease_bind_count(x.distribution().variables());
  }

  void enter(const deadlock_summand& x)
  {
    increase_bind_count(x.summation_variables());
  }

  void leave(const deadlock_summand& x)
  {
    decrease_bind_count(x.summation_variables());
  }

  void enter(const linear_process& x)
  {
    increase_bind_count(x.process_parameters());
  }

  void leave(const linear_process& x)
  {
    decrease_bind_count(x.process_parameters());
  }

  void enter(const stochastic_linear_process& x)
  {
    increase_bind_count(x.process_parameters());
  }

  void leave(const stochastic_linear_process& x)
  {
    decrease_bind_count(x.process_parameters());
  }

  void enter(const specification& x)
  {
    increase_bind_count(x.global_variables());
  }

  void leave(const specification& x)
  {
    decrease_bind_count(x.global_variables());
  }

  void enter(const stochastic_specification& x)
  {
    increase_bind_count(x.global_variables());
  }

  void leave(const stochastic_specification& x)
  {
    decrease_bind_count(x.global_variables());
  }

  void enter(const stochastic_process_initializer& x)
  {
    increase_bind_count(x.distribution().variables());
  }

  void leave(const stochastic_process_initializer& x)
  {
    decrease_bind_count(x.distribution().variables());
  }
};

// TODO: get rid of this code duplication
// special handling for where clauses
template <template <class> class Builder, class Derived>
struct add_data_variable_traverser_binding: public add_data_variable_binding<Builder, Derived>
{
  using super = add_data_variable_binding<Builder, Derived>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void apply(const data::where_clause& x)
  {
    static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).apply(x.body());
    static_cast<Derived&>(*this).leave(x);
  }
};

// special handling for where clauses
template <template <class> class Builder, class Derived>
struct add_data_variable_builder_binding: public add_data_variable_binding<Builder, Derived>
{
  using super = add_data_variable_binding<Builder, Derived>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  template <class T>
  void apply(T& result,  data::where_clause& x)
  {
    data::assignment_expression declarations;
    static_cast<Derived&>(*this).apply(declarations, x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::make_where_clause(result, [&](data::data_expression& r){ static_cast<Derived&>(*this).apply(r, x.body()); }, declarations);
    static_cast<Derived&>(*this).leave(x);
  }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_ADD_BINDING_H
