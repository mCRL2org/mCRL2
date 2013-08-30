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

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include "mcrl2/core/parse.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

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

/// \brief Parse actions for FSM format
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
    return boost::algorithm::trim_copy(result);
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

inline
void parse_fsm_specification(std::istream& in, lts_fsm_t& result)
{
  std::string text = utilities::read_text(in);
  parse_fsm_specification(text, result);
}

struct dot_attrs
{
  dot_attrs() : has_label(false) {}
  bool has_label;
  std::string label;
};

struct dot_actions: public core::default_parser_actions
{
  dot_actions(const core::parser_table& table_, lts_dot_t& dot_)
    : core::default_parser_actions(table_),
      dot(dot_)
  {
    default_node_label.push("");
    default_edge_label.push("");
  }

  lts_dot_t& dot;
  std::map<std::string, std::size_t> labelTable;
  std::map<std::string, std::size_t> stateTable;
  std::vector<std::string> state_sequence;
  std::stack<std::string> default_node_label;
  std::stack<std::string> default_edge_label;

  std::size_t dot_state(const std::string& id, const std::string* label=NULL)
  {
    using namespace mcrl2::lts::detail;
    std::size_t idx;

    std::map<std::string, std::size_t>::const_iterator state_index = stateTable.find(id);
    if (state_index == stateTable.end())
    {
      if (label)
      {
        idx = dot.add_state(state_label_dot(id,*label));
      }
      else
      {
        idx = dot.add_state(state_label_dot(id,default_node_label.top()));
      }
      stateTable[id] = idx;
    }
    else
    {
      idx = state_index->second;
      if (label)
      {
        dot.set_state_label(idx, state_label_dot(id,*label));
      }
    }
    return idx;
  }

  void add_transition(std::size_t from, const std::string &label_string, std::size_t to)
  {
    std::map <std::string, size_t>::const_iterator label_index = labelTable.find(label_string);
    if (label_index == labelTable.end())
    {
      const lts_dot_t::labels_size_type n = dot.add_action(label_string, label_string == "tau");
      labelTable[label_string] = n;
      dot.add_transition(transition(from, n, to));
    }
    else
    {
      dot.add_transition(transition(from, label_index->second, to));
    }
  }

  void add_transitions(const std::vector<std::string>& state_sequence, const std::string &label)
  {
    assert(!state_sequence.empty());
    std::vector <std::string>::const_iterator i = state_sequence.begin();
    std::string from_string = *i;
    for (++i; i!=state_sequence.end(); ++i)
    {
      const std::string to_string = *i;
      add_transition(dot_state(from_string), label, dot_state(to_string));
      from_string = to_string;
    }
  }

  std::string parse_ID(const core::parse_node& node)
  {
    std::string result = node.string();
    if (result.size() >= 2 && result[0] == '"')
    {
      result = result.substr(1, result.size() - 2);
    }
    return result;
  }

  void parse_graph(const core::parse_node& node)
  {
    std::string id = parse_ID(node.child(2));
    parse_stmt_list(node.child(4));
  }

  void parse_subgraph(const core::parse_node& node)
  {
    default_edge_label.push(default_edge_label.top());
    default_node_label.push(default_node_label.top());
    parse_stmt_list(node.child(2));
    default_edge_label.pop();
    default_node_label.pop();
  }

  void parse_stmt(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "node_stmt")) { parse_node_stmt(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "edge_stmt")) { parse_edge_stmt(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "attr_stmt")) { parse_attr_stmt(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "attribute")) { /* Ignore, we currently have no (sub)graph attributes. */ }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "subgraph")) { parse_subgraph(node.child(0)); }
    else
    {
      report_unexpected_node(node);
    }
  }

  void parse_stmt_list(const core::parse_node& node)
  {
    traverse(node, make_visitor(table, "stmt", boost::bind(&dot_actions::parse_stmt, this, _1)));
  }

  void parse_attribute(const core::parse_node& node, dot_attrs& attrs)
  {
    std::string name = parse_ID(node.child(0));
    std::string value = parse_ID(node.child(2));
    if (name == "label")
    {
      attrs.has_label = true;
      attrs.label = value;
    }
  }

  void parse_attr_list(const core::parse_node& node, dot_attrs& attrs)
  {
    if (node.child(1))
    {
      parse_a_list(node.child(1).child(0), attrs);
    }
    if (node.child(3).child(0))
    {
      parse_attr_list(node.child(3).child(0), attrs);
    }
  }

  void parse_a_list(const core::parse_node& node, dot_attrs& attrs)
  {
    parse_attribute(node.child(0), attrs);
    if (node.child(2).child(0))
    {
      parse_a_list(node.child(2).child(0), attrs);
    }
  }

  void parse_attr_stmt(const core::parse_node& node)
  {
    dot_attrs attrs;
    parse_attr_list(node.child(1), attrs);
    if (symbol_name(node.child(0).child(0)) == "node" && attrs.has_label)
    {
      default_node_label.top() = attrs.label;
    }
    else
    if (symbol_name(node.child(0).child(0)) == "edge" && attrs.has_label)
    {
      default_edge_label.top() = attrs.label;
    }
  }

  // Collect all node_id's, and use the optional attr_list as label.
  void parse_edge_stmt(const core::parse_node& node)
  {
    dot_attrs attrs;
    state_sequence.clear();
    traverse(node, make_visitor(table, "node_id", boost::bind(&dot_actions::parse_node_id, this, _1)));
    if (node.child(2).child(0))
    {
      parse_attr_list(node.child(2).child(0), attrs);
    }
    std::string label = attrs.has_label ? attrs.label : default_edge_label.top();
    add_transitions(state_sequence, label);
  }

  void parse_node_stmt(const core::parse_node& node)
  {
    dot_attrs attrs;
    parse_node_id(node.child(0));
    if (node.child(1).child(0))
    {
      parse_attr_list(node.child(1).child(0), attrs);
    }
    std::string label = attrs.has_label ? attrs.label : default_node_label.top();
    dot_state(state_sequence.back(), &label);
  }

  void parse_node_id(const core::parse_node& node)
  {
    state_sequence.push_back(parse_ID(node.child(0)));
  }
};

inline
void parse_dot_specification(const std::string& text, lts_dot_t& result)
{
  result = lts_dot_t();
  core::parser p(parser_tables_dot);
  unsigned int start_symbol_index = p.start_symbol_index("dot");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  dot_actions(parser_tables_dot, result).parse_graph(node);
  p.destroy_parse_node(node);
  result.set_initial_state(0);
}

inline
void parse_dot_specification(std::istream& in, lts_dot_t& result)
{
  std::string text = utilities::read_text(in);
  parse_dot_specification(text, result);
}

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_PARSE_H
