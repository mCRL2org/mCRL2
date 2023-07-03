// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/detail/fsm_builder.h
/// \brief The code in this file is used to construct an lts in fsm format. 

#ifndef MCRL2_LTS_DETAIL_FSM_BUILDER_H
#define MCRL2_LTS_DETAIL_FSM_BUILDER_H

#include <map>
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/utilities/parse_numbers.h"

namespace mcrl2::lts::detail {

// Read a numeric value before a symbol c1 or c2, and remove it from s, including the symbol.
inline std::string split_string_until(std::string& s, const std::string& c1, const std::string& c2="")
{
  std::size_t n=s.find(c1);
  if (!c2.empty())
  {
    n=std::min(n,s.find(c2));
  }
  if (n==std::string::npos)
  {
    if (c2.empty())
    { 
      throw mcrl2::runtime_error("Expect '" + c1 + "' in distribution " + s + ".");
    }
    else
    {
      throw mcrl2::runtime_error("Expect either '" + c1 + "' or '" + c2 + " in distribution " + s + ".");
    }
  }
  std::string result=s.substr(0,n);
  s=s.substr(n+1);
  return result;
}

inline lts_fsm_base::probabilistic_state parse_distribution(const std::string& distribution)
{
  if (distribution.find('[')==std::string::npos) // So the distribution must consist of a state index.
  {
    std::size_t state_number=utilities::parse_natural_number(distribution);
    if (state_number==0)
    {
      throw mcrl2::runtime_error("Transition has a zero as target state number.");
    }
    return lts_fsm_base::probabilistic_state(state_number-1);
  }
  
  // Otherwise the distribution has the shape [state1 enumerator1/denominator1 ... staten enumeratorn/denominatorn]
  std::vector<lts_fsm_base::state_probability_pair> result;
  std::string s = utilities::trim_copy(distribution);
  if (s.substr(0,1) != "[")
  {
    throw mcrl2::runtime_error("Distribution does not start with ']': " + distribution + ".");
  }
  s = s.substr(1);  // Remove initial "[";
  for(; s.size() > 1; s = utilities::trim_copy(s))
  {
    std::size_t state_number = utilities::parse_natural_number(split_string_until(s," "));
    if (state_number == 0)
    {
      throw mcrl2::runtime_error("Transition has a zero as target state number.");
    }
    std::string enumerator = split_string_until(s,"/");
    std::string denominator = split_string_until(s," ","]");
    result.emplace_back(state_number - 1, probabilistic_arbitrary_precision_fraction(enumerator,denominator));
  }
  return lts_fsm_base::probabilistic_state(result.begin(), result.end());
}

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
/// \note All state numbers in an FSM file are larger or equal to 1. When reading one is subtracted.
class fsm_transition
{
  protected:
    std::size_t m_source;
    detail::lts_fsm_base::probabilistic_state m_target;
    std::string m_label;

  public:
    fsm_transition(std::size_t source, std::size_t target, const std::string& label)
      : m_source(source - 1),
        m_target(detail::lts_fsm_base::probabilistic_state(target - 1)),
        m_label(label)
    {
      if (source == 0 || target == 0)
      {
        throw mcrl2::runtime_error("A transition contains a state with state number 0.");
      }
    }

    fsm_transition(const std::string& source_text, const std::string& target_text, const std::string& label)
      : m_label(label)
    {
      std::size_t source = utilities::parse_natural_number(source_text);
      if (source == 0)
      {
        throw mcrl2::runtime_error("A transition constains a source state with number 0.");
      }
      m_source = source - 1;
      m_target = parse_distribution(target_text);
    }

    std::size_t source() const
    {
      return m_source;
    }

    std::size_t& source()
    {
      return m_source;
    }

    const lts_fsm_base::probabilistic_state& target() const
    {
      return m_target;
    }

    lts_fsm_base::probabilistic_state& target()
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
  explicit fsm_builder(probabilistic_lts_fsm_t& fsm_)
    : fsm(fsm_),
      m_initial_state_is_set(false)
  {}

  // Contains the result
  probabilistic_lts_fsm_t& fsm;

  // The parameters of the FSM
  std::vector<fsm_parameter> parameters;

  // Maps labels of the FSM to numbers
  std::map<std::string, std::size_t> labels;

  // This variable records if the initial state is set explicitly.
  // If not it needs to be done while finishing the fsm.
  bool m_initial_state_is_set;

  void start()
  {
    parameters.clear();
    labels.clear();
    labels[action_label_string::tau_action()] = 0; // The label 0 is the tau action by default.
    fsm.clear();
  }

  std::size_t find_maximal_state_index(const lts_fsm_base::probabilistic_state& distribution)
  {
    std::size_t max = 0;
    if (distribution.size()>1)
    { 
      for (const detail::lts_fsm_base::state_probability_pair& p: distribution)
      { 
        max = std::max(max, p.state());
      }
    } 
    else
    {
      max=distribution.get();
    }
    return max;
  }

  void add_transition(const std::string& source, const std::string& target, const std::string& label)
  {
    fsm_transition t(source, target, label);
    
    // Apply a correction with +1 for the mismatch between numbering in lts_fsm and the fsm file format
    std::size_t max = std::max(t.source(),find_maximal_state_index(t.target()))+1;

    if (fsm.num_states() <= max)
    {
      fsm.set_num_states(max, fsm.has_state_info());
    }
    auto i = labels.find(t.label());
    lts_fsm_t::labels_size_type label_index = 0;
    if (i == labels.end())
    {
      assert(t.label() != action_label_string::tau_action());
      label_index = fsm.add_action(action_label_string(t.label()));
      labels[t.label()] = label_index;
    }
    else
    {
      label_index = i->second;
    }

    const std::size_t probabilistic_state_index = fsm.add_probabilistic_state(detail::lts_fsm_base::probabilistic_state(t.target()));
    fsm.add_transition(transition(t.source(), label_index, probabilistic_state_index));
  }

  void add_state(const std::vector<std::size_t>& values)
  {
    fsm.add_state(state_label_fsm(values));
  }

  void add_parameter(const std::string& name, const std::string& cardinality, const std::string& sort, const std::vector<std::string>& domain_values)
  {
    parameters.emplace_back(name, cardinality, sort, domain_values);
  }

  void add_initial_distribution(const std::string& distribution)
  {
    const lts_fsm_base::probabilistic_state d=parse_distribution(distribution);
    std::size_t max=find_maximal_state_index(d)+1;  // Add one as state numbers are subtracted by one when parsing. 

    if (fsm.num_states() <= max)
    {
      fsm.set_num_states(max, fsm.has_state_info());
    }
    fsm.add_probabilistic_state(detail::lts_fsm_base::probabilistic_state(d));

    fsm.set_initial_probabilistic_state(d);
    m_initial_state_is_set = true;
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
    if (!m_initial_state_is_set)
    {
      fsm.set_initial_probabilistic_state(detail::lts_fsm_base::probabilistic_state(0));
    }
  }
};

} // namespace mcrl2::lts::detail

#endif // MCRL2_LTS_DETAIL_FSM_BUILDER_H
