// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/stochastic_stochastic_lts_builder.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_STOCHASTIC_LTS_BUILDER_H
#define MCRL2_LTS_STOCHASTIC_LTS_BUILDER_H

namespace mcrl2 {

namespace lts {

struct stochastic_lts_builder
{
  // All LTS classes use integers to represent actions in transitions. A mapping from actions to integers
  // is needed to avoid duplicates.
  std::unordered_map<process::timed_multi_action, std::size_t> m_actions;

  stochastic_lts_builder()
  {
    process::timed_multi_action tau(process::action_list(), data::undefined_real());
    m_actions.emplace(std::make_pair(tau, m_actions.size()));
  }

  std::size_t add_action(const process::timed_multi_action& a)
  {
    auto i = m_actions.find(a);
    if (i == m_actions.end())
    {
      i = m_actions.emplace(std::make_pair(a, m_actions.size())).first;
    }
    return i->second;
  }

  void print_stochastic_state(const std::list<std::size_t>& to, const std::vector<data::data_expression>& probabilities)
  {
    auto i = to.begin();
    auto j = probabilities.begin();
    for (; i != to.end(); ++i, ++j)
    {
      std::cout << " (" << *j << "," << *i << ")";
    }
    std::cout << std::endl;
  }

  // Set the initial (stochastic) state of the LTS
  virtual void set_initial_state(const std::list<std::size_t>& to, const std::vector<data::data_expression>& probabilities)
  {
    std::cout << "initial state = ";
    print_stochastic_state(to, probabilities);
  }

  // Add a transition to the LTS
  virtual void add_transition(std::size_t from, const process::timed_multi_action& a, const std::list<std::size_t>& to, const std::vector<data::data_expression>& probabilities)
  {
    std::cout << "transition from = " << from << " action = " << a << " target = ";
    print_stochastic_state(to, probabilities);
  }

  // Add actions and states to the LTS
  virtual void finalize(const std::unordered_map<lps::state, std::size_t>& state_map)
  {
  }

  // Save the LTS to a file
  virtual void save(const std::string& filename)
  {
  }

  virtual ~stochastic_lts_builder() = default;
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_STOCHASTIC_LTS_BUILDER_H
