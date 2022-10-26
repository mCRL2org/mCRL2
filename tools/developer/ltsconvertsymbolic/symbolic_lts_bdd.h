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

#include "mcrl2/utilities/platform.h"
#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/symbolic_lts.h"

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

  data::data_specification data_spec;
  data::variable_list process_parameters;

  bdd states;
  bdd initial_state;
  bdd state_variables; // Even numbers are used as variables to encode the states (to allow interleaving).
  bdd action_label_variables; // Used to encode the action labels.

  std::vector<transition_group> transitions;

  std::size_t state_variables_length() const { return m_state_variables_length; }
  
private:
  std::size_t m_state_variables_length = 0;
};

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_LTS_BDD_H