// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/add_binding.h
/// \brief add your file description here.

// to avoid circular header file problems
#ifndef MCRL2_PROCESS_PROCESS_SPECIFICATION_H
#include "mcrl2/process/process_specification.h"
#endif

#ifndef MCRL2_PROCESS_ADD_BINDING_H
#define MCRL2_PROCESS_ADD_BINDING_H

#include "mcrl2/process/process_specification.h"
#include "mcrl2/lps/add_binding.h"

namespace mcrl2
{

namespace process
{

/// \brief Maintains a multiset of bound data variables during traversal
template <template <class> class Builder, class Derived>
struct add_data_variable_binding: public lps::add_data_variable_binding<Builder, Derived>
{
  typedef lps::add_data_variable_binding<Builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::bound_variables;
  using super::increase_bind_count;
  using super::decrease_bind_count;

  void enter(const process::sum& x)
  {
    increase_bind_count(x.bound_variables());
  }

  void leave(const process::sum& x)
  {
    decrease_bind_count(x.bound_variables());
  }
};

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ADD_BINDING_H
