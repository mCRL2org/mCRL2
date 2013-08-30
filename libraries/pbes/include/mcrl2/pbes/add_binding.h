// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/add_binding.h
/// \brief add your file description here.

// to avoid circular header file problems
#ifndef MCRL2_PBES_H
#include "mcrl2/pbes/pbes.h"
#endif

#ifndef MCRL2_PBES_ADD_BINDING_H
#define MCRL2_PBES_ADD_BINDING_H

#include "mcrl2/data/add_binding.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public data::add_data_variable_binding<Builder, Derived>
{
  typedef data::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::bind_count;
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ADD_BINDING_H
