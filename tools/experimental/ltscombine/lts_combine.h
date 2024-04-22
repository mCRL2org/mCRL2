
// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTS_COMBINE_H_
#define MCRL2_LTS_COMBINE_H_

#include "../../../libraries/lts/include/mcrl2/lts/lts_lts.h"

namespace mcrl2
{

// Combine two LTSs resulting from the state space exploration of LPSs of lpscleave into a single LTS.
// The input LTSs are modified during the process.
void combine_lts(
  std::vector<lts::lts_lts_t>& ltss,
  std::vector<core::identifier_string_list>& syncs,
  std::vector<core::identifier_string>& resulting_actions,
  std::vector<core::identifier_string>& blocks,
  std::vector<core::identifier_string>& hiden,
  std::vector<core::identifier_string_list>& allow, 
  std::ostream& stream, std::string filename);
}

#endif // MCRL2_LTS_COMBINE_H_