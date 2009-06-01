// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/traverse.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TRAVERSE_H
#define MCRL2_PROCESS_TRAVERSE_H

#include "mcrl2/process/detail/process_sort_traverser.h"

namespace mcrl2 {

namespace process {

  /// \brief Traverses all sort expressions in the data type o, and writes them
  /// to the output iterator dest.
  template <typename Object, typename OutIter>
  void traverse_sort_expressions(const Object& o, OutIter dest)
  {
    process::detail::process_sort_traverser<OutIter> r(dest);
    r(o);
  }

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRAVERSE_H
