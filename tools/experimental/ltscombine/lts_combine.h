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
#include "mcrl2/process/detail/alphabet_parse.h"

namespace mcrl2
{

inline
std::pair<std::vector<core::identifier_string_list>, std::vector<core::identifier_string>>parse_comm_set(const std::string& text)
{
  std::pair<std::vector<core::identifier_string_list>, std::vector<core::identifier_string>> result;

  const std::vector<std::string> set_elements = utilities::regex_split(text, "\\s*,\\s*");
  for (const std::string& word: set_elements)
  {
    auto [lhs, rhs] = process::detail::split_arrow(word);
    result.first.emplace_back(process::detail::make_identifier_string_list(process::detail::split_bar(lhs)));
    result.second.emplace_back(rhs);
  }

  return result;
}

inline std::pair<std::vector<core::identifier_string_list>, std::vector<core::identifier_string>> parse_comm_set(
  std::istream& input)
{
  return parse_comm_set(utilities::read_text(input));
}

inline
process::action_name_multiset_list parse_multi_action_name_set(const std::string& text)
{
  const std::vector<std::string> set_elements = utilities::regex_split(text, "\\s*,\\s*");
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
core::identifier_string_list parse_action_name_set(const std::string& text)
{
  const std::vector<std::string> set_elements = utilities::regex_split(text, "\\s*,\\s*");
  return core::identifier_string_list(set_elements.begin(), set_elements.end());
}

inline
core::identifier_string_list parse_action_name_set(std::istream& input)
{
  return parse_action_name_set(utilities::read_text(input));
}

/// \brief Combine two LTSs and apply the comm, block, allow and hide operators.
void combine_lts(const std::vector<lts::lts_lts_t>& ltss,
  const std::vector<core::identifier_string_list>& syncs,
  const std::vector<core::identifier_string>& resulting_actions,
  const core::identifier_string_list& blocks,
  const core::identifier_string_list& hiden,
  const process::action_name_multiset_list& allow,
  const lps::detail::allow_list_cache& allow_cache,
  const std::string& filename,
  bool save_at_end,
  std::size_t nr_of_threads = 1);
} // namespace mcrl2

#endif // MCRL2_LTS_COMBINE_H_
