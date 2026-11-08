// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSCONVERTSYMBOLIC_CONVERT_CONCRETE_LTS_H
#define MCRL2_LTSCONVERTSYMBOLIC_CONVERT_CONCRETE_LTS_H

#include "mcrl2/lps/symbolic_lts_io.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

/// \brief Converts a state vector of indices to a vector of the corresponding data expressions.
inline lps::state
array2state(const std::vector<symbolic::data_expression_index>& data_index, std::uint32_t* x, std::size_t n)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < n; i++)
  {
    if (x[i] == symbolic::relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[i][x[i]]);
    }
  }
  
  return lps::state(result.begin(), n);
}

/// \brief Explore all outgoing transitions per state vector.
template <typename Context>
void explore_state_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context);

class convert_concrete_lts
{
  public:
    convert_concrete_lts(const lps::symbolic_lts& lts, std::unique_ptr<lts::lts_builder> builder)
      : m_lts(lts), m_builder(std::move(builder)), m_progress_monitor(mcrl2::lps::exploration_strategy::es_none)
    {
      m_number_of_states = satcount(m_lts.states);
    }

    void run()
    {
      // Explore all states in the LDD.
      sat_all_nopar(m_lts.states, explore_state_callback<convert_concrete_lts>, this);

      m_progress_monitor.finish_exploration(m_discovered.size(), 1);
      m_builder->finalize(m_discovered, false);
    }

    void save(const std::string& filename)
    {
      m_builder->save(filename);
    }

    const lps::symbolic_lts& m_lts;
    std::unique_ptr<lts::lts_builder> m_builder;
    mcrl2::lts::lts_builder::indexed_set_for_states_type m_discovered;
    mcrl2::lts::detail::progress_monitor m_progress_monitor;
    std::size_t m_number_of_states;
};

/// \brief std::tuple is completely useless due to the cryptic compilation errors.
struct callback_context
{
  convert_concrete_lts& algorithm;
  std::uint32_t* state;
  std::size_t state_size;
  std::size_t state_index;
  const lps::lps_summand_group& group;
};

/// \brief Adds the transition x of the summand group from the current state, if x applies to it.
/// \details A copy node in x is relprod_ignore. At a read position it matches every value, and at a write position
///          it keeps the value of the current state.
inline void explore_transition(callback_context& p, const std::uint32_t* x, std::size_t n)
{
  // Try to match the read parameters.
  for (std::size_t i = 0; i < p.group.read.size(); ++i)
  {
    const std::uint32_t value = x[p.group.read_pos[i]];
    if (value != symbolic::relprod_ignore && p.state[p.group.read[i]] != value)
    {
      return;
    }
  }

  // Apply the writes to the state vector.
  MCRL2_DECLARE_STACK_ARRAY(target, std::uint32_t, p.state_size);
  for (std::size_t i = 0; i < p.state_size; ++i)
  {
    target[i] = p.state[i];
  }

  for (std::size_t i = 0; i < p.group.write.size(); ++i)
  {
    const std::uint32_t value = x[p.group.write_pos[i]];
    if (value != symbolic::relprod_ignore)
    {
      target[p.group.write[i]] = value;
    }
  }

  lps::multi_action action_label = p.algorithm.m_lts.action_index[x[n - 1]];
  lps::state target_state(array2state(p.algorithm.m_lts.data_index, target.data(), p.state_size));
  std::size_t target_index = p.algorithm.m_discovered.insert(target_state).first;

  p.algorithm.m_progress_monitor.examine_transition();
  p.algorithm.m_builder->add_transition(p.state_index, action_label, target_index);
}

/// \brief Calls explore_transition for every vector in L, where x[0 .. depth) is the prefix that leads to L.
/// \details Unlike sat_all_nopar, this gives relprod_ignore for a copy node instead of its value, which is meaningless.
///          It only reads nodes, so garbage collection cannot happen meanwhile; the summand group keeps L alive.
inline void explore_transitions(callback_context& p, sylvan::MDD L, std::uint32_t* x, std::size_t depth)
{
  for (; L != sylvan::lddmc_false; L = sylvan::lddmc_getright(L))
  {
    if (L == sylvan::lddmc_true)
    {
      explore_transition(p, x, depth);
      return;
    }

    x[depth] = sylvan::lddmc_iscopy(L) ? symbolic::relprod_ignore : sylvan::lddmc_getvalue(L);
    explore_transitions(p, sylvan::lddmc_getdown(L), x, depth + 1);
  }
}

template <typename Context>
void explore_state_callback(WorkerP*, Task*, std::uint32_t* x, std::size_t n, void* context)
{
  auto p = reinterpret_cast<Context*>(context);
  auto& algorithm = *p;

  lps::state current(array2state(algorithm.m_lts.data_index, x, n));
  auto [current_index, _] = algorithm.m_discovered.insert(current);

  for (const lps::lps_summand_group& group : algorithm.m_lts.summand_groups)
  {
    // Find all outgoing transitions of this state. A transition consists of the read and written values and the action label.
    callback_context context { algorithm, x, n, current_index, group };
    MCRL2_DECLARE_STACK_ARRAY(transition, std::uint32_t, group.read.size() + group.write.size() + 1);
    explore_transitions(context, group.L.get(), transition.data(), 0);
  }

  algorithm.m_progress_monitor.finish_state(algorithm.m_discovered.size(), algorithm.m_number_of_states - algorithm.m_discovered.size(), 1);
}

} // namespace mcrl2

#endif // MCRL2_LTSCONVERTSYMBOLIC_CONVERT_CONCRETE_LTS_H
