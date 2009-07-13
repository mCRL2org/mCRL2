// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/substitute_fwd.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_SUBSTITUTE_FWD_H
#define MCRL2_LPS_SUBSTITUTE_FWD_H

namespace mcrl2 {

namespace lps {

  /// \brief Applies a substitution to an LPS data type.
  template <typename Object, typename Substitution>
  void substitute(Object& o, const Substitution& sigma, bool replace_parameters = false);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUBSTITUTE_FWD_H
