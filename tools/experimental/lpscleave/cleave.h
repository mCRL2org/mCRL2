// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_CLEAVE_H_
#define MCRL2_CLEAVE_H_

#include "mcrl2/lps/stochastic_specification.h"

namespace mcrl2
{
/// \brief Performs the a refined cleave based on the given parameters V and W.
std::pair<lps::stochastic_specification, lps::stochastic_specification> cleave(
  const lps::stochastic_specification& spec,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  const std::list<std::size_t>& indices,
  const data::data_expression& invariant,
  const std::string& action_prefix,
  bool enable_split_condition,
  bool split_action,
  bool merge_heuristic,
  bool use_next_state
);

} // namespace mcrl2

#endif // MCRL2_CLEAVE_H_
