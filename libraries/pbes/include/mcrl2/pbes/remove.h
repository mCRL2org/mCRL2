// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/remove.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REMOVE_H
#define MCRL2_PBES_REMOVE_H

#include "mcrl2/pbes/detail/pbes_parameter_remover.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Removes parameters from a PBES data type.
  template <typename Object, typename SetContainer>
  void remove_parameters(Object& o, const SetContainer& to_be_removed)
  {
    pbes_system::detail::pbes_parameter_remover<SetContainer> r(to_be_removed);
    r(o);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REMOVE_H
