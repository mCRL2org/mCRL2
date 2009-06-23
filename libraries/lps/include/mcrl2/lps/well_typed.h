// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/well_typed.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_WELL_TYPED_H
#define MCRL2_LPS_WELL_TYPED_H

#include "mcrl2/lps/detail/lps_well_typed_checker.h"

namespace mcrl2 {

namespace lps {

  /// \brief Checks well typedness of an LPS object.
  template <typename Object>
  bool is_well_typed(const Object& o)
  {
    lps::detail::lps_well_typed_checker checker;
    return checker(o);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_WELL_TYPED_H
