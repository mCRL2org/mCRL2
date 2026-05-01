// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "mcrl2/lps/lps_summand_group.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/explorer_utilities.h"
#include "mcrl2/lps/find.h"

namespace mcrl2::lps
{

/// \brief Assigns a unique index to every parameter of the process.
template <typename Specification>
std::map<data::variable, std::size_t> compute_process_parameter_index(const Specification& lpsspec)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: lpsspec.process().process_parameters())
  {
    result[v] = i++;
  }
  return result;
}

// Computes separate bitsets for read and write parameters
template <typename Specification>
std::pair<std::vector<std::vector<std::size_t>>, std::vector<std::vector<std::size_t>>> compute_read_write_patterns_separated(const Specification& lpsspec)
{
  using utilities::detail::as_vector;

  std::vector<std::vector<std::size_t>> result_r;
  std::vector<std::vector<std::size_t>> result_w;

  auto process_parameters = as_set(lpsspec.process().process_parameters());
  std::map<data::variable, std::size_t> index = process_parameter_index(lpsspec);

  for (const auto& summand: lpsspec.process().action_summands())
  {
    auto [read_parameters, write_parameters] = read_write_parameters(summand, process_parameters);
    auto read = symbolic::parameter_indices(read_parameters, index);
    auto write = symbolic::parameter_indices(write_parameters, index);
    result_r.push_back(as_vector(read));
    result_w.push_back(as_vector(write));
  }

  return {result_r, result_w};
}

struct counting_iterator
{
  using value_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using pointer = const std::size_t*;
  using reference = std::size_t;

  std::size_t value;

  explicit counting_iterator(std::size_t v = 0) : value(v) {}

  std::size_t operator*() const
  {
    return value;
  }

  counting_iterator& operator++()
  {
    ++value;
    return *this;
  }

  counting_iterator operator++(int)
  {
    counting_iterator tmp(*this);
    ++value;
    return tmp;
  }

  bool operator==(const counting_iterator& other) const
  {
    return value == other.value;
  }

  bool operator!=(const counting_iterator& other) const
  {
    return value != other.value;
  }
};

namespace detail
{
// This struct is used to avoid the explicit construction of the hash key for entries in the projection_cache.
struct projection_cache_key
{
  data::mutable_indexed_substitution<>& sigma;
  const std::vector<std::size_t>& indices;
  const data::variable_vector& process_parameters;
};

// Compare projection_cache_key with a state
struct projection_cache_equality1
{
  bool operator()(const atermpp::aterm& key1, const atermpp::aterm& key2) const
  {
    return key1==key2;
  }

  bool operator()(const lps::state& stored, const projection_cache_key& k) const
  {
    auto it = k.indices.begin();
    for (const data::data_expression& d : stored)
    {
      if (d != k.sigma(k.process_parameters[*it]))
      {
        return false;
      }
      ++it;
    }
    return true;
  }

  // symmetric overload (unordered_map is allowed to call the comparator in either order)
  bool operator()(const projection_cache_key& k, const lps::state& stored) const
  {
    return operator()(stored, k);
  }
};

// Compare projection_cache_key with a state
struct projection_cache_equality
{
  // Standard aterm vs aterm comparison
  bool operator()(const atermpp::aterm& key1, const atermpp::aterm& key2) const
  {
    return key1 == key2;
  }

  // Comparison between a stored aterm and the lookup key
  bool operator()(const atermpp::aterm& stored, const projection_cache_key& k) const
  {
    auto it = k.indices.begin();
    for (const atermpp::aterm& d : stored)
    {
      // If we encounter more elements in the stored state than indices in the key
      if (it == k.indices.end())
      {
        return false;
      }

      // Compare the stored expression with the value in the substitution for the current index
      if (d != k.sigma(k.process_parameters[*it]))
      {
        return false;
      }
      ++it;
    }
    // Ensure all indices were checked
    return it == k.indices.end();
  }

  // Symmetric overload for the lookup key
  bool operator()(const projection_cache_key& k, const atermpp::aterm& stored) const
  {
    return operator()(stored, k);
  }
};

struct projection_cache_hash
{
  std::size_t operator()(const std::pair<const atermpp::aterm, std::list<atermpp::term_list<mcrl2::data::data_expression>>>& pair) const
  {
    return operator()(pair.first);
  }

  std::size_t operator()(const atermpp::aterm& key) const
  {
    std::size_t hash = 0;
    for (const atermpp::aterm& d: key)
    {
      hash = atermpp::detail::combine(hash, d);
    }
    return hash;
  }

  std::size_t operator()(const projection_cache_key& key) const
  {
    std::size_t hash = 0;
    for (auto i: key.indices)
    {
      const data::variable& v = key.process_parameters[i];
      hash = atermpp::detail::combine(hash,key.sigma(v));
    }
    return hash;
  }
};

} // namespace detail

} // namespace mcrl2::lps
