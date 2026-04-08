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
std::vector<core::identifier_string_list> parse_multi_action_name_set(const std::string& text)
{
  std::vector<core::identifier_string_list> result;
  const std::vector<std::string> set_elements = utilities::regex_split(text, "\\s*,\\s*");
  for (const std::string& word: set_elements)
  {
    result.emplace_back(process::detail::make_identifier_string_list(process::detail::split_bar(word)));
  }
  return result;
}

inline
std::vector<core::identifier_string_list> parse_multi_action_name_set(std::istream& input)
{
  return parse_multi_action_name_set(utilities::read_text(input));
}

inline
std::vector<core::identifier_string> parse_action_name_set(const std::string& text)
{
  std::vector<core::identifier_string> result;
  const std::vector<std::string> set_elements = utilities::regex_split(text, "\\s*,\\s*");
  for (const std::string& word: set_elements)
  {
    result.emplace_back(word);
  }
  return result;
}

inline
std::vector<core::identifier_string> parse_action_name_set(std::istream& input)
{
  return parse_action_name_set(utilities::read_text(input));
}

/// \brief Combine two LTSs and apply the comm, block, allow and hide operators.
void combine_lts(const std::vector<lts::lts_lts_t> & ltss,
const std::vector<core::identifier_string_list> & syncs,
const std::vector<core::identifier_string> & resulting_actions,
const std::vector<core::identifier_string> & blocks,
const std::vector<core::identifier_string> & hiden,
const std::vector<core::identifier_string_list> & allow,
const std::string& filename,
bool save_at_end,
std::size_t nr_of_threads = 1);
} // namespace mcrl2

#endif // MCRL2_LTS_COMBINE_H_
