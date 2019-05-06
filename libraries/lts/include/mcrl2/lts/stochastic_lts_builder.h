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

#include "mcrl2/lts/lts_builder.h"

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

  // Set the initial (stochastic) state of the LTS
  virtual void set_initial_state(const std::list<std::size_t>& targets, const std::vector<data::data_expression>& probabilities) = 0;

  // Add a transition to the LTS
  virtual void add_transition(std::size_t from, const process::timed_multi_action& a, const std::list<std::size_t>& targets, const std::vector<data::data_expression>& probabilities) = 0;

  // Add actions and states to the LTS
  virtual void finalize(const std::unordered_map<lps::state, std::size_t>& state_map) = 0;

  // Save the LTS to a file
  virtual void save(const std::string& filename) = 0;

  virtual ~stochastic_lts_builder() = default;
};

class stochastic_lts_none_builder: public stochastic_lts_builder
{
  public:
    void set_initial_state(const std::list<std::size_t>& /* to */, const std::vector<data::data_expression>& /* probabilities */) override
    {}

    void add_transition(std::size_t /* from */, const process::timed_multi_action& /* a */, const std::list<std::size_t>& /* targets */, const std::vector<data::data_expression>& /* probabilities */) override
    {}

    void finalize(const std::unordered_map<lps::state, std::size_t>& /* state_map */) override
    {}

    void save(const std::string& /* filename */) override
    {}
};

class stochastic_lts_aut_builder: public stochastic_lts_builder
{
  protected:
    struct stochastic_state
    {
      std::list<std::size_t> targets;
      std::vector<data::data_expression> probabilities;

      stochastic_state(std::list<std::size_t>  targets_, std::vector<data::data_expression>  probabilities_)
        : targets(std::move(targets_)), probabilities(std::move(probabilities_))
      {}

      void save_to_aut(std::ostream& out) const
      {
        auto j = targets.begin();
        out << *j;
        for (auto i = probabilities.begin(); j != targets.end(); ++i, ++j)
        {
          out << " " << lps::print_probability(*i) << " " << *j;
        }
      }
    };

    struct transition
    {
      std::size_t from;
      std::size_t label;
      std::size_t to;

      transition(std::size_t from_, std::size_t label_, std::size_t to_)
        : from(from_), label(label_), to(to_)
      {}

      bool operator<(const transition& other) const
      {
        return std::tie(from, label, to) < std::tie(other.from, other.label, other.to);
      }
    };

    std::vector<stochastic_state> m_stochastic_states;
    std::vector<transition> m_transitions;
    std::size_t m_number_of_states = 0;

  public:
    stochastic_lts_aut_builder() = default;

    // Set the initial (stochastic) state of the LTS
    void set_initial_state(const std::list<std::size_t>& targets, const std::vector<data::data_expression>& probabilities) override
    {
      m_stochastic_states.emplace_back(targets, probabilities);
    }

    // Add a transition to the LTS
    void add_transition(std::size_t from, const process::timed_multi_action& a, const std::list<std::size_t>& targets, const std::vector<data::data_expression>& probabilities) override
    {
      std::size_t to = m_stochastic_states.size();
      std::size_t label = add_action(a);
      m_stochastic_states.emplace_back(targets, probabilities);
      m_transitions.emplace_back(from, label, to);
    }

    // Add actions and states to the LTS
    void finalize(const std::unordered_map<lps::state, std::size_t>& state_map) override
    {
      m_number_of_states = state_map.size();
    }

    void save(std::ostream& out) const
    {
      std::vector<process::timed_multi_action> actions{ m_actions.size() };
      for (const auto& p: m_actions)
      {
        actions[p.second] = p.first;
      }

      out << "des (";
      m_stochastic_states[0].save_to_aut(out);
      out << "," << m_transitions.size() << "," << m_number_of_states << ")" << "\n";

      for (const transition& t: m_transitions)
      {
        out << "(" << t.from << ",\"" << lps::pp(actions[t.label]) << "\",";
        m_stochastic_states[t.to].save_to_aut(out);
        out << ")" << "\n";
      }
    }

    // Save the LTS to a file
    void save(const std::string& filename) override
    {
      if (filename.empty())
      {
        save(std::cout);
      }
      else
      {
        std::ofstream out(filename.c_str());
        if (!out.is_open())
        {
          throw mcrl2::runtime_error("cannot create .aut file '" + filename + ".");
        }
        save(out);
        out.close();
      }
    }
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_STOCHASTIC_LTS_BUILDER_H
