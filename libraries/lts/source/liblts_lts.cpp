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

/// \brief Converts a probabilistic state into an aterm that encodes it.
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const probabilistic_lts_lts_t::probabilistic_state_t& state)
{
  atermpp::aterm_list list;
  for(const auto& state : state)
  {
    // Push a (index, probability) pair into the list.
    list.push_front(state.probability());
    list.push_front(atermpp::aterm_int(state.state()));
  }

  stream << list;

  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, probabilistic_lts_lts_t::probabilistic_state_t& state)
{
  atermpp::aterm_list list;
  stream >> list;

  // Convert the list into a vector of pairs.
  std::vector<lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>> result;

  for(auto it = list.begin(); it != list.end(); ++it)
  {
    // Read the (index, probability) pair from the list.
    const std::size_t state_number = atermpp::down_cast<atermpp::aterm_int>(*it).value();
    ++it;

    // The indices are already added to the header before.
    const lps::probabilistic_data_expression& probability = atermpp::down_cast<lps::probabilistic_data_expression>(*it);
    result.emplace_back(state_number, probability);
  }

  state = probabilistic_lts_lts_t::probabilistic_state_t(result.begin(), result.end());
  return stream;
}

namespace detail
{

using namespace atermpp;

// Special terms to indicate the type of the following structure.

static atermpp::aterm transition_mark()
{
  static atermpp::aterm_appl mark(atermpp::function_symbol("transition", 0));
  return mark;
}

static atermpp::aterm probabilistic_transition_mark()
{
  static atermpp::aterm_appl mark(atermpp::function_symbol("probabilistic_transition", 0));
  return mark;
}

static atermpp::aterm initial_state_mark()
{
  static atermpp::aterm_appl mark(atermpp::function_symbol("initial_state", 0));
  return mark;
}

// Utility functions

static void set_initial_state(lts_lts_t& lts, const probabilistic_lts_lts_t::probabilistic_state_t& initial_state)
{
  if (initial_state.size() > 1)
  {
    throw mcrl2::runtime_error("The initial state of the non probabilistic input lts is probabilistic.");
  }

  lts.set_initial_state(initial_state.begin()->state());
}

static void set_initial_state(probabilistic_lts_lts_t& lts, const probabilistic_lts_lts_t::probabilistic_state_t& initial_state)
{
  lts.set_initial_probabilistic_state(initial_state);
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
  mcrl2::utilities::indexed_set<process::timed_multi_action> multi_actions;

  // The initial state is stored and set as last.
  probabilistic_lts_lts_t::probabilistic_state_t initial_state;

  while (true)
  {
    aterm term = stream.get();
    if (!term.defined())
    {
      // The default constructed term indicates the end of the stream.
      break;
    }

    if (term == transition_mark())
    {
      aterm_int from;
      process::timed_multi_action label;
      aterm_int to;

      stream >> from;
      stream >> label;
      stream >> to;

      auto [index, inserted] = multi_actions.insert(label);

      std::size_t target_index = to.value();
      if constexpr (std::is_same<LTS, probabilistic_lts_lts_t>::value)
      {
        // For probabilistic lts it is necessary to add the state first (and use the returned index).
        target_index = lts.add_probabilistic_state(probabilistic_lts_lts_t::probabilistic_state_t(to.value()));
      }

      lts.add_transition(transition(from.value(), index, target_index));

      if (inserted)
      {
        lts.add_action(action_label_lts(lps::multi_action(label.actions(), label.time())));
      }
    }
    else if(term == probabilistic_transition_mark())
    {
      if constexpr (std::is_same<LTS, probabilistic_lts_lts_t>::value)
      {
        aterm_int from;
        process::timed_multi_action label;
        probabilistic_lts_lts_t::probabilistic_state_t to;

        stream >> from;
        stream >> label;
        stream >> to;

        auto [index, inserted] = multi_actions.insert(label);
        std::size_t to_index = lts.add_probabilistic_state(to);
        lts.add_transition(transition(from.value(), index, to_index));

        if (inserted)
        {
          lts.add_action(action_label_lts(lps::multi_action(label.actions(), label.time())));
        }
      }
      else
      {
        throw mcrl2::runtime_error("Attempting to read a probabilistic lts as a regular lts.");
      }
    }
    else if (term.function() == atermpp::detail::g_term_pool().as_list())
    {
      // Lists always represent state labels, only need to add the indices.
      lts.add_state(reinterpret_cast<const state_label_lts&>(term));
    }
    else if (term == initial_state_mark())
    {
      // Read the initial state.
      stream >> initial_state;
    }
  }

  // The initial state can only be set after the states are known.
  set_initial_state(lts, initial_state);
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

void write_initial_state(atermpp::aterm_ostream& stream, const probabilistic_lts_lts_t& lts)
{
  stream << detail::initial_state_mark();
  stream << lts.initial_probabilistic_state();
}

void write_initial_state(atermpp::aterm_ostream& stream, const lts_lts_t& lts)
{
  stream << detail::initial_state_mark();
  stream << probabilistic_lts_lts_t::probabilistic_state_t(lts.initial_state());
}

template <class LTS>
static void write_lts(atermpp::aterm_ostream& stream, LTS& lts)
{
  // Write the process related information.
  write_lts_header(stream,
   lts.data(),
   lts.process_parameters(),
   lts.action_label_declarations());

  for (auto& trans : lts.get_transitions())
  {
    lts_lts_t::action_label_t label = lts.action_label(lts.apply_hidden_label_map(trans.label()));
    write_transition(stream, trans.from(), process::timed_multi_action(label.actions(), label.time()), trans.to());
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
  write_initial_state(stream, lts);
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
  const data::data_specification& data_spec,
  const data::variable_list& parameters,
  const process::action_label_list& action_labels)
{
  // Write the header of the lts.
  stream << data::detail::remove_index_impl;
  stream << data_spec;
  stream << parameters;
  stream << action_labels;
}

void write_transition(atermpp::aterm_ostream& stream, std::size_t from, const process::timed_multi_action& label, std::size_t to)
{
  stream << detail::transition_mark();
  stream << atermpp::aterm_int(from);
  stream << label;
  stream << atermpp::aterm_int(to);
}

void write_transition(atermpp::aterm_ostream& stream, std::size_t from, const process::timed_multi_action& label, const probabilistic_lts_lts_t::probabilistic_state_t& to)
{
  if (to.size() == 1)
  {
    // Actually a non probabilistic transition.
    write_transition(stream, from, label, to);
  }
  else
  {
    stream << detail::probabilistic_transition_mark();
    stream << atermpp::aterm_int(from);
    stream << label;
    stream << to;
  }
}

void write_state_label(atermpp::aterm_ostream& stream, const state_label_lts& label)
{
  // During reading we assume that state labels are the only aterm_list.
  stream << label;
}

void write_initial_state(atermpp::aterm_ostream& stream, std::size_t index)
{
  stream << detail::initial_state_mark();
  stream << probabilistic_lts_lts_t::probabilistic_state_t(index);
}

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
