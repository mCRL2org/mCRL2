// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_combine.h
/// \brief The combine_lts function combines two LTSs and applies
/// the comm, block, allow and hide operators in that order.

#ifndef MCRL2_LTS_COMBINE_H_
#define MCRL2_LTS_COMBINE_H_

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/lps/linearise_allow_block.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/process/action_label.h"
#include "mcrl2/process/communication_expression.h"
#include "mcrl2/process/detail/alphabet_parse.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2
{

inline process::communication_expression_list parse_comm_set(std::istream& input)
{
  return process::detail::parse_comm_set(utilities::read_text(input));
}

inline
process::action_name_multiset_list parse_multi_action_name_set(const std::string& text)
{
  const std::vector<std::string> set_elements = process::detail::set_elements(text);
  return process::action_name_multiset_list(
    set_elements.begin(),
    set_elements.end(),
    [](const std::string& word)
    {
        return process::action_name_multiset(process::detail::make_identifier_string_list(process::detail::split_bar(word)));
    });
}

inline
process::action_name_multiset_list parse_multi_action_name_set(std::istream& input)
{
  return parse_multi_action_name_set(utilities::read_text(input));
}

inline
core::identifier_string_list parse_block_set(std::istream& input)
{
  return process::detail::parse_block_set(utilities::read_text(input));
}

inline core::identifier_string_list parse_hide_set(std::istream& input)
{
  return process::detail::parse_hide_set(utilities::read_text(input));
}

struct combine_lts_input
{
  const std::vector<lts::lts_lts_t>& ltss;
  const process::communication_expression_list& comm_set;
  const core::identifier_string_list& blocks;
  const core::identifier_string_list& hiden;
  const lps::detail::allow_list_cache& allow_cache;
  const std::string& filename;
  bool save_at_end;
  std::size_t nr_of_threads = 1;
};

/// \brief Combine two LTSs and apply the comm, block, allow and hide operators.
void combine_lts(const combine_lts_input& input);
} // namespace mcrl2

#endif // MCRL2_LTS_COMBINE_H_
