// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/anonymize.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ANONYMIZE_H
#define MCRL2_PBES_ANONYMIZE_H

#include "mcrl2/data/anonymize.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Derived>
struct anonymize_builder: public add_sort_expressions<data::detail::anonymize_builder, Derived>
{
  typedef add_sort_expressions<data::detail::anonymize_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::add_name;
  using super::derived;

  std::map<core::identifier_string, core::identifier_string> propositional_variable_name_substitution;

  void update(pbes& x)
  {
    super::update(x.data());
    super::update(x);
  }

  void enter(const propositional_variable& x)
  {
    add_name(x.name(), propositional_variable_name_substitution, "X");
  }

  propositional_variable apply(const propositional_variable& x)
  {
    derived().enter(x);
    propositional_variable result(propositional_variable_name_substitution[x.name()], derived().apply(x.parameters()));
    derived().leave(x);
    return result;
  }

  void enter(const propositional_variable_instantiation& x)
  {
    add_name(x.name(), propositional_variable_name_substitution, "X");
  }

  propositional_variable_instantiation apply(const propositional_variable_instantiation& x)
  {
    derived().enter(x);
    propositional_variable_instantiation result = propositional_variable_instantiation(propositional_variable_name_substitution[x.name()], derived().apply(x.parameters()));
    derived().leave(x);
    return result;
  }
};

struct anonymize_builder_instance: public anonymize_builder<anonymize_builder_instance>
{
};

} // namespace detail

inline
void anonymize(pbes& pbesspec)
{
  detail::anonymize_builder_instance f;
  f.update(pbesspec);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ANONYMIZE_H
