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

// Removes surrounding braces from a string.
inline
std::string remove_braces(const std::string& text)
{
  std::string s = utilities::trim_copy(text);
  if (s.size() < 2 || s.front() != '{' || s.back() != '}')
  {
    throw mcrl2::runtime_error("Expected braces around string: " + text);
  }
  s = s.substr(1, s.size() - 2);
  return utilities::trim_copy(s);
}

// Removes a trailing @ symbol from a string, if it exists.
// Returns the resulting string, and a boolean indicating if an @ symbol was removed.
inline
std::pair<std::string, bool> remove_trailing_at_symbol(const std::string& text)
{
  std::string s = utilities::trim_copy(text);
  bool found_at_symbol = false;
  if (!s.empty() && s.back() == '@')
  {
    found_at_symbol = true;
    s.erase(s.size() - 1);
  }
  return { s, found_at_symbol };
}

// Returns the elements of a comma separated list inside braces.
// For example "{ s1, s2, s3 }" -> [s1, s2, s3]
inline
std::vector<std::string> set_elements(const std::string& text)
{
  std::string s = remove_braces(text);
  return utilities::regex_split(s, "\\s*,\\s*");
}

// Returns the left and right hand sides of strings separated by "->".
// For example "s1 -> s2" -> [s1, s2]
inline
std::pair<std::string, std::string> split_arrow(const std::string& text)
{
  std::string s = utilities::trim_copy(text);
  std::vector<std::string> w = utilities::regex_split(s, "\\s*->\\s*");
  if (w.size() != 2)
  {
    throw mcrl2::runtime_error("Expected a string of the shape lhs -> rhs: " + text);
  }
  return { w[0], w[1] };
}

// Returns the elements of a bar separated list.
// For example "s1 | s2 | s3" -> [s1, s2, s3]
inline
std::vector<std::string> split_bar(const std::string& text)
{
  std::string s = utilities::trim_copy(text);
  return utilities::regex_split(s, "\\s*\\|\\s*");
}

// Splits a word into characters.
inline
std::vector<std::string> split_characters(const std::string& text)
{
  std::string s = utilities::trim_copy(text);
  std::vector<std::string> result;
  for (auto i = s.begin(); i != s.end(); ++i)
  {
    result.push_back(std::string(1, *i));
  }
  return result;
}

// Converts a vector of strings into an identifier_string_list.
inline
core::identifier_string_list make_identifier_string_list(const std::vector<std::string>& words)
{
  std::vector<core::identifier_string> ids;
  for (const std::string& word: words)
  {
    ids.push_back(core::identifier_string(word));
  }
  return core::identifier_string_list(ids.begin(), ids.end());
}

// Converts a list of strings into a multi action name.
inline
multi_action_name make_multi_action(const std::vector<std::string>& words)
{
  multi_action_name result;
  for (const std::string& word: words)
  {
    result.insert(core::identifier_string(word));
  }
  return result;
}

// Parses a multi action name.
// Example: "s1 | s2 | s3" -> {s1, s2, s3}.
inline
multi_action_name parse_multi_action_name(const std::string& text)
{
  return make_multi_action(split_bar(text));
}

// Parses a multi action name set.
// Example: "{ s1 | s2 | s3, t1 | t2 }" -> {{s1, s2, s3}, {t2, t2}}
multi_action_name_set parse_multi_action_name_set(const std::string& text)
{
  std::vector<multi_action_name> result;
  for (const std::string& word: set_elements(text))
  {
    result.push_back(parse_multi_action_name(word));
  }
  return multi_action_name_set(result.begin(), result.end());
}

// Parses a multi action name. It is assumed that the actions consist of one character.
// Example: "abc" -> {a, b, c}.
inline
multi_action_name parse_simple_multi_action_name(const std::string& text)
{
  return make_multi_action(split_characters(text));
}

// Parses a multi action name set, optionally followed by an @ symbol.
// It is assumed that the actions consist of one character.
// Example: "{ abc, de }@" -> [{{a, b, c}, {d, e}}, true].
inline
std::pair<multi_action_name_set, bool> parse_simple_multi_action_name_set(const std::string& text)
{
  multi_action_name_set result;
  auto p = remove_trailing_at_symbol(text);
  std::string s = p.first;
  bool result_includes_subsets = p.second;

  for (const std::string& word: set_elements(s))
  {
    result.insert(parse_simple_multi_action_name(word));
  }
  return { result, result_includes_subsets };
}

inline
action_name_multiset_list parse_allow_set(const std::string& text)
{
  std::vector<action_name_multiset> result;
  for (const std::string& word: set_elements(text))
  {
    result.push_back(action_name_multiset(make_identifier_string_list(split_bar(word))));
  }
  return action_name_multiset_list(result.begin(), result.end());
}

inline
core::identifier_string_list parse_block_set(const std::string& text)
{
  return make_identifier_string_list(set_elements(text));
}

inline
communication_expression_list parse_comm_set(const std::string& text)
{
  std::vector<communication_expression> result;
  for (const std::string& word: set_elements(text))
  {
    std::string lhs;
    std::string rhs;
    std::tie(lhs, rhs) = split_arrow(word);
    result.push_back(communication_expression(make_identifier_string_list(split_bar(lhs)), core::identifier_string(rhs)));
  }
  return communication_expression_list(result.begin(), result.end());
}

inline
rename_expression_list parse_rename_set(const std::string& text)
{
  std::vector<rename_expression> result;
  for (const std::string& word: set_elements(text))
  {
    std::string lhs;
    std::string rhs;
    std::tie(lhs, rhs) = split_arrow(word);
    result.push_back(rename_expression(core::identifier_string(lhs), core::identifier_string(rhs)));
  }
  return rename_expression_list(result.begin(), result.end());
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_PARSE_H
