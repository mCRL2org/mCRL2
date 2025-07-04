// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ADD_BINDING_H
#define MCRL2_PBES_ADD_BINDING_H

#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public data::add_data_variable_binding<Builder, Derived>
{
  using super = data::add_data_variable_binding<Builder, Derived>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void enter(pbes_equation const& x)
  {
    increase_bind_count(x.variable().parameters());
  }

  void leave(pbes_equation const& x)
  {
    decrease_bind_count(x.variable().parameters());
  }

  void enter(exists const& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(exists const& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(forall const& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(forall const& x)
  {
    decrease_bind_count(x.variables());
  }

  void enter(const pbes& x)
  {
    increase_bind_count(x.global_variables());
  }

  void leave(const pbes& x)
  {
    increase_bind_count(x.global_variables());
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

  
  void apply(data::where_clause& result, const data::where_clause& x)
  {
    auto declarations = static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).enter(x);
    result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), declarations);
    static_cast<Derived&>(*this).leave(x);
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_ADD_BINDING_H
