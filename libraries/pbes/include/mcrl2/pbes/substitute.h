// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/substitute.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SUBSTITUTE_H
#define MCRL2_PBES_SUBSTITUTE_H

#include "mcrl2/pbes/detail/pbes_substituter.h"
#include "mcrl2/pbes/detail/propositional_variable_substituter.h"

namespace mcrl2 {

namespace pbes_system {

  // TODO: g++ doesn't like 'const Substitution& sigma here; why???
  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, Substitution sigma, bool replace_parameters = false)
  {
    pbes_system::detail::pbes_substituter<Substitution> r(sigma, replace_parameters);
    r(o);
  }

  /// \brief Applies a propositional variable substitution to a PBES data type.
  template <typename Object, typename PropositionalVariableSubstitution>
  void propositional_variable_substitute(Object& o, PropositionalVariableSubstitution sigma, bool replace_parameters = false)
  {
    pbes_system::detail::propositional_variable_substituter<PropositionalVariableSubstitution> r(sigma, replace_parameters);
    r(o);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SUBSTITUTE_H
