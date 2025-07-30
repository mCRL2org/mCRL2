// Author(s): Jan Friso Groote. Based on lts2pbes_lts.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lts2pres_lts.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_LTS2PRES_LTS_H
#define MCRL2_PRES_DETAIL_LTS2PRES_LTS_H

#include "mcrl2/lts/lts_lts.h"

namespace mcrl2::pres_system::detail
{

inline
std::ostream& operator<<(std::ostream& out, const lts::probabilistic_lts_lts_t& ltsspec)
{
  const std::vector<mcrl2::lts::action_label_lts>& action_labels = ltsspec.action_labels();
  out << "LTS" << std::endl;
  for (const lts::transition& tr: ltsspec.get_transitions())
  {
    out << tr.from() << " --" << action_labels[tr.label()] << "--> " << tr.to() << std::endl;
  }
  return out;
}

// custom LTS type that maps states to a vector of outgoing edges
class lts2pres_lts
{
  public:
    using state_type = std::size_t;
    using probabilistic_state_type = std::size_t;
    using label_type = std::size_t;

    struct edge
    {
      edge(label_type label_, probabilistic_state_type state_, std::size_t index_)
        : label(label_),
          probabilistic_state(state_),
          index(index_)
      {}

      label_type label;
      probabilistic_state_type probabilistic_state;
      std::size_t index;
    };

    using edge_list = std::vector<edge>;

  protected:
    std::map<state_type, edge_list> m_state_map;
    std::vector<lps::multi_action> m_action_labels;
    std::size_t m_state_count;
    edge_list m_empty_edge_list;

  public:
    lts2pres_lts(const lts::probabilistic_lts_lts_t& ltsspec)
    {
      const std::vector<lts::transition>& transitions = ltsspec.get_transitions();
      for (std::size_t i = 0; i < transitions.size(); i++)
      {
        const lts::transition& transition = transitions[i];
        state_type s = transition.from();
        label_type a = transition.label();
        state_type t = transition.to();
        m_state_map[s].emplace_back(a, t, i);
      }

      for (const auto& a: ltsspec.action_labels())
      {
        m_action_labels.emplace_back(a.actions(), a.time());
      }
      m_state_count = ltsspec.num_states();
    }

    // returns the outgoing edges of state s
    const edge_list& edges(state_type s) const
    {
      const std::map<state_type, edge_list>::const_iterator i = m_state_map.find(s);
      if (i == m_state_map.end())
      {
        return m_empty_edge_list;
      }
      else
      {
        return i->second;
      }
    }

    const std::vector<lps::multi_action>& action_labels() const
    {
      return m_action_labels;
    }

    std::size_t state_count() const
    {
      return m_state_count;
    }

    const std::map<state_type, edge_list>& state_map() const
    {
      return m_state_map;
    }
};

inline std::ostream& operator<<(std::ostream& out, const lts2pres_lts& ltsspec)
{
  const auto& action_labels = ltsspec.action_labels();
  out << "pres-LTS" << std::endl;
  for (const auto& p: ltsspec.state_map())
  {
    for (const auto& edge: p.second)
    {
      out << edge.index << " " << p.first << " --" << action_labels[edge.label] << "--> " << edge.probabilistic_state << std::endl;
    }
  }
  return out;
}

} // namespace mcrl2::pres_system::detail

#endif // MCRL2_PRES_DETAIL_LTS2PRES_LTS_H
