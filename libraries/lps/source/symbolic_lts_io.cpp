// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/platform.h"

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/symbolic_lts_io.h"

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/data/data_io.h"
#include "mcrl2/symbolic/ldd_stream.h"

using namespace mcrl2;

namespace mcrl2::lps
{

static atermpp::aterm symbolic_labelled_transition_system_mark()
{
  return atermpp::aterm(atermpp::function_symbol("symbolic_labelled_transition_system", 0));
}

std::ostream& operator<<(std::ostream& stream, const symbolic_lts& lts)
{
  std::shared_ptr<utilities::obitstream> bitstream = std::make_shared<utilities::obitstream>(stream);

  atermpp::binary_aterm_ostream aterm_stream(bitstream);
  symbolic::binary_ldd_ostream ldd_stream(bitstream);

  // We set the transformer for all other elements (transitions, state labels and the initial state).
  aterm_stream << data::detail::remove_index_impl;

  aterm_stream << symbolic_labelled_transition_system_mark();
  aterm_stream << lts.data_spec;
  aterm_stream << lts.process_parameters;

  ldd_stream << lts.initial_state;
  ldd_stream << lts.states;

  // Write the mapping from indices to terms.
  for (const auto& index : lts.data_index)
  {
    bitstream->write_integer(index.size());

    for (const auto& term : index)
    {
      aterm_stream << term;
    }
  }

  // Write the action label indices.
  bitstream->write_integer(lts.action_index.size());
  for (const auto& term : lts.action_index)
  {
    aterm_stream << term;
  }

  // Write the transition group information: read and write dependencies and the local transition relation.
  bitstream->write_integer(lts.summand_groups.size());
  for (const auto& group : lts.summand_groups)
  {
    bitstream->write_integer(group.read_parameters.size());
    for (const auto& parameter : group.read_parameters)
    {
      aterm_stream << parameter;
    }

    bitstream->write_integer(group.write_parameters.size());
    for (const auto& parameter : group.write_parameters)
    {
      aterm_stream << parameter;
    }
    ldd_stream << group.L;
  }

  return stream;
}

std::istream& operator>>(std::istream& stream, symbolic_lts& lts)
{
  std::shared_ptr<utilities::ibitstream> bitstream = std::make_shared<utilities::ibitstream>(stream);

  atermpp::binary_aterm_istream aterm_stream(bitstream);
  symbolic::binary_ldd_istream ldd_stream(bitstream);

  // We set the transformer for all other elements (transitions, state labels and the initial state).
  aterm_stream >> data::detail::add_index_impl;

  atermpp::aterm marker;
  aterm_stream >> marker;

  if (marker != symbolic_labelled_transition_system_mark())
  {
    throw mcrl2::runtime_error("Stream does not contain a symbolic labelled transition system (SLTS).");
  }

  aterm_stream >> lts.data_spec;
  aterm_stream >> lts.process_parameters;
  ldd_stream >> lts.initial_state;
  ldd_stream >> lts.states;

  // For every process parameter read the data index.
  lts.data_index.clear();
  for (const data::variable& parameter : lts.process_parameters)
  {
    lts.data_index.emplace_back(parameter.sort());

    std::size_t number_of_entries = bitstream->read_integer();
    for (std::size_t i = 0; i < number_of_entries; ++i)
    {
      data::data_expression value;
      aterm_stream >> value;

      [[ maybe_unused]]
      auto [result, inserted] = lts.data_index.back().insert(value);
    }
  }

  std::size_t number_of_action_labels = bitstream->read_integer();
  lts.action_index.clear();

  for (std::size_t i = 0; i < number_of_action_labels; ++i)
  {
    lps::multi_action value;
    aterm_stream >> value;

    [[ maybe_unused]]
    auto [result, inserted] = lts.action_index.insert(value);
  }

  std::size_t number_of_groups = bitstream->read_integer();
  lts.summand_groups.clear();

  for (std::size_t i = 0; i < number_of_groups; ++i)
  {
    std::size_t number_of_read = bitstream->read_integer();
    data::variable_vector read_parameters = std::vector<data::variable>(number_of_read);

    for (auto& parameter : read_parameters)
    {
      aterm_stream >> parameter;
    }

    std::size_t number_of_write = bitstream->read_integer();
    data::variable_vector write_parameters = std::vector<data::variable>(number_of_write);

    for (auto& parameter : write_parameters)
    {
      aterm_stream >> parameter;
    }

    lts.summand_groups.emplace_back(lts.process_parameters, read_parameters, write_parameters);
    ldd_stream >> lts.summand_groups.back().L;
  }

  return stream;
}

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN
