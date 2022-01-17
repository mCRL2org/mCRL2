// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ADD_BINDING_H
#define MCRL2_PROCESS_ADD_BINDING_H

#include "mcrl2/data/add_binding.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public data::add_data_variable_binding<Builder, Derived>
{
  typedef data::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void enter(const process::sum& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const process::sum& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const process::stochastic_operator& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const process::stochastic_operator& x)
  {
    decrease_bind_count(x.variables());
  }
};

// TODO: get rid of this code duplication
// special handling for where clauses
template <template <class> class Builder, class Derived>
struct add_data_variable_traverser_binding: public add_data_variable_binding<Builder, Derived>
{
  typedef add_data_variable_binding<Builder, Derived> super;
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
  typedef add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  template <class T>
  void apply(T& result, const data::where_clause& x)
  {
    data::assignment_expression_list declarations;
    static_cast<Derived&>(*this).apply(declarations, x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::make_where_clause(result, [&](data::data_expression& r){ static_cast<Derived&>(*this).apply(r, x.body()); }, declarations);
    static_cast<Derived&>(*this).leave(x);
  }
};

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ADD_BINDING_H
