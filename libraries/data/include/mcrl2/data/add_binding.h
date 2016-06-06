// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/data/variable.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_equation.h"

namespace mcrl2
{

namespace data
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public core::add_binding<Builder, Derived, variable>
{
  typedef core::add_binding<Builder, Derived, variable> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void increase_bind_count(const assignment_list& assignments)
  {
    for (const auto& assignment : assignments)
    {
      increase_bind_count(assignment.lhs());
    }
  }

  void decrease_bind_count(const assignment_list& assignments)
  {
    for (const auto& assignment : assignments)
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

  void enter(const data::data_equation& x)
  {
    increase_bind_count(x.variables());
  }

  void leave(const data::data_equation& x)
  {
    decrease_bind_count(x.variables());
  }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ADD_BINDING_H
