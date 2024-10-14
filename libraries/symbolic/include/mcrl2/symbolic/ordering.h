// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_ORDERING_H
#define MCRL2_SYMBOLIC_ORDERING_H

#include "mcrl2/data/variable.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/parse_numbers.h"
#include "mcrl2/utilities/text_utility.h"

#include <boost/dynamic_bitset.hpp>

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <random>
#include <regex>

namespace mcrl2::symbolic
{

inline
std::vector<std::set<std::size_t>> parse_summand_groups(std::string text, std::size_t n)
{
  using utilities::regex_split;
  using utilities::trim_copy;
  using utilities::detail::as_set;

  // check if the format of the group is correct
  std::string group_format = R"(\s*\d+(\s+\d+)*\s*)";
  if (!std::regex_match(text, std::regex(group_format + "(;" + group_format + ")*")))
  {
    throw mcrl2::runtime_error("The format of the groups '" + text + "' is incorrect.");
  }

  std::vector<std::set<std::size_t>> result;
  text = trim_copy(text);
  for (const std::string& numbers: regex_split(text, "\\s*;\\s*"))
  {
    result.push_back(as_set(utilities::parse_natural_number_sequence(numbers)));
  }

  // check if result is a partition of [0 .. n)
  std::set<std::size_t> all;
  std::set<std::size_t> expected;
  for (std::size_t i = 0; i < n; i++)
  {
    expected.insert(i);
  }
  for (const auto& group: result)
  {
    std::size_t size = all.size();
    all.insert(group.begin(), group.end());
    if (all.size() != size + group.size())
    {
      throw mcrl2::runtime_error("The groups in '" + text + "' are not disjoint.");
    }
  }
  if (all != expected)
  {
    throw mcrl2::runtime_error("The groups '" + text + "' do not form a partition of [0 .. " + std::to_string(n-1) + "].");
  }

  return result;
}

// puts every summand in a separate group
inline
std::vector<std::set<std::size_t>> compute_summand_groups_default(const std::vector<boost::dynamic_bitset<>>& patterns)
{
  std::vector<std::set<std::size_t>> result;
  for (std::size_t i = 0; i < patterns.size(); i++)
  {
    result.push_back({i});
  }
  return result;
}

// joins summands that depend on the same parameters
inline
std::vector<std::set<std::size_t>> compute_summand_groups_used(const std::vector<boost::dynamic_bitset<>>& patterns)
{
  std::map<boost::dynamic_bitset<>, std::set<std::size_t>> group_map;
  for (std::size_t i = 0; i < patterns.size(); i++)
  {
    // Only keep track of whether the parameter is used (either read or write).
    boost::dynamic_bitset<> pattern = patterns[i];
    boost::dynamic_bitset<> new_pattern = pattern;
    for (std::size_t j = 0; j < pattern.size(); j+=2)
    {
      if (pattern[j] || pattern[j+1])
      {
        new_pattern[j] = 1;
        new_pattern[j+1] = 1;
      }
    }

    group_map[new_pattern].insert(i);
  }

  std::vector<std::set<std::size_t>> groups;
  for (const auto& [_, group]: group_map)
  {
    groups.push_back(group);
  }
  return groups;
}

// joins summands with exactly the same pattern
inline
std::vector<std::set<std::size_t>> compute_summand_groups_simple(const std::vector<boost::dynamic_bitset<>>& patterns)
{
  std::map<boost::dynamic_bitset<>, std::set<std::size_t>> group_map;
  for (std::size_t i = 0; i < patterns.size(); i++)
  {
    group_map[patterns[i]].insert(i);
  }
  std::vector<std::set<std::size_t>> groups;
  for (const auto& [_, group]: group_map)
  {
    groups.push_back(group);
  }
  return groups;
}

inline
std::vector<std::set<std::size_t>> compute_summand_groups(const std::string& text, const std::vector<boost::dynamic_bitset<>>& patterns)
{
  if (text == "none")
  {
    return compute_summand_groups_default(patterns);
  }
  else if (text == "used")
  {
    return compute_summand_groups_used(patterns);
  }
  else if (text == "simple")
  {
    return compute_summand_groups_simple(patterns);
  }
  else
  {
    return parse_summand_groups(text, patterns.size());
  }
}

inline
std::vector<std::size_t> compute_variable_order_default(std::size_t n)
{
  std::vector<std::size_t> result;
  for (std::size_t i = 0; i < n; i++)
  {
    result.push_back(i);
  }
  return result;
}

inline
std::vector<std::size_t> compute_variable_order_random(std::size_t n, bool exclude_first_variable = false)
{
  std::vector<std::size_t> result;
  for (std::size_t i = 0; i < n; i++)
  {
    result.push_back(i);
  }
  std::random_device rd;
  std::mt19937 g(rd());
  std::size_t first = exclude_first_variable ? 1 : 0;
  std::shuffle(result.begin() + first, result.end(), g);
  return result;
}

inline
std::vector<std::size_t> parse_variable_order(std::string text, std::size_t n, bool exclude_first_variable = false)
{
  using utilities::trim_copy;
  using utilities::detail::as_set;

  // check if the format of the order is correct
  std::string format = R"(\s*\d+(\s+\d+)*\s*)";
  if (!std::regex_match(text, std::regex(format)))
  {
    throw mcrl2::runtime_error("The format of the variable order '" + text + "' is incorrect.");
  }

  text = trim_copy(text);
  std::vector<std::size_t> result = utilities::parse_natural_number_sequence(trim_copy(text));

  std::set<std::size_t> expected;
  for (std::size_t i = 0; i < n; i++)
  {
    expected.insert(i);
  }
  if (as_set(result) != expected)
  {
    throw mcrl2::runtime_error("The variable order '" + text + "' is not a permutation of [0 .. " + std::to_string(n-1) + "].");
  }

  if (exclude_first_variable && result[0] != 0)
  {
    throw mcrl2::runtime_error("The variable order '" + text + "' does not start with a zero.");
  }
  return result;
}

inline
std::vector<std::size_t> compute_variable_order(const std::string& text, std::size_t number_of_variables, bool exclude_first_variable = false)
{
  if (text == "none")
  {
    return compute_variable_order_default(number_of_variables);
  }
  else if (text == "random")
  {
    return compute_variable_order_random(number_of_variables, exclude_first_variable);
  }
  else
  {
    return parse_variable_order(text, number_of_variables, exclude_first_variable);
  }
}

inline
std::vector<boost::dynamic_bitset<>> reorder_read_write_patterns(const std::vector<boost::dynamic_bitset<>>& patterns, const std::vector<std::size_t>& variable_order)
{
  std::size_t n = variable_order.size();
  std::vector<boost::dynamic_bitset<>> result;
  for (const auto& pattern: patterns)
  {
    boost::dynamic_bitset<> reordered_pattern(2*n);
    for (std::size_t i = 0; i < n; i++)
    {
      std::size_t j = variable_order[i];
      reordered_pattern[2*i] = pattern[2*j];
      reordered_pattern[2*i+1] = pattern[2*j+1];
    }
    result.push_back(reordered_pattern);
  }
  return result;
}

inline
std::set<std::size_t> parameter_indices(const std::set<data::variable>& parameters, const std::map<data::variable, std::size_t>& index)
{
  std::set<std::size_t> result;
  for (const data::variable& p: parameters)
  {
    result.insert(index.at(p));
  }
  return result;
}

inline
std::string print_read_write_patterns(const std::vector<boost::dynamic_bitset<>>& patterns)
{
  if (patterns.empty())
  {
    return "there are no read/write patterns\n";
  }

  std::size_t n = patterns.front().size() / 2;

  /* auto print_used = [n](const boost::dynamic_bitset<>& pattern)
  {
    std::ostringstream out;
    for (std::size_t i = 0; i < n; i++)
    {
      bool read = pattern[2*i];
      bool write = pattern[2*i+1];
      bool used = read || write;
      out << (used ? "1" : "0");
    }
    return out.str();
  }; */

  // N.B. The stream operator of boost::dynamic_bitset prints the bits in reverse order(!)
  /* auto print_rw = [n](const boost::dynamic_bitset<>& pattern)
  {
    std::ostringstream out;
    for (std::size_t i = 0; i < n; i++)
    {
      bool read = pattern[2*i];
      bool write = pattern[2*i+1];
      out << (read ? "1" : "0");
      out << (write ? "1" : "0");
    }
    return out.str();
  }; */ 

  auto print_compact = [n](const boost::dynamic_bitset<>& pattern)
  {
    std::ostringstream out;
    for (std::size_t i = 0; i < n; i++)
    {
      bool read = pattern[2*i];
      bool write = pattern[2*i+1];
      if (read && write)
      {
        out << '+';
      }
      else if (read)
      {
        out << 'r';
      }
      else if (write)
      {
        out << 'w';
      }
      else
      {
        out << '-';
      }
    }
    return out.str();
  };

  std::ostringstream out;
  out << "read/write patterns compacted" << std::endl;
  for (std::size_t i = 0; i < patterns.size(); i++)
  {
    out << std::setw(4) << (i+1) << " " << print_compact(patterns[i]) << std::endl;
  }
  return out.str();
}

// Set some additional read and write bits to true in patterns. This is only used for testing purposes.
template <typename Options>
void adjust_read_write_patterns(std::vector<boost::dynamic_bitset<>>& patterns, const Options& options)
{
  for (boost::dynamic_bitset<>& pattern: patterns)
  {
    std::size_t n = pattern.size() / 2;
    for (std::size_t j = 0; j < n; j++)
    {
      bool read = pattern[2*j];
      bool write = pattern[2*j + 1];
      if (options.no_discard)
      {
        read = true;
        write = true;
      }
      else if (read || write)
      {
        if (options.no_discard_read)
        {
          read = true;
        }
        if (options.no_discard_write)
        {
          write = true;
        }
      }
      pattern[2*j] = read;
      pattern[2*j + 1] = write;
    }
  }
}

inline
std::vector<boost::dynamic_bitset<>> compute_summand_group_patterns(const std::vector<boost::dynamic_bitset<>>& patterns, const std::vector<std::set<std::size_t>> groups)
{
  std::vector<boost::dynamic_bitset<>> result;

  if (!patterns.empty())
  {
    boost::dynamic_bitset<> empty(patterns.front().size());

    for (const std::set<std::size_t>& group: groups)
    {
      if (group.empty())
      {
        continue;
      }
      boost::dynamic_bitset<> pattern = empty;
      for (std::size_t i: group)
      {
        pattern |= patterns[i];
      }
      result.push_back(pattern);
    }
  }
  
  return result;
}

} // namespace mcrl2::symbolic

#endif // MCRL2_SYMBOLIC_ORDERING_H
