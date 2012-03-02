// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_PARSE_H
#define MCRL2_LTS_PARSE_H

#include <boost/lexical_cast.hpp>

#include "mcrl2/core/parse.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/lts/lts_fsm.h"

namespace mcrl2 {

namespace lts {

namespace detail {

inline
std::size_t parse_number(const std::string& s)
{
  try
  {
    return boost::lexical_cast<std::size_t>(s);
  }
  catch (boost::bad_lexical_cast&)
  {
    std::cout << "ERROR: could not parse number " << s << std::endl;
  }
  return 0;
}

} // namespace detail

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
        m_cardinality(detail::parse_number(cardinality)),
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

class fsm_transition
{
  protected:
    std::size_t m_source;
    std::size_t m_target;
    std::string m_label;

  public:
    fsm_transition(std::size_t source, std::size_t target, const std::string& label)
      : m_source(source), m_target(target), m_label(label)
    {}

    fsm_transition(const std::string& source, const std::string& target, const std::string& label)
      : m_source(detail::parse_number(source)),
        m_target(detail::parse_number(target)),
        m_label(label)
    {}

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

typedef std::vector<std::size_t> fsm_state;

struct fsm_actions: public core::default_parser_actions
{
  fsm_actions(const core::parser_table& table_)
    : core::default_parser_actions(table_)
  {}

  // The parameters of the FSM
  std::vector<fsm_parameter> m_parameters;

  // The states of the FSM
  std::vector<fsm_state> m_states;

  // The transitions of the FSM
  std::vector<fsm_transition> m_transitions;

  // Maps labels of the FSM to numbers
  std::map<std::string, std::size_t> m_labels;

  template <typename T, typename Function>
  std::vector<T> parse_vector(const core::parse_node& node, const std::string& type, Function f)
  {
    std::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return result;
  }

  std::string parse_Id(const core::parse_node& node)
  {
    return node.string();
  }

  std::vector<std::string> parse_IdList(const core::parse_node& node)
  {
    return parse_vector<std::string>(node, "Id", boost::bind(&fsm_actions::parse_Id, this, _1));
  }

  std::string parse_QuotedString(const core::parse_node& node)
  {
    std::string s = node.string();
    return s.substr(1, s.size() - 2);
  }

  std::string parse_Number(const core::parse_node& node)
  {
    return node.string();
  }

  std::vector<std::string> parse_NumberList(const core::parse_node& node)
  {
    return parse_vector<std::string>(node, "Number", boost::bind(&fsm_actions::parse_Number, this, _1));
  }

  std::string parse_ParameterName(const core::parse_node& node)
  {
    return parse_Id(node.child(0));
  }

  std::string parse_SortExpr(const core::parse_node& node)
  {
    std::string result = parse_Id(node.child(0));
    if (result.empty())
    {
      result = "Nat";
    }
    return result;
  }

  std::string parse_DomainCardinality(const core::parse_node& node)
  {
    return parse_Number(node.child(1));
  }

  std::string parse_DomainValue(const core::parse_node& node)
  {
    return parse_QuotedString(node.child(0));
  }

  std::vector<std::string> parse_DomainValueList(const core::parse_node& node)
  {
    return parse_vector<std::string>(node, "QuotedString", boost::bind(&fsm_actions::parse_QuotedString, this, _1));
  }

  void parse_Parameter(const core::parse_node& node)
  {
    m_parameters.push_back(fsm_parameter(parse_ParameterName(node.child(0)), parse_DomainCardinality(node.child(1)), parse_SortExpr(node.child(2)), parse_DomainValueList(node.child(3))));
  }

  void parse_ParameterList(const core::parse_node& node)
  {
    traverse(node, make_visitor(table, "Parameter", boost::bind(&fsm_actions::parse_Parameter, this, _1)));
  }

