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

namespace mcrl2 {

namespace pbes_system {

  /// \brief Applies a substitution to a PBES data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, const Substitution& sigma, bool replace_parameters = false)
  {
    pbes_system::detail::pbes_substituter<Substitution> r(sigma, replace_parameters);
    r(o);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SUBSTITUTE_H
