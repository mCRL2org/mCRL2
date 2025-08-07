// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_ADD_BINDING_H
#define MCRL2_DATA_ADD_BINDING_H

#include "mcrl2/core/add_binding.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/set_comprehension.h"
#include "mcrl2/data/bag_comprehension.h"
#include "mcrl2/data/untyped_set_or_bag_comprehension.h"

namespace mcrl2::data
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public core::add_binding<Builder, Derived, variable>
{
  using super = core::add_binding<Builder, Derived, variable>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void increase_bind_count(const assignment_list& assignments)
  {
    for (const assignment& assignment : assignments)
    {
      increase_bind_count(assignment.lhs());
    }
  }

  void decrease_bind_count(const assignment_list& assignments)
  {
    for (const assignment& assignment : assignments)
    {
      decrease_bind_count(assignment.lhs());
    }
  }

  void enter(const data::where_clause& x)
  {
    increase_bind_count(x.assignments());
  }

  void leave(const data::where_clause& x)
  {
    decrease_bind_count(x.assignments());
  }

  void enter(const data::forall& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::forall& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::exists& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::exists& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::lambda& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::lambda& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::set_comprehension& x)
  { 
    increase_bind_count(x.variables());
  }

  void leave(const data::set_comprehension& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::bag_comprehension& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::bag_comprehension& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::untyped_set_or_bag_comprehension& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::untyped_set_or_bag_comprehension& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const data::data_equation& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::data_equation& x)
  {
    decrease_bind_count(x.variables());
  }
};

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
  void apply(T& result, const data::where_clause& x)
  {
    assignment_expression_list declarations;  
    static_cast<Derived&>(*this).apply(declarations, x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::make_where_clause(result, 
                            [&](data_expression& r){ static_cast<Derived&>(*this).apply(r, x.body()); }, 
                            declarations);
    static_cast<Derived&>(*this).leave(x);
  }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_ADD_BINDING_H
