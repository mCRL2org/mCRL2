// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/add_binding.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_ADD_BINDING_H
#define MCRL2_MODAL_FORMULA_ADD_BINDING_H

#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/lps/add_binding.h"

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
  using super::operator();
  using super::bind_count;
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
  using super::operator();
  using super::bind_count;
  using super::increase_bind_count;
  using super::decrease_bind_count;
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
  using super::operator();
  using super::bind_count;
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

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_ADD_BINDING_H
