// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_COMBINE_H_
#define MCRL2_COMBINE_H_

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"

namespace mcrl2
{

/// \brief Generate an mCRL2 specification from two given LPSs resulting from lpscleave.
void combine_specification(const lps::stochastic_specification& left_spec,
  const lps::stochastic_specification& right_spec,
  const std::string& prefix,
  std::ostream& stream);
}

#endif // MCRL2_COMBINE_H_