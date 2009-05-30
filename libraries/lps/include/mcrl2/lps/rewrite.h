// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rewrite.h
/// \brief add your file description here.

#include "mcrl2/lps/detail/lps_rewriter.h"

#ifndef MCRL2_LPS_REWRITE_H
#define MCRL2_LPS_REWRITE_H

namespace mcrl2 {

namespace lps {

  /// \brief Rewrites an LPS data type.
  template <typename Object, typename DataRewriter>
  void rewrite(Object& o, const DataRewriter& R)
  {
    lps::detail::lps_rewriter<DataRewriter> r(R);
    r(o);
  }

  /// \brief Rewrites an LPS data type.
  template <typename Object, typename DataRewriter, typename Substitution>
  void rewrite(Object& o, const DataRewriter& R, const Substitution& sigma)
  {
    lps::detail::rewriter_adapter<DataRewriter, Substitution> Rsigma(R, sigma);
    rewrite(o, Rsigma);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REWRITE_H
