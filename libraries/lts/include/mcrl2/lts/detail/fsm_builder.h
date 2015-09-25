// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/detail/fsm_builder.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_DETAIL_FSM_BUILDER_H
#define MCRL2_LTS_DETAIL_FSM_BUILDER_H

#include <map>
#include <string>
#include <vector>

#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/utilities/parse_numbers.h"

namespace mcrl2 {

namespace lts {

namespace detail {

class fsm_parameter
{
  protected:
    std::string m_name;
    std::size_t m_cardinality;
    std::string m_sort;
    std::vector<std::string> m_values;

  public:
    fsm_parameter(const std::string& name, const std::string& cardinality, const std::string& sort, const std::vector<std::string>& values)
      : m_name(name),
        m_cardinality(utilities::parse_natural_number(cardinality)),
        m_sort(sort),
        m_values(values)
    {}

    const std::string& name() const
    {
      return m_name;
    }

    std::string& name()
    {
      return m_name;
    }

    const std::string& sort() const
    {
      return m_sort;
    }

    std::string& sort()
    {
      return m_sort;
    }

    // If the cardinality is zero, the sort and the values are ignored in the FSM.
    std::size_t cardinality() const
    {
      return m_cardinality;
    }

    std::size_t& cardinality()
    {
      return m_cardinality;
    }

    const std::vector<std::string>& values() const
    {
      return m_values;
    }

    std::vector<std::string>& values()
    {
      return m_values;
    }
};

/// \brief Transitions in an FSM
///
/// \note The source and target are 1-based!
class fsm_transition
{
  protected:
    std::size_t m_source;
    std::size_t m_target;
    std::string m_label;

  public:
    fsm_transition(std::size_t source, std::size_t target, const std::string& label)
      : m_source(source - 1),
        m_target(target - 1),
        m_label(label)
    {
      if (source == 0 || target == 0)
      {
        throw mcrl2::runtime_error("transition has a zero coordinate!");
      }
    }

    fsm_transition(const std::string& source_text, const std::string& target_text, const std::string& label)
      : m_label(label)
    {
      std::size_t source = utilities::parse_natural_number(source_text);
      std::size_t target = utilities::parse_natural_number(target_text);
      if (source == 0 || target == 0)
      {
        throw mcrl2::runtime_error("transition has a zero coordinate!");
      }
      m_source = source - 1;
      m_target = target - 1;
    }

    std::size_t source() const
    {
      return m_source;
    }

    std::size_t& source()
    {
      return m_source;
    }

    std::size_t target() const
    {
      return m_target;
    }

    std::size_t& target()
    {
      return m_target;
    }

    const std::string& label() const
    {
      return m_label;
    }

    std::string& label()
    {
      return m_label;
    }
};

struct fsm_builder
{
  fsm_builder(lts_fsm_t& fsm_)
    : fsm(fsm_)
  {}

  // Contains the result
  lts_fsm_t& fsm;

  // The parameters of the FSM
  std::vector<fsm_parameter> parameters;

  // Maps labels of the FSM to numbers
  std::map<std::string, std::size_t> labels;

  void start()
  {
    parameters.clear();
    labels.clear();
    fsm.clear();
  }

  void add_transition(const std::string& source, const std::string& target, const std::string& label)
  {
    fsm_transition t(source, target, label);
    std::size_t max = (std::max)(t.source(), t.target());
    if (fsm.num_states() <= max)
    {
      fsm.set_num_states(max, fsm.has_state_info());
    }
    std::map<std::string, std::size_t>::const_iterator i = labels.find(t.label());
    lts_fsm_t::labels_size_type label_index = 0;
    if (i == labels.end())
    {
      label_index = fsm.add_action(t.label(), t.label() == "tau");
      labels[t.label()] = label_index;
    }
    else
    {
      label_index = i->second;
    }
    fsm.add_transition(transition(t.source(), label_index, t.target()));
  }

  void add_state(const std::vector<std::size_t>& values)
  {
    fsm.add_state(state_label_fsm(values));
  }

  void add_parameter(const std::string& name, const std::string& cardinality, const std::string& sort, const std::vector<std::string>& domain_values)
  {
    parameters.push_back(fsm_parameter(name, cardinality, sort, domain_values));
  }

  void write_parameters()
  {
    std::size_t index = 0;
    for (const fsm_parameter& param: parameters)
    {
      if (param.cardinality() > 0)
      {
        fsm.add_process_parameter(param.name(), param.sort());
        for (const std::string& value: param.values())
        {
          fsm.add_state_element_value(index, value);
        }
      }
      index++;
    }
  }

  void finish()
  {
    // guarantee that the LTS has at least one state
    if (fsm.num_states() == 0)
    {
      fsm.add_state();
    }

    fsm.set_initial_state(0);
  }
};

} // namespace detail

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_DETAIL_FSM_BUILDER_H
