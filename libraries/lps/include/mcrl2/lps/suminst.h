// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file suminst.h
/// \brief Instantiate summation variables.

#ifndef MCRL2_LPS_SUMINST_H
#define MCRL2_LPS_SUMINST_H

#include "mcrl2/lps/specification.h"
#include "mcrl2/data/rewrite.h"

namespace mcrl2 {

namespace lps {

struct t_suminst_options {
  bool tau_only;
  bool finite_only;
  RewriteStrategy strategy;

  t_suminst_options()
    : tau_only(false),
      finite_only(true),
      strategy(GS_REWR_JITTY)

  {}
};

specification instantiate_sums(const specification& s, Rewriter& r, const t_suminst_options& o);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMINST_H
