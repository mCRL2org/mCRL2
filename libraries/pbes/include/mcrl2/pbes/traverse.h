// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/traverse.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRAVERSE_H
#define MCRL2_PBES_TRAVERSE_H

#include "mcrl2/pbes/detail/pbes_sort_traverser.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Traverses all sort expressions in the data type o, and writes them
  /// to the output iterator dest.
  template <typename Object, typename OutIter>
  void traverse_sort_expressions(const Object& o, OutIter dest)
  {
    pbes_system::detail::pbes_sort_traverser<OutIter> r(dest);
    r(o);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRAVERSE_H
