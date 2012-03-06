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

// N.B. The source and target are 1-based!
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
      std::size_t source = detail::parse_number(source_text);
      std::size_t target = detail::parse_number(target_text);
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

typedef std::vector<std::size_t> fsm_state;

struct fsm_actions: public core::default_parser_actions
{
  fsm_actions(const core::parser_table& table_, lts_fsm_t& fsm_)
    : core::default_parser_actions(table_),
      fsm(fsm_)
  {}

  // The parameters of the FSM
  std::vector<fsm_parameter> m_parameters;

  // Maps labels of the FSM to numbers
  std::map<std::string, std::size_t> m_labels;

  // Where the parse result is stored
  lts_fsm_t& fsm;

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
    fsm.add_state(detail::state_label_fsm(result));
  }

  void parse_StateList(const core::parse_node& node)
  {
    traverse(node, make_visitor(table, "State", boost::bind(&fsm_actions::parse_State, this, _1)));
  }

  void add_transition(const fsm_transition& t)
  {
    std::size_t max = (std::max)(t.source(), t.target());
    if (fsm.num_states() <= max)
    {
      fsm.set_num_states(max, fsm.has_state_info());
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

  void parse_Transition(const core::parse_node& node)
  {
    add_transition(fsm_transition(parse_Source(node.child(0)), parse_Target(node.child(1)), parse_Label(node.child(2))));
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

  void parse_FSM(const core::parse_node& node)
  {
    fsm = lts_fsm_t(); // TODO: add a proper clear() function to lts_fsm_t
    m_parameters.clear();
    m_labels.clear();

    // parse parameters
    parse_ParameterList(node.child(0));
    std::size_t index = 0;
    for (std::vector<fsm_parameter>::const_iterator i = m_parameters.begin(); i != m_parameters.end(); ++i)
    {
      if (i->cardinality() > 0)
      {
        fsm.add_process_parameter(i->name(), i->sort());
        for (std::vector<std::string>::const_iterator j = i->values().begin(); j != i->values().end(); ++j)
        {
          fsm.add_state_element_value(index, *j);
        }
      }
      index++;
    }

    // parse states
    parse_StateList(node.child(2));

    // parse transitions
    parse_TransitionList(node.child(4));

    fsm.set_initial_state(0);
  }
};

inline
void parse_fsm_specification(const std::string& text, lts_fsm_t& result)
{
  core::parser p(parser_tables_fsm);
  unsigned int start_symbol_index = p.start_symbol_index("FSM");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  fsm_actions(parser_tables_fsm, result).parse_FSM(node);
  p.destroy_parse_node(node);
}

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_PARSE_H
