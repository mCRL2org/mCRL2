// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/new_data/rewriter.h"

namespace mcrl2 {

namespace lps {

struct t_suminst_options {
  bool tau_only;
  bool finite_only;
  new_data::rewriter::strategy strategy;

  t_suminst_options()
    : tau_only(false),
      finite_only(true),
      strategy(new_data::rewriter::jitty)

  {}
};

specification instantiate_sums(const specification& s, new_data::rewriter& r, const t_suminst_options& o);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMINST_H
