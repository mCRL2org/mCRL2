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

#include <sstream>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "mcrl2/core/parse.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"
#include "mcrl2/utilities/text_utility.h"

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
    fsm = lts_fsm_t(); // TODO: add a proper clear() function to lts_fsm_t
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
    fsm.add_state(detail::state_label_fsm(values));
  }

  void add_parameter(const std::string& name, const std::string& cardinality, const std::string& sort, const std::vector<std::string>& domain_values)
  {
    parameters.push_back(fsm_parameter(name, cardinality, sort, domain_values));
  }

  void add_parameters()
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

typedef std::vector<std::size_t> fsm_state;

/// \brief Parse actions for FSM format
struct fsm_actions: public core::default_parser_actions
{
  fsm_actions(const core::parser& parser_, lts_fsm_t& fsm)
    : core::default_parser_actions(parser_),
      builder(fsm)
  {}

  fsm_builder builder;

  template <typename T, typename Function>
  std::vector<T> parse_vector(const core::parse_node& node, const std::string& type, Function f)
  {
    std::vector<T> result;
    traverse(node, make_collector(m_parser.symbol_table(), type, result, f));
    return result;
  }

  std::string parse_Id(const core::parse_node& node)
  {
    return node.string();
  }

  std::vector<std::string> parse_IdList(const core::parse_node& node)
  {
    return parse_vector<std::string>(node, "Id", [&](const core::parse_node& node) { return parse_Id(node); });
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
    return parse_vector<std::string>(node, "Number", [&](const core::parse_node& node) { return parse_Number(node); });
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
    return parse_vector<std::string>(node, "QuotedString", [&](const core::parse_node& node) { return parse_QuotedString(node); });
  }

  void parse_Parameter(const core::parse_node& node)
  {
    builder.add_parameter(parse_ParameterName(node.child(0)), parse_DomainCardinality(node.child(1)), parse_SortExpr(node.child(2)), parse_DomainValueList(node.child(3)));
  }

  void parse_ParameterList(const core::parse_node& node)
  {
    traverse(node, make_visitor(m_parser.symbol_table(), "Parameter", [&](const core::parse_node& node) { return parse_Parameter(node); }));
  }

  void parse_State(const core::parse_node& node)
  {
    std::vector<std::string> v = parse_NumberList(node.child(0));
    std::vector<std::size_t> result;
    if (v.size() != builder.parameters.size())
    {
      throw mcrl2::runtime_error("parse_State: wrong number of elements");
    }
    for (auto i = v.begin(); i != v.end(); ++i)
    {
      if (builder.parameters[i - v.begin()].cardinality() != 0)
      {
        result.push_back(detail::parse_number(*i));
      }
    }
    builder.add_state(result);
  }

  void parse_StateList(const core::parse_node& node)
  {
    traverse(node, make_visitor(m_parser.symbol_table(), "State", [&](const core::parse_node& node) { return parse_State(node); }));
  }

  void parse_Transition(const core::parse_node& node)
  {
    builder.add_transition(parse_Source(node.child(0)), parse_Target(node.child(1)), parse_Label(node.child(2)));
  }

  void parse_TransitionList(const core::parse_node& node)
  {
    traverse(node, make_visitor(m_parser.symbol_table(), "Transition", [&](const core::parse_node& node) { return parse_Transition(node); }));
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
    builder.start();

    // parse parameters
    parse_ParameterList(node.child(0));

    builder.add_parameters();

    // parse states
    parse_StateList(node.child(2));

    // parse transitions
    parse_TransitionList(node.child(4));

    builder.finish();
  }
};

class simple_fsm_parser
{
  protected:
    enum states { PARAMETERS, STATES, TRANSITIONS };

    // Maintains the parsing phase
    states state;

    // Used for constructing an FSM
    fsm_builder builder;

    // Used for parsing lines
    boost::xpressive::sregex regex_parameter;
    boost::xpressive::sregex regex_transition;

