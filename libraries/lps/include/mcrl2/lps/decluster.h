// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decluster.h
/// \brief The decluster algorithm.

#ifndef MCRL2_LPS_DECLUSTER_H
#define MCRL2_LPS_DECLUSTER_H

#include "mcrl2/lps/specification.h"
#include "mcrl2/data/rewrite.h"

namespace mcrl2 {

namespace lps {

specification decluster(const specification& s, Rewriter& r, bool finite_only = true);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DECLUSTER_H
