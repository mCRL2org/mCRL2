// Author(s): Muck van Weerdenburg; completely rewritten by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lts/lts_lts.h"

#include <fstream>
#include <string>
#include <cstring>
#include <sstream>

namespace mcrl2
{
namespace lts
{
namespace detail
{

using namespace atermpp;


typedef term_list<state_label_lts> state_labels_t;               // The state labels listed consecutively.
typedef term_list<atermpp::aterm_appl> action_labels_t;          // A multiaction has the shape "multi_action(action_list,data_expression)
typedef term_list<data::data_expression> probabilistic_labels_t; // This contains a list of probabilities.
typedef term_list<data::function_symbol> boolean_list_t;         // A list with constants true or false, indicating
                                                                 // whether a state is probabilistic.

// Special function symbols to indicate the type of written terms.

// A header consisting of the data_specification, the process parameters, the list of action labels, the initial state and the number of states.
static atermpp::function_symbol lts_header()
{
  static atermpp::function_symbol lts_h("labelled_transition_system", 5);
  return lts_h;
}

// A simple transition of three aterm_int indicating the from, label and to states.
static atermpp::function_symbol transition_header()
{
  static atermpp::function_symbol t_h("transition", 3);
  return t_h;
}

// A simple transition of two aterm_int indicating the from state and label, the last argument is a state_probability_list.
static atermpp::function_symbol probabilistic_transition_header()
{
  static atermpp::function_symbol t_h("probabilistic_transition", 3);
  return t_h;
}

static atermpp::function_symbol multi_action_header()
{
  static atermpp::function_symbol ma_h("multi_action", 2);
  return ma_h;
}

// Utility functions

/// \brief Converts a probabilistic state into an aterm that encodes it.
static aterm_list encode_probabilitistic_state(const probabilistic_lts_lts_t::probabilistic_state_t& target)
{
  aterm_list result;
  for(const auto& state : target)
  {
    // Push a (index, probability) pair into the list.
    result.push_front(state.probability());
    result.push_front(aterm_int(state.state()));
  }

  return result;
}

static probabilistic_lts_lts_t::probabilistic_state_t decode_probabilistic_state(const atermpp::aterm_list& list)
{
  // Convert the list into a vector of pairs.
  std::vector<lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>> result;

  for(auto it = list.begin(); it != list.end(); ++it)
  {
    // Read the (index, probability) pair from the list.
    const std::size_t state_number = down_cast<aterm_int>(*it).value();
    ++it;

    // The indices are already added to the header before.
    const lps::probabilistic_data_expression& probability = down_cast<lps::probabilistic_data_expression>(*it);
    result.emplace_back(state_number, probability);
  }

  return probabilistic_lts_lts_t::probabilistic_state_t(result.begin(), result.end());
}

static aterm encode_initial_state(const probabilistic_lts_lts_t& lts)
{
  return encode_probabilitistic_state(lts.initial_probabilistic_state());
}

static aterm encode_initial_state(const lts_lts_t& lts)
{
  return encode_probabilitistic_state(probabilistic_lts_lts_t::probabilistic_state_t(lts.initial_state()));
}

static void decode_initial_state(lts_lts_t& lts, const probabilistic_lts_lts_t::probabilistic_state_t& initial_state)
{
  if (initial_state.size() > 1)
  {
    throw mcrl2::runtime_error("The initial state of the non probabilistic input lts is probabilistic.");
  }

  lts.set_initial_state(initial_state.begin()->state());
}

static void decode_initial_state(probabilistic_lts_lts_t& lts, const probabilistic_lts_lts_t::probabilistic_state_t& initial_state)
{
  lts.set_initial_probabilistic_state(initial_state);
}

static aterm encode_transition(const lts_lts_t& lts, const transition& trans)
{
  return atermpp::aterm_appl(transition_header(),
    aterm_int(trans.from()),
    aterm_int(lts.apply_hidden_label_map(trans.label())),
    aterm_int(trans.to()));
}

static aterm encode_transition(const probabilistic_lts_lts_t& lts, const transition& trans)
{
  const probabilistic_lts_lts_t::probabilistic_state_t& probabilistic_state = lts.probabilistic_state(trans.to());

  if (probabilistic_state.size() == 1)
  {
    // Actually a non_probabilistic transition.
    return atermpp::aterm_appl(transition_header(),
      aterm_int(trans.from()),
      aterm_int(lts.apply_hidden_label_map(trans.label())),
      aterm_int(probabilistic_state.begin()->state()));
  }
  else
  {
    return atermpp::aterm_appl(probabilistic_transition_header(),
      aterm_int(trans.from()),
      aterm_int(lts.apply_hidden_label_map(trans.label())),
      encode_probabilitistic_state(probabilistic_state));
  }

}

static void decode_transition(lts_lts_t& lts, const aterm_appl& appl)
{
  if (appl.function() == probabilistic_transition_header())
  {
    throw mcrl2::runtime_error("Attempting to read a probabilistic lts as a regular lts.");
  }

  lts.add_transition(transition(
    static_cast<const aterm_int&>(appl[0]).value(),
    static_cast<const aterm_int&>(appl[1]).value(),
    static_cast<const aterm_int&>(appl[2]).value()));
}

static void decode_transition(probabilistic_lts_lts_t& lts, const aterm_appl& appl)
{
  std::size_t target_index = 0;

  // Depending on the header its the index of a probabilistic state or an actual probabilistic state encoded as a list.
  if (appl.function() == transition_header())
  {
    auto target = probabilistic_lts_lts_t::probabilistic_state_t(static_cast<const aterm_int&>(appl[2]).value());
    target_index = lts.add_probabilistic_state(target);
  }
  else
  {
    assert(appl.function() == probabilistic_transition_header());
    auto target = decode_probabilistic_state(static_cast<const aterm_list&>(appl[2]));
    target_index = lts.add_probabilistic_state(target);
  }

  lts.add_transition(transition(
    static_cast<const aterm_int&>(appl[0]).value(),
    static_cast<const aterm_int&>(appl[1]).value(),
    target_index));
}

template <class LTS_TRANSITION_SYSTEM>     
static void read_from_lts(LTS_TRANSITION_SYSTEM& lts, const std::string& filename)
{
  static_assert(std::is_same<LTS_TRANSITION_SYSTEM,probabilistic_lts_lts_t>::value || 
                std::is_same<LTS_TRANSITION_SYSTEM,lts_lts_t>::value,
                "Function read_from_lts can only be applied to a (probabilistic) lts. ");

  std::ifstream fstream;
  if (!filename.empty())
  {
    fstream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    try
    {  
      fstream.open(filename, std::ifstream::in | std::ifstream::binary);
    }
    catch (std::ifstream::failure&)
    {
      if (filename.empty())
      {
        throw mcrl2::runtime_error("Fail to open standard input to read an lts.");
      }
      else 
      {
        throw mcrl2::runtime_error("Fail to open file " + filename + " to read an lts.");
      }
    }
  }

  try
  {
    atermpp::binary_aterm_istream stream(filename.empty() ? std::cin : fstream);
    stream >> data::detail::add_index_impl;

    while (true)
    {
      aterm term = stream.get();
      if (!term.defined())
      {
        // The default constructed term indicates the end of the stream.
        break;
      }

      if (term.function() == transition_header() || term.function() == probabilistic_transition_header())
      {
        const aterm_appl& appl = static_cast<const aterm_appl&>(term);
        decode_transition(lts, appl);
      }
      else if (term.function() == multi_action_header())
      {
        const aterm_appl& appl = static_cast<const aterm_appl&>(term);

        const lps::multi_action action = lps::multi_action(
          down_cast<process::action_list>(appl[0]),
          down_cast<data::data_expression>(appl[1]));

        lts.add_action(action_label_lts(action));
      }
      else if (term.function() == atermpp::detail::g_term_pool().as_list())
      {
        // Lists always represent state labels, only need to add the indices.
        lts.add_state(reinterpret_cast<const state_label_lts&>(term));
      }
      else if (term.function() == lts_header())
      {
        const aterm_appl& header= static_cast<const aterm_appl&>(term);

        lts.set_data(data::data_specification(static_cast<const aterm_appl&>(header[0])));
        lts.set_process_parameters(static_cast<const data::variable_list&>(header[1]));
        lts.set_action_label_declarations(static_cast<const process::action_label_list&>(header[2]));

        if (lts.num_state_labels() == 0)
        {
          lts.set_num_states(static_cast<const aterm_int&>(header[4]).value());
        }
        else
        {
          assert(lts.num_states() == lts.num_state_labels());
        }

        // The initial state can only be set after the states are known.
        decode_initial_state(lts, decode_probabilistic_state(static_cast<const aterm_list&>(header[3])));
      }
    }
  }
  catch (std::ifstream::failure&)
  {
    if (filename.empty())
    {
      throw mcrl2::runtime_error("Fail to correctly read an lts from standard input.");
    }
    else
    {
      throw mcrl2::runtime_error("Fail to correctly read an lts from the file " + filename + ".");
    }
  }
}

template <class LTS_TRANSITION_SYSTEM>
static void write_to_lts(const LTS_TRANSITION_SYSTEM& lts, const std::string& filename)
{
  static_assert(std::is_same<LTS_TRANSITION_SYSTEM,probabilistic_lts_lts_t>::value ||
                std::is_same<LTS_TRANSITION_SYSTEM,lts_lts_t>::value,
                "Function read_from_lts can only be applied to a (probabilistic) lts. ");

  std::ofstream fstream;
  if (!filename.empty())
  {
    fstream.exceptions ( std::ofstream::failbit | std::ofstream::badbit );

    try
    {
      fstream.open(filename, std::ofstream::out | std::ofstream::binary);
    }
    catch (std::ofstream::failure&)
    {
      throw mcrl2::runtime_error("Fail to open file " + filename + " for writing.");
    }
  }

  try
  {
    atermpp::binary_aterm_ostream stream(filename.empty() ? std::cout : fstream);
    stream << data::detail::remove_index_impl;

    if (lts.has_state_info())
    {
      for (std::size_t i = 0; i < lts.num_state_labels(); ++i)
      {
        // Write state labels as such, we assume that all list terms without headers are state labels.
        stream << lts.state_label(i);
      }
    }

    if (lts.has_action_info())
    {
      for (std::size_t i = 1; i < lts.num_action_labels(); ++i)
      {
        stream << atermpp::aterm_appl(multi_action_header(), lts.action_label(i).actions(), lts.action_label(i).time());
      }
    }

    for (auto& trans : lts.get_transitions())
    {
      stream << encode_transition(lts, trans);
    }

    // Write the header of the labelled transition system at the end, because the initial state must be set of adding the transitions.
    aterm header = aterm_appl(lts_header(),
                data::detail::data_specification_to_aterm(lts.data()),
                lts.process_parameters(),
                lts.action_label_declarations(),
                encode_initial_state(lts),
                aterm_int(lts.num_states()));

    // Write the header with the indices of dedicated terms removed.
    stream << header;
  }
  catch (std::ofstream::failure&)
  {
    throw mcrl2::runtime_error("Fail to write lts correctly to the file " + filename + ".");
  }
}

} // namespace detail

// Implementation of public functions.

void probabilistic_lts_lts_t::save(const std::string& filename) const
{
  mCRL2log(log::verbose) << "Starting to save a probabilistic lts to the file " << filename << ".\n";
  detail::write_to_lts(*this, filename);
}

void lts_lts_t::save(std::string const& filename) const
{
  mCRL2log(log::verbose) << "Starting to save an lts to the file " << filename << ".\n";
  detail::write_to_lts(*this, filename);
}

void probabilistic_lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load a probabilistic lts from the file " << filename << ".\n";
  detail::read_from_lts(*this, filename);
}

void lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load an lts from the file " << filename << ".\n";
  detail::read_from_lts(*this, filename);
}

}
}
