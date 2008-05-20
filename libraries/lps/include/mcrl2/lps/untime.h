// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file untime.h
/// \brief Removes time from a linear process.

#ifndef MCRL2_LPS_UNTIME_H
#define MCRL2_LPS_UNTIME_H

#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  specification untime(const specification& s);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_UNTIME_H
