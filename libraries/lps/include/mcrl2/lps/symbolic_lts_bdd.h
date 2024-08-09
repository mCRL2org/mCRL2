// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SYMBOLIC_LTS_BDD_H
#define MCRL2_LPS_SYMBOLIC_LTS_BDD_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/symbolic_lts.h"
#include "mcrl2/symbolic/data_index.h"
#include "mcrl2/utilities/indexed_set.h"

#include <sylvan_bdd.hpp>

namespace mcrl2::lps
{

struct transition_group
{
  transition_group(sylvan::bdds::bdd _relation, sylvan::bdds::bdd _variables)
   : relation(_relation),
     variables(_variables)
  {}

  sylvan::bdds::bdd relation;
  sylvan::bdds::bdd variables;
};

/// \brief Represents a symbolic LTS encoded by a disjunctive transition relation and a set of states.
class symbolic_lts_bdd
{
  using bdd = sylvan::bdds::bdd;

public:
  // Convert a LDD symbolic LTS to a BDD representation.
  symbolic_lts_bdd(const symbolic_lts& lts);

  symbolic_lts_bdd(data::data_specification data_spec,
    data::variable_list process_parameters,
    bdd states,
    bdd initial_state,
    bdd state_variables,
    bdd action_labels_variables,
    std::vector<std::uint32_t> bits,
    std::uint32_t bits_action_label,
    std::vector<symbolic::data_expression_index>,
    utilities::indexed_set<lps::multi_action>,
    std::vector<transition_group>,
    std::size_t m_state_variables_length);

  // Provide access to the underlying structure.
  const data::data_specification& data_specification() const { return m_data_spec; }

  const data::variable_list& process_parameters() const { return m_process_parameters; }

  const bdd& states() const { return m_states; }

  const bdd& initial_state() const { return m_initial_state; }

  const bdd& state_variables() const { return m_state_variables; }
  
  const bdd& action_label_variables() const { return m_action_label_variables; }

  std::size_t state_variables_length() const { return m_state_variables_length; }

  std::size_t action_label_bits() const { return m_bits_action_label; }

  const std::vector<transition_group>& transition_groups() const { return m_transitions; }

  const std::vector<std::uint32_t>& state_variable_bits() const { return m_bits; }
  const std::vector<symbolic::data_expression_index>& data_index() const { return m_data_index; }
  const utilities::indexed_set<lps::multi_action>& action_index() const { return m_action_index; }
  
private:
  data::data_specification m_data_spec;
  data::variable_list m_process_parameters;

  bdd m_states;
  bdd m_initial_state;
  bdd m_state_variables; // Even numbers are used as variables to encode the states (to allow interleaving).
  bdd m_action_label_variables; // Used to encode the action labels.

  std::vector<std::uint32_t> m_bits; // The number of bits at every level of the bdd.
  std::uint32_t m_bits_action_label; // The number of bits for the action label (stored at the end of relation).

  std::vector<symbolic::data_expression_index> m_data_index;
  utilities::indexed_set<lps::multi_action> m_action_index;

  std::vector<transition_group> m_transitions;
  
  std::size_t m_state_variables_length = 0;
};

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_LTS_BDD_H