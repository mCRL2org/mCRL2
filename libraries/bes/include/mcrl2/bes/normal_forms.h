// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/normal_forms.h
/// \brief add your file description here.

#ifndef MCRL2_BES_NORMAL_FORMS_H
#define MCRL2_BES_NORMAL_FORMS_H

#include "mcrl2/bes/detail/standard_form_traverser.h"
//#include "mcrl2/bes/detail/standard_form_visitor.h"

namespace mcrl2
{

namespace bes
{

/// \brief Transforms a BES into standard form.
/// \param eqn A boolean equation system
/// \param recursive_form Determines whether or not the result will be in standard recursive normal form
inline
void make_standard_form(boolean_equation_system<>& eqn, bool recursive_form = false)
{
  detail::standard_form_traverser t(recursive_form);
  t(eqn);
  assert(!is_boolean_variable(eqn.initial_state()) || eqn.equations().begin()->variable() == boolean_variable(eqn.initial_state()));
  assert(!is_boolean_variable(eqn.initial_state()) || t.m_equations.begin()->variable() == boolean_variable(eqn.initial_state()));
  eqn.equations() = t.m_equations;
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_NORMAL_FORMS_H
