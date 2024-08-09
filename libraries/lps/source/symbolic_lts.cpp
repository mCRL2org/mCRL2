// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/symbolic_lts.h"

#include "mcrl2/lps/symbolic_lts_bdd.h"

using namespace mcrl2::data;
using namespace mcrl2::utilities;

namespace mcrl2::lps
{

symbolic_lts::symbolic_lts(data::data_specification data_spec,
  data::variable_list process_parameters,
  sylvan::ldds::ldd states,
  sylvan::ldds::ldd initial_state,
  std::vector<symbolic::data_expression_index> data_index,
  utilities::indexed_set<lps::multi_action> action_index,
  std::vector<lps_summand_group> summand_groups)
  : m_data_spec(data_spec),
    m_process_parameters(process_parameters),
    m_states(states),
    m_initial_state(initial_state),
    m_data_index(data_index),
    m_action_index(action_index),
    m_summand_groups(summand_groups)
{}

symbolic_lts::symbolic_lts(const symbolic_lts_bdd& lts)
  : m_data_spec(lts.data_specification()),
    m_process_parameters(lts.process_parameters())
{
  // TODO: Convert into this representation

}

} // namespace lps

#endif // MCRL2_ENABLE_SYLVAN