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
static aterm_list encode_probabilitistic_state(const probabilistic_lts_lts_t::probabilistic_state_t& target, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
{
  aterm_list result;
  for(const auto& state : target)
  {
    // Push a (index, probability) pair into the list.
    result.push_front(data::detail::remove_index(state.probability(), cache));
    result.push_front(aterm_int(state.state()));
  }

  return result;
}

static probabilistic_lts_lts_t::probabilistic_state_t decode_probabilistic_state(const atermpp::aterm_list& list, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
{
  // Convert the list into a vector of pairs.
  std::vector<lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>> result;

  for(auto it = list.begin(); it != list.end(); ++it)
  {
    const std::size_t state_number = down_cast<aterm_int>(*it).value();
    ++it;

    const lps::probabilistic_data_expression& probability = down_cast<lps::probabilistic_data_expression>(data::detail::add_index(*it, cache));
    result.push_back(lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>(state_number, probability));
  }

  return probabilistic_lts_lts_t::probabilistic_state_t(result.begin(), result.end());
}

static aterm encode_initial_state(const probabilistic_lts_lts_t& lts, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
{
  return encode_probabilitistic_state(lts.initial_probabilistic_state(), cache);
}

static aterm encode_initial_state(const lts_lts_t& lts, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
{
  return encode_probabilitistic_state(probabilistic_lts_lts_t::probabilistic_state_t(lts.initial_state()), cache);
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

static aterm encode_transition(const lts_lts_t& lts, const transition& trans, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>&)
{
  return atermpp::aterm_appl(transition_header(),
    aterm_int(trans.from()),
    aterm_int(lts.apply_hidden_label_map(trans.label())),
    aterm_int(trans.to()));
}

static aterm encode_transition(const probabilistic_lts_lts_t& lts, const transition& trans, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
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
      encode_probabilitistic_state(probabilistic_state, cache));
  }

}

static void decode_transition(lts_lts_t& lts, const aterm_appl& appl, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>&)
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

static void decode_transition(probabilistic_lts_lts_t& lts, const aterm_appl& appl, std::unordered_map<atermpp::aterm_appl, atermpp::aterm>& cache)
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
    auto target = decode_probabilistic_state(static_cast<const aterm_list&>(appl[2]), cache);
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
    // Used for bottom_up_replace, keeping track of already defined terms.
    std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;

    binary_aterm_input stream(filename.empty() ? std::cin : fstream);

    while (true)
    {
      aterm term = stream.read_term();

      if (!term.defined())
      {
        // The default constructed term indicates the end of the stream.
        break;
      }

      if (term.function() == transition_header() || term.function() == probabilistic_transition_header())
      {
        const aterm_appl& appl = static_cast<const aterm_appl&>(term);
        decode_transition(lts, appl, cache);
      }
      else if (term.function() == multi_action_header())
      {
        // Add the indices again, construct a multi_action and add it to the lts.
        aterm_appl appl = down_cast<aterm_appl>(data::detail::add_index(term, cache));

        const lps::multi_action action = lps::multi_action(
          down_cast<process::action_list>(appl[0]),
          down_cast<data::data_expression>(appl[1]));

        lts.add_action(action_label_lts(action));
      }
      else if (term.function() == atermpp::detail::g_term_pool().as_list())
      {
        // Lists always represent state labels, only need to add the indices.
        term = data::detail::add_index(term, cache);
        lts.add_state(reinterpret_cast<const state_label_lts&>(term));
      }
      else if (term.function() == lts_header())
      {
        aterm_appl header = down_cast<aterm_appl>(data::detail::add_index(term, cache));

        lts.set_data(data::data_specification(static_cast<const aterm_appl&>(header[0])));
        lts.set_process_parameters(static_cast<const data::variable_list&>(header[1]));
        lts.set_action_label_declarations(static_cast<const process::action_label_list&>(header[2]));
        decode_initial_state(lts, decode_probabilistic_state(static_cast<const aterm_list&>(header[3]), cache));

        if (lts.num_state_labels() == 0)
        {
          lts.set_num_states(static_cast<const aterm_int&>(header[4]).value());
        }
        else
        {
          assert(lts.num_states() == lts.state_labels().size());
        }
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
    // Used for bottom_up_replace, keeping track of already defined terms.
    std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;

    atermpp::binary_aterm_output stream(filename.empty() ? std::cout : fstream);

    if (lts.has_state_info())
    {
      for (std::size_t i = 0; i < lts.num_state_labels(); ++i)
      {
        // Write state labels as such, we assume that all list terms without headers are state labels.
        stream.write_term(data::detail::remove_index(lts.state_label(i), cache));
      }
    }

    if (lts.has_action_info())
    {
      for (std::size_t i = 1; i < lts.num_action_labels(); ++i)
      {
        stream.write_term(data::detail::remove_index(atermpp::aterm_appl(multi_action_header(), lts.action_label(i).actions(), lts.action_label(i).time()), cache));
      }
    }

    for (auto& trans : lts.get_transitions())
    {
      stream.write_term(encode_transition(lts, trans, cache));
    }

    // Write the header of the labelled transition system at the end, because the initial state must be set of adding the transitions.
    aterm header = aterm_appl(lts_header(),
                data::detail::data_specification_to_aterm(lts.data()),
                lts.process_parameters(),
                lts.action_label_declarations(),
                encode_initial_state(lts, cache),
                aterm_int(lts.num_states()));

    // Write the header with the indices of dedicated terms removed.
    stream.write_term(data::detail::remove_index(header, cache));
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
