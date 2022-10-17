// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SIGREF_H
#define MCRL2_LPS_SIGREF_H

#include "bdd_util.h"

inline
void sigref(const symbolic_lts_bdd& lts)
{
  // extend transition relations to the full domain.
  std::vector<sylvan::bdds::bdd> new_transition_relations;
  for (const auto& group: lts.transitions) {
      new_transition_relations.emplace_back(extend_relation(group.relation, group.variables, lts.state_variables_length()));
  }

  sylvan::bdds::bdd partition = lts.states;

  

    
  /*BDD partition;
    sylvan_protect(&partition);

    if (lts.getInitialPartition().size() > 0) {
        partition = sylvan_false;
        // note that our algorithms assume a partition is defined on s',b (not s,b)
        for (Bdd dd : lts.getInitialPartition()) {
            // encode next block number
            BDD block = CALL(encode_block, get_next_block());
            bdd_refs_push(block);
            // rename states from s to s'
            BDD states = swap_prime(dd.GetBDD());
            bdd_refs_push(states);
            // block := states' * block
            block = sylvan_and(states, block);
            bdd_refs_push(block);
            // partition := partition + block
            partition = sylvan_or(partition, block);
            bdd_refs_pop(3);
        }
    } else {
        partition = CALL(encode_block, get_next_block());
    }
    */
}

#endif // MCRL2_LPS_SIGREF_H