// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/lts_builder.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_BUILDER_H
#define MCRL2_LTS_BUILDER_H

#include <unordered_map>
#include "mcrl2/data/undefined.h"
#include "mcrl2/lps/generate_lts.h"
#include "mcrl2/process/timed_multi_action.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_dot.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_lts.h"

namespace mcrl2 {

namespace lts {

struct lts_builder
{
  // All LTS classes use integers to represent actions in transitions. A mapping from actions to integers
  // is needed to avoid duplicates.
  std::unordered_map<process::timed_multi_action, std::size_t> m_actions;

  lts_builder()
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

  // Add a transition to the LTS
  virtual void add_transition(std::size_t from, const process::timed_multi_action& a, std::size_t to) = 0;

  // Add actions and states to the LTS
  virtual void finalize(const atermpp::indexed_set<lps::state>& state_map) = 0;

  // Save the LTS to a file
  virtual void save(const std::string& filename) = 0;
};


class lts_aut_builder: public lts_builder
{
  protected:
    lts_aut_t m_lts;

  public:
    lts_aut_builder() = default;

    void add_transition(std::size_t from, const process::timed_multi_action& a, std::size_t to) override
    {
      std::size_t label = add_action(a);
      m_lts.add_transition(transition(from, label, to));
    }

    // Add actions and states to the LTS
    void finalize(const atermpp::indexed_set<lps::state>& state_map) override
    {
      // add actions
      m_lts.set_num_action_labels(m_actions.size());
      for (const auto& p: m_actions)
      {
        m_lts.set_action_label(p.second, action_label_string(process::pp(p.first)));
      }

      m_lts.set_num_states(state_map.size());
    }

    void save(const std::string& filename) override
    {
      m_lts.save(filename);
    }
};

class lts_lts_builder: public lts_builder
{
  protected:
    lts_lts_t m_lts;

  public:
    lts_lts_builder() = default;

    void add_transition(std::size_t from, const process::timed_multi_action& a, std::size_t to) override
    {
      std::size_t label = add_action(a);
      m_lts.add_transition(transition(from, label, to));
    }

    // Add actions and states to the LTS
    void finalize(const atermpp::indexed_set<lps::state>& state_map) override
    {
      // add actions
      m_lts.set_num_action_labels(m_actions.size());
      for (const auto& p: m_actions)
      {
        m_lts.set_action_label(p.second, action_label_lts(lps::multi_action(p.first.actions(), p.first.time())));
      }

      // add states
      for (std::size_t i = 0; i < state_map.size(); i++)
      {
        m_lts.add_state(state_label_lts(state_map.get(i)));
      }
    }

    void save(const std::string& filename) override
    {
      m_lts.save(filename);
    }
};

class lts_dot_builder: public lts_lts_builder
{
  public:
    void save(const std::string& filename) override
    {
      lts_dot_t dot;
      detail::lts_convert(m_lts, dot);
      m_lts.save(filename);
    }
};

class lts_fsm_builder: public lts_lts_builder
{
  public:
    void save(const std::string& filename) override
    {
      lts_fsm_t fsm;
      detail::lts_convert(m_lts, fsm);
      m_lts.save(filename);
    }
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_BUILDER_H
