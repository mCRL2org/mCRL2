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

#include "mcrl2/bes/detail/standard_recursive_form_visitor.h"

namespace mcrl2 {

namespace bes {

  void make_standard_recursive_form(boolean_equation_system<>& eqn)
  {
    detail::standard_recursive_form_visitor visitor;
    visitor.visit_boolean_equation_system(eqn);
    eqn.equations() = visitor.m_equations;
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_NORMAL_FORMS_H
