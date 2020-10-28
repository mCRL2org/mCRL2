
// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTS_COMBINE_H_
#define MCRL2_LTS_COMBINE_H_

#include "mcrl2/lts/lts_lts.h"

namespace mcrl2
{

// Combine two LTSs resulting from the state space exploration of LPSs of lpscleave into a single LTS.
void combine_lts(const lts::lts_lts_t& left_lts,
  const lts::lts_lts_t& right_lts,
  const std::string& prefix,
  std::ostream& stream);
}

#endif // MCRL2_LTS_COMBINE_H_