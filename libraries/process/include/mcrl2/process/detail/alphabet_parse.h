// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_parse.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_PARSE_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_PARSE_H

#include "mcrl2/process/parse.h"

namespace mcrl2 {

namespace process {

namespace detail {

multi_action_name parse_multi_action_name(const std::string& text)
{
  multi_action_name result;
  for (auto i = text.begin(); i != text.end(); ++i)
  {
    result.insert(core::identifier_string(std::string(1, *i)));
  }
  return result;
}

std::pair<multi_action_name_set, bool> parse_multi_action_name_set(const std::string& text)
{
  multi_action_name_set result;
  bool result_includes_subsets = boost::algorithm::ends_with(text, "@");

  // remove {}@
  std::string s = utilities::regex_replace("[{}@]", "", text);

  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    result.insert(parse_multi_action_name(*i));
  }
  return std::make_pair(result, result_includes_subsets);
}

action_name_multiset_list parse_allow_set(const std::string& text)
{
  std::vector<action_name_multiset> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    std::string word = utilities::regex_replace("\\s*\\|\\s*", "", *i);
    multi_action_name alpha = parse_multi_action_name(word);
    result.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
  }
  return action_name_multiset_list(result.begin(), result.end());
}

core::identifier_string_list parse_block_set(const std::string& text)
{
  std::vector<core::identifier_string> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    result.push_back(core::identifier_string(*i));
  }
  return core::identifier_string_list(result.begin(), result.end());
}

communication_expression_list parse_comm_set(const std::string& text)
{
  std::vector<communication_expression> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    std::vector<std::string> w = utilities::regex_split(*i, "\\s*->\\s*");
    std::string lhs = utilities::regex_replace("\\s*\\|\\s*", "", w[0]);
    std::string rhs = w[1];
    multi_action_name beta = parse_multi_action_name(lhs);
    core::identifier_string_list alpha(beta.begin(), beta.end());
    core::identifier_string a(rhs);
    result.push_back(communication_expression(alpha, a));
  }
  return communication_expression_list(result.begin(), result.end());
}

rename_expression_list parse_rename_set(const std::string& text)
{
  std::vector<rename_expression> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    std::vector<std::string> w = utilities::regex_split(*i, "\\s*->\\s*");
    result.push_back(rename_expression(core::identifier_string(w[0]), core::identifier_string(w[1])));
  }
  return rename_expression_list(result.begin(), result.end());
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PARSE_H
