// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SYMBOLIC_LTS_H
#define MCRL2_LPS_SYMBOLIC_LTS_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/lps_summand_group.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/symbolic/data_index.h"
#include "mcrl2/utilities/indexed_set.h"

#include <sylvan_ldd.hpp>

namespace mcrl2::lps
{

// Forward declaration
class symbolic_lts_bdd;

/// \brief Represents a symbolic LTS encoded by a disjunctive transition relation and a set of states.
class symbolic_lts
{
  using ldd = sylvan::ldds::ldd;

public:
  /// \brief Constructs a symbolic lts from the given information. 
  symbolic_lts(mcrl2::data::data_specification data_spec,
    mcrl2::data::variable_list process_parameters,
    sylvan::ldds::ldd states,
    sylvan::ldds::ldd initial_state,
    std::vector<symbolic::data_expression_index> data_index,
    utilities::indexed_set<lps::multi_action> action_index,
    std::vector<lps_summand_group> summand_groups);  

  /// \brief Converts the bdd to an LDD. 
  symbolic_lts(const symbolic_lts_bdd& lts); 

  symbolic_lts() {};

  
  symbolic_lts(const symbolic_lts& other) = default;
  symbolic_lts(symbolic_lts&& other) = default;
  symbolic_lts& operator=(const symbolic_lts& other) = default;
  symbolic_lts& operator=(symbolic_lts&& other) = default;

  const data::data_specification& data_spec() const { return m_data_spec; };
  const data::variable_list& process_parameters() const { return m_process_parameters; };
  std::uint32_t state_vector_size() const { return m_data_index.size(); }
  const sylvan::ldds::ldd& states() const { return m_states; }
  const sylvan::ldds::ldd& initial_state() const { return m_initial_state; }
  const std::vector<symbolic::data_expression_index>& data_index() const { return m_data_index; }
  const utilities::indexed_set<lps::multi_action>& action_index() const { return m_action_index; }
  const std::vector<lps_summand_group>& summand_groups() const { return m_summand_groups; };

private:
  data::data_specification m_data_spec;
  data::variable_list m_process_parameters;

  sylvan::ldds::ldd m_states;
  sylvan::ldds::ldd m_initial_state;

  std::vector<symbolic::data_expression_index> m_data_index;
  utilities::indexed_set<lps::multi_action> m_action_index;

  std::vector<lps_summand_group> m_summand_groups;
};

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_LTS_H