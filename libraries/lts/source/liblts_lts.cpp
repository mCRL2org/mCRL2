// Author(s): Muck van Weerdenburg; completely rewritten by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_io.h"

#include <fstream>

namespace mcrl2::lts
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

static atermpp::function_symbol initial_state_header()
{
  static atermpp::function_symbol ma_h("initial_state", 1);
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

template <class LTS>
static void read_lts(atermpp::aterm_istream& stream, LTS& lts)
{
  stream >> data::detail::add_index_impl;

  // Read the header of the lts.
  data::data_specification spec;
  stream >> spec;
  lts.set_data(spec);

  data::variable_list parameters;
  stream >> parameters;
  lts.set_process_parameters(parameters);

  process::action_label_list action_labels;
  stream >> action_labels;
  lts.set_action_label_declarations(action_labels);

  // An indexed set to keep indices for multi actions.

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
    else if (term.function() == initial_state_header())
    {
      // The initial state can only be set after the states are known.
      decode_initial_state(lts, decode_probabilistic_state(static_cast<const aterm_appl&>(term)[0]));
    }
  }
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
    stream >> lts;
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

template <class LTS>
static void write_lts(atermpp::aterm_ostream& stream, LTS& lts)
{
  stream << data::detail::remove_index_impl;

  // Write the process related information.
  write_lts_header(stream,
   lts.data(),
   lts.process_parameters(),
   lts.action_label_declarations());

  for (auto& trans : lts.get_transitions())
  {
    write_transition(stream, trans.from(), lts.action_label(lts.apply_hidden_label_map(trans.label())), trans.to());
  }

  if (lts.has_state_info())
  {
    for (std::size_t i = 0; i < lts.num_state_labels(); ++i)
    {
      // Write state labels as such, we assume that all list terms without headers are state labels.
      stream << lts.state_label(i);
    }
  }

  // Write the initial state.
  write_initial_state(stream, lts.initial_state());
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
    stream << lts;
  }
  catch (std::ofstream::failure&)
  {
    throw mcrl2::runtime_error("Fail to write lts correctly to the file " + filename + ".");
  }
}

} // namespace detail

// Implementation of public functions.

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, lts_lts_t& lts)
{
  read_lts(stream, lts);
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, probabilistic_lts_lts_t& lts)
{
  read_lts(stream, lts);
  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const lts_lts_t& lts)
{
  detail::write_lts(stream, lts);
  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const probabilistic_lts_lts_t& lts)
{
  detail::write_lts(stream, lts);
  return stream;
}

void write_lts_header(atermpp::aterm_ostream& stream,
  const data::data_specification& data,
  const data::variable_list& parameters,
  const process::action_label_list& action_labels)
{
  // Write the header of the lts.
  stream << data;
  stream << parameters;
  stream << action_labels;
}

void write_transition(atermpp::aterm_ostream& stream, std::size_t from, const process::timed_multi_action& label, std::size_t to)
{
  stream << atermpp::aterm_appl(detail::transition_header(),
    atermpp::aterm_int(from),
    label,
    atermpp::aterm_int(to));
}

void write_transition(atermpp::aterm_ostream& stream, const probabilistic_lts_lts_t& lts, const transition& trans)
{
  const probabilistic_lts_lts_t::probabilistic_state_t& probabilistic_state = lts.probabilistic_state(trans.to());

  if (probabilistic_state.size() == 1)
  {
    // Actually a non probabilistic transition.
    //write_transition(stream, transition(trans.from(), trans.label(), probabilistic_state.begin()->state()));
  }
  else
  {
    stream << atermpp::aterm_appl(detail::probabilistic_transition_header(),
      atermpp::aterm_int(trans.from()),
      atermpp::aterm_int(lts.apply_hidden_label_map(trans.label())),
      detail::encode_probabilitistic_state(probabilistic_state));
  }
}

void write_state_label(atermpp::aterm_ostream& stream, const state_label_lts& label)
{
  // During reading we assume that state labels are the only aterm_list.
  stream << label;
}

void write_initial_state(atermpp::aterm_ostream& stream, std::size_t state)
{
  stream << detail::encode_probabilitistic_state(probabilistic_lts_lts_t::probabilistic_state_t(state));
}

//static aterm encode_initial_state(const lts_lts_t& lts)
//{
//  return encode_probabilitistic_state(probabilistic_lts_lts_t::probabilistic_state_t(lts.initial_state()));
//}

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

} // namespace mcrl2::lts
