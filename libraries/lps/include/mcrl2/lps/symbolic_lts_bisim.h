// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SYMBOLIC_LTS_BISIM_H
#define MCRL2_LPS_SYMBOLIC_LTS_BISIM_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/ldd_stream.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/lps/symbolic_lts.h"

#include <sylvan_ldd.hpp>

namespace mcrl2::lps
{

namespace
{

struct result
{
  sylvan::ldds::ldd projected;
  std::size_t action_index = 0UL;
};

} // anonymous namespace

/// \brief Project on all transitions where the last index is action_index.
template <typename Context>
void project_transitions(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{  
  using namespace sylvan::ldds;

  auto pointer = reinterpret_cast<Context*>(context);
  auto& p = *pointer;

  if (x[n-1] == p.action_index)
  {
    p.projected = union_(p.projected, cube(x, n));
  }
}

/// \brief Project on all transitions where n is the height of the LDD and action_index the action that we want to keep. (Assumes that action label is the last).
inline sylvan::ldds::ldd project_transitions(const sylvan::ldds::ldd& x, std::size_t height, std::size_t action_index)
{
  using namespace sylvan::ldds;

  if (x == empty_set() || x == empty_list())
  {
    return x;
  }
  
  if (height == 1)
  {
    // We are at the action label depth (keep only action_index).
    if (x.value() < action_index)
    {
      return project_transitions(x.right(), height, action_index);
    }
    else if (x.value() == action_index)
    {
      return node(x.value());
    }
    else 
    {
      return false_();
    }
  }
  else
  {
    return node(x.value(), project_transitions(x.down(), height - 1, action_index), project_transitions(x.right(), height, action_index));
  }
}

inline void bisim(const symbolic_lts& lts)
{
  using namespace sylvan::ldds;

  // Split all transition groups such that there is only one action label for each. 
  mCRL2log(log::verbose) << "Preprocessing the transition groups..." << std::endl;
  std::vector<lps_summand_group> new_groups;

  for (const auto& group : lts.summand_groups)
  {
    for (const auto& action : lts.action_index)
    {
      // Explore all transitions in the LDD.
      result res;
      //res.action_index = lts.action_index.index(action);
      //sat_all_nopar(group.L, project_transitions<result>, &res);
      res.projected = project_transitions(group.L, height(group.L), lts.action_index.index(action));

      if (res.projected != sylvan::ldds::empty_set())
      {
        new_groups.emplace_back(group);
        new_groups.back().L = res.projected;
      }
    }
  }

  mcrl2::utilities::unordered_set<ldd> partition; // The set of sets of states, where each set of states is an LDD.
  mcrl2::utilities::unordered_set<ldd> new_partition;
  partition.emplace(lts.states);

  bool refined_block = false;
  bool refined_any_block = false;
  std::size_t iterations = 0;

  mCRL2log(log::verbose) << "Starting signature refinement..." << std::endl;
  do
  {
    refined_any_block = false;

    for (const ldd& C : partition)
    {
      refined_block = false;

      for (const ldd& C_prime : partition)
      {
        // Refine each block one by one.
        for (const lps_summand_group& group : new_groups)
        {
          ldd A = relprev(C_prime, group.L, group.Ir, C);
          if (A != sylvan::ldds::empty_set() && A != C)
          {
            // This block must be refined.
            new_partition.emplace(A),
            new_partition.emplace(minus(C, A));
            refined_block = true;
            break;
          }
        }

        if (refined_block)
        {
          refined_any_block = true;
          break;
        }
      }

      if (!refined_block)
      {
        new_partition.emplace(C); // Keep the old block for the next iteration.
      }
    }

    swap(partition, new_partition);
    new_partition.clear();

    ++iterations;
    mCRL2log(log::verbose) << "found " << std::setw(12) << partition.size() << " equivalence classes after " << std::setw(4) << iterations << " iterations." << std::endl;
  }
  while (refined_any_block);

  // The partition is now a bisimulation.
  mCRL2log(log::verbose) << "There are " << partition.size() << " equivalence classes." << std::endl;
  for (const ldd& C : partition)
  {
    mCRL2log(log::debug) << symbolic::print_size(C, true, true) << std::endl;
    mCRL2log(log::debug) << symbolic::print_states(lts.data_index, C) << std::endl;
  }
}

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_LTS_BISIM_H