    states next_state(states state)
    {
      switch (state)
      {
        case PARAMETERS: return STATES;
        case STATES: return TRANSITIONS;
        default: throw mcrl2::runtime_error("Unexpected split line --- encountered while parsing FSM!");
      }
    }

    void parse_parameter(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      boost::xpressive::smatch what;
      if (!boost::xpressive::regex_match(line, what, regex_parameter))
      {
        throw mcrl2::runtime_error("could not parse the following line as an FSM parameter: " + text);
      }
      std::string name = what[1];
      std::string cardinality = what[2];
      std::string sort = what[3];
      std::vector<std::string> domain_values = utilities::regex_split(what[4], "\\s+");
      builder.add_parameter(name, cardinality, sort, domain_values);
    }

    void parse_state(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      std::vector<std::string> values = utilities::regex_split(text, "\\s+");
      std::vector<std::size_t> numbers;
      for (const std::string& value: values)
      {
        try
        {
          numbers.push_back(boost::lexical_cast<std::size_t>(value));
        }
        catch (boost::bad_lexical_cast&)
        {
          throw mcrl2::runtime_error("could not parse the following line as an FSM state: " + text);
        }
      }
      builder.add_state(numbers);
    }

    void parse_transition(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      boost::xpressive::smatch what;
      if (!boost::xpressive::regex_match(line, what, regex_transition))
      {
        throw mcrl2::runtime_error("could not parse the following line as an FSM parameter: " + text);
      }
      std::string source = what[1];
      std::string target = what[3];
      std::string label = what[5];
      builder.add_transition(source, target, label);
    }

  public:
    simple_fsm_parser(lts_fsm_t& fsm)
      : builder(fsm)
    {
      // \s*([a-zA-Z_][a-zA-Z0-9_'@]*)\((\d+)\)\s*([a-zA-Z_][a-zA-Z0-9_'@#\-> \t=,\\(\\):]*)?\s*((\"[^\"]*\"\s*)*)
      regex_parameter = boost::xpressive::sregex::compile("\\s*([a-zA-Z_][a-zA-Z0-9_'@]*)\\((\\d+)\\)\\s*([a-zA-Z_][a-zA-Z0-9_'@#\\-> \\t=,\\\\(\\\\):]*)?\\s*((\\\"[^\\\"]*\\\"\\s*)*)");

      // (0|([1-9][0-9]*))+\s+(0|([1-9][0-9]*))+\s+"([^"]*)"
      regex_transition = boost::xpressive::sregex::compile("(0|([1-9][0-9]*))+\\s+(0|([1-9][0-9]*))+\\s+\"([^\"]*)\"");
    }

    void run(std::istream& from)
    {
      builder.start();

      states state = PARAMETERS;
      std::string line;
      while (std::getline(from, line))
      {
        utilities::trim(line);
        if (line == "---")
        {
          state = next_state(state);
          continue;
        }
        switch (state)
        {
          case PARAMETERS: { parse_parameter(line); break; }
          case STATES: { parse_state(line); break; }
          case TRANSITIONS: { parse_transition(line); break; }
        }
      }
      builder.finish();
    }
};

} // namespace detail

inline
void parse_fsm_specification(const std::string& text, lts_fsm_t& result)
{
  core::parser p(parser_tables_fsm);
  unsigned int start_symbol_index = p.start_symbol_index("FSM");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  detail::fsm_actions(p, result).parse_FSM(node);
  p.destroy_parse_node(node);
}

inline
void parse_fsm_specification(std::istream& in, lts_fsm_t& result)
{
  std::string text = utilities::read_text(in);
  parse_fsm_specification(text, result);
}

inline
void parse_fsm_specification_simple(std::istream& from, lts_fsm_t& result)
{
  detail::simple_fsm_parser fsm_parser(result);
  fsm_parser.run(from);
}

inline
void parse_fsm_specification_simple(const std::string& text, lts_fsm_t& result)
{
  std::istringstream is(text);
  parse_fsm_specification_simple(is, result);
}

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_PARSE_H
