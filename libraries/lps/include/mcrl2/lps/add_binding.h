// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/add_binding.h
/// \brief add your file description here.

// To avoid circular inclusion problems
#ifndef MCRL2_LPS_SPECIFICATION_H
#include "mcrl2/lps/specification.h"
#endif

#ifndef MCRL2_LPS_ADD_BINDING_H
#define MCRL2_LPS_ADD_BINDING_H

#include "mcrl2/data/add_binding.h"

namespace mcrl2
{

namespace lps
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public data::add_data_variable_binding<Builder, Derived>
{
  typedef data::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::bound_variables;
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

  void enter(const specification& x)
  {
    increase_bind_count(x.global_variables());
  }

  void leave(const specification& x)
  {
    decrease_bind_count(x.global_variables());
  }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ADD_BINDING_H
