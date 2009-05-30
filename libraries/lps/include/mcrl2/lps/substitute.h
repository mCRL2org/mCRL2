// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/substitute.h
/// \brief add your file description here.

#include "mcrl2/lps/detail/lps_replacer.h"

#ifndef MCRL2_LPS_SUBSTITUTE_H
#define MCRL2_LPS_SUBSTITUTE_H

namespace mcrl2 {

namespace lps {

  /// \brief Applies a substitution to an LPS data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, const Substitution& sigma)
  {
    lps::detail::lps_replacer<Substitution> r(sigma);
    r(o);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUBSTITUTE_H
