// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/parse.h
/// \brief A simple straighforward parser for .fsm files.

#ifndef MCRL2_LTS_PARSE_H
#define MCRL2_LTS_PARSE_H

#include <regex>
#include "mcrl2/lts/detail/fsm_builder.h"

namespace mcrl2::lts {

namespace detail {

class simple_fsm_parser
{
  protected:
    enum states { PARAMETERS, STATES, TRANSITIONS, INITIAL_DISTRIBUTION };

    // Used for constructing an FSM
    detail::fsm_builder builder;

    const std::regex regex_parameter {"\\s*([a-zA-Z_][a-zA-Z0-9_'@]*)\\((\\d+)\\)\\s*([a-zA-Z_][a-zA-Z0-9_'@#\\-> \t=,\\(\\):]*)?\\s*((\"[^\"]*\"\\s*)*)"};

    // Probabilistic transitions can have one of the following two forms.
    //        in out "label"
    //        in [out1 prob1 ... out_n prob_n] "label"
    // where out is a state number of a state that can be reached with probability 1 in the first format, whereas
    // it is precisely indicated what the probabilities of the reachable states are in the second form.
    const std::regex regex_transition {"(0|([1-9][0-9]*))+\\s+(0|([1-9][0-9]*|\\[[^\\]]*\\]))+\\s+\"([^\"]*)\""};

    const std::regex regex_quoted_string {"\"([^\"]*)\""};

    // The initial state, is either a number or it has the shape [num1 prob1 ... num_n prob_n].
    const std::regex regex_probabilistic_initial_distribution{R"(0|([1-9][0-9]*)|\[[^\]]*\])"};

    states next_state(states state)
    {
      switch (state)
      {
        case PARAMETERS:
        {
          builder.write_parameters();
          return STATES;
        }
        case STATES: return TRANSITIONS;
        case TRANSITIONS: return INITIAL_DISTRIBUTION;
        default: throw mcrl2::runtime_error("unexpected split line --- encountered while parsing FSM!");
      }
    }

    std::vector<std::string> parse_domain_values(const std::string& text)
    {
      std::vector<std::string> result;

      std::regex_token_iterator cur{text.begin(), text.end(), regex_quoted_string};
      std::regex_token_iterator<std::string::const_iterator> end;
      for (; cur != end; ++cur)
      {
        std::string value = cur->str();
        if (!value.empty())
        {
          result.push_back(value.substr(1, value.size() - 2));
        }
      }

      return result;
    }

    void parse_parameter(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      std::smatch what;
      if (!std::regex_match(line, what, regex_parameter))
      {
        throw mcrl2::runtime_error("could not parse the following line as an FSM parameter: " + text);
      }
      std::string name = what[1];
      std::string cardinality = what[2];
      std::string sort = utilities::trim_copy(what[3]);
      std::vector<std::string> domain_values = parse_domain_values(what[4]);
      builder.add_parameter(name, cardinality, sort, domain_values);
    }

    void parse_state(const std::string& line)
    {
      try
      {
        std::vector<std::size_t> values = utilities::parse_natural_number_sequence(line);
        builder.add_state(values);
      }
      catch (mcrl2::runtime_error&)
      {
        throw mcrl2::runtime_error("could not parse the following line as an FSM state: " + line);
      }
    }

    void parse_transition(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      std::smatch what;
      if (!std::regex_match(line, what, regex_transition))
      {
        throw mcrl2::runtime_error("could not parse the following line as an FSM transition: " + text);
      }
      std::string source = what[1];
      std::string target = what[3];
      std::string label = what[5];
      builder.add_transition(source, target, label);
    }

    void parse_initial_distribution(const std::string& line)
    {
      std::string text = utilities::trim_copy(line);
      std::smatch what;
      if (!std::regex_match(line, what, regex_probabilistic_initial_distribution))
      {
        throw mcrl2::runtime_error("Could not parse the following line as an initial distribution: " + line + ".");
      }
      builder.add_initial_distribution(what[0]);
    }

  public:
    explicit simple_fsm_parser(probabilistic_lts_fsm_t& fsm)
      : builder(fsm)
    {}

    void run(std::istream& from)
    {
      builder.start();

      states state = PARAMETERS;
      std::string line;
      while (std::getline(from, line))
      {
        utilities::trim(line);
        if (!line.empty())  // This deals with the case when there are empty lines in or at the end of the file.
        { 
          if (line == "---")
          {
            state = next_state(state);
          }
          else 
          { 
            switch (state)
            {
              case PARAMETERS: { parse_parameter(line); break; }
              case STATES: { parse_state(line); break; }
              case TRANSITIONS: { parse_transition(line); break; }
              case INITIAL_DISTRIBUTION: { parse_initial_distribution(line); break; }
            }
          }
        }
      }
      builder.finish();
    }
};

} // namespace detail

inline
void parse_fsm_specification(std::istream& from, probabilistic_lts_fsm_t& result)
{
  detail::simple_fsm_parser fsm_parser(result);
  fsm_parser.run(from);
}

inline
void parse_fsm_specification(const std::string& text, probabilistic_lts_fsm_t& result)
{
  std::istringstream is(text);
  parse_fsm_specification(is, result);
}

} // namespace mcrl2::lts

#endif // MCRL2_LTS_PARSE_H
