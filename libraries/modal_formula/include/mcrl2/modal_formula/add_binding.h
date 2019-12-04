// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_ADD_BINDING_H
#define MCRL2_MODAL_FORMULA_ADD_BINDING_H

#include "mcrl2/lps/add_binding.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2
{

namespace action_formulas
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public lps::add_data_variable_binding<Builder, Derived>
{
  typedef lps::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

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

  data::where_clause apply(const data::where_clause& x)
  {
    auto declarations = static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), declarations);
    static_cast<Derived&>(*this).leave(x);
    return result;
  }
};

} // namespace action_formulas

namespace regular_formulas
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public action_formulas::add_data_variable_binding<Builder, Derived>
{
  typedef action_formulas::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;
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

  data::where_clause apply(const data::where_clause& x)
  {
    auto declarations = static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), declarations);
    static_cast<Derived&>(*this).leave(x);
    return result;
  }
};

} // namespace regular_formulas

namespace state_formulas
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public regular_formulas::add_data_variable_binding<Builder, Derived>
{
  typedef regular_formulas::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

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

  data::where_clause apply(const data::where_clause& x)
  {
    auto declarations = static_cast<Derived&>(*this).apply(x.declarations());
    static_cast<Derived&>(*this).enter(x);
    data::where_clause result = data::where_clause(static_cast<Derived&>(*this).apply(x.body()), declarations);
    static_cast<Derived&>(*this).leave(x);
    return result;
  }
};

/// \brief Maintains a multiset of bound state variables during traversal
template <template <class> class Builder, class Derived>
struct add_state_variable_binding: public core::add_binding<Builder, Derived, core::identifier_string>
{
  typedef core::add_binding<Builder, Derived, core::identifier_string> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void enter(mu const& x)
  {
    increase_bind_count(x.name());
  }

  void leave(mu const& x)
  {
    decrease_bind_count(x.name());
  }

  void enter(nu const& x)
  {
    increase_bind_count(x.name());
  }

  void leave(nu const& x)
  {
    decrease_bind_count(x.name());
  }
};

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_ADD_BINDING_H
