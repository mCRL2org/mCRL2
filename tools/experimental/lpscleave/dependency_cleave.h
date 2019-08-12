// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/stochastic_specification.h"


namespace mcrl2
{

/// \brief Performs the a dependency cleave based on the given parameters V, and the indices J.
lps::stochastic_specification dependency_cleave(const lps::stochastic_specification& spec,
  const data::variable_list& parameters,
  const std::list<std::size_t>& indices,
  bool right_process);

} // namespace mcrl2