  void parse_State(const core::parse_node& node)
  {
    std::vector<std::string> v = parse_NumberList(node.child(0));
    std::vector<std::size_t> result;
    if (v.size() != m_parameters.size())
    {
      throw mcrl2::runtime_error("parse_State: wrong number of elements");
    }
    for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      if (m_parameters[i - v.begin()].cardinality() != 0)
      {
        result.push_back(detail::parse_number(*i));
      }
    }
    m_states.push_back(result);
  }

  void parse_StateList(const core::parse_node& node)
  {
    traverse(node, make_visitor(table, "State", boost::bind(&fsm_actions::parse_State, this, _1)));
  }

  void parse_Transition(const core::parse_node& node)
  {
    m_transitions.push_back(fsm_transition(parse_Source(node.child(0)), parse_Target(node.child(1)), parse_Label(node.child(2))));
  }

  void parse_TransitionList(const core::parse_node& node)
  {
    traverse(node, make_visitor(table, "Transition", boost::bind(&fsm_actions::parse_Transition, this, _1)));
  }

  std::string parse_Source(const core::parse_node& node)
  {
    return parse_Number(node.child(0));
  }

  std::string parse_Target(const core::parse_node& node)
  {
    return parse_Number(node.child(0));
  }

  std::string parse_Label(const core::parse_node& node)
  {
    return parse_QuotedString(node.child(0));
  }

  // Adds a transition to an FSM.
  void add_transition(lts_fsm_t& fsm, const fsm_transition& t)
  {
    std::size_t no_states = fsm.num_states();
    std::size_t max = (std::max)(t.source(), t.target());
    if (no_states <= max)
    {
      fsm.set_num_states(max + 1, fsm.has_state_info());
    }
    std::map<std::string, std::size_t>::const_iterator i = m_labels.find(t.label());
    lts_fsm_t::labels_size_type label_index = 0;
    if (i == m_labels.end())
    {
      label_index = fsm.add_action(t.label(), t.label() == "tau");
      m_labels[t.label()] = label_index;
    }
    else
    {
      label_index = i->second;
    }
    fsm.add_transition(transition(t.source(), label_index, t.target()));
  }

  void parse_FSM(const core::parse_node& node, lts_fsm_t& result)
  {
    result = lts_fsm_t(); // TODO: add a proper clear() function to lts_fsm_t
    m_parameters.clear();
    m_states.clear();
    m_transitions.clear();
    parse_ParameterList(node.child(0));
    parse_StateList(node.child(2));
    parse_TransitionList(node.child(4));
    std::size_t index = 0;

    for (std::vector<fsm_parameter>::const_iterator i = m_parameters.begin(); i != m_parameters.end(); ++i)
    {
      std::cout << "parameter " << i->name() << " " << i->sort() << " " << i->cardinality() << " " << i->values().size() << std::endl;
      if (i->cardinality() > 0)
      {
        result.add_process_parameter(i->name(), i->sort());
        for (std::vector<std::string>::const_iterator j = i->values().begin(); j != i->values().end(); ++j)
        {
          result.add_state_element_value(index, *j);
        }
      }
      index++;
    }

    for (std::vector<fsm_state>::const_iterator j = m_states.begin(); j != m_states.end(); ++j)
    {
      result.add_state(detail::state_label_fsm(*j));
      std::cout << "state ";
      for (std::vector<std::size_t>::const_iterator p = j->begin(); p != j->end(); ++p)
      {
        std::cout << *p << " ";
      }
      std::cout << std::endl;
    }

    for (std::vector<fsm_transition>::const_iterator k =  m_transitions.begin(); k != m_transitions.end(); ++k)
    {
      add_transition(result, *k);
      std::cout << "transition " << k->source() << " " << k->target() << " " << k->label() << std::endl;
    }

    result.set_initial_state(0);
  }
};

inline
void parse_fsm_specification(const std::string& text, lts_fsm_t& result)
{
  core::parser p(parser_tables_fsm);
  unsigned int start_symbol_index = p.start_symbol_index("FSM");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  fsm_actions(parser_tables_fsm).parse_FSM(node, result);
  p.destroy_parse_node(node);
}

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_PARSE_H
