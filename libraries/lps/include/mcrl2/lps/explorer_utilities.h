// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer_todo_set.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_EXPLORER_UTILITIES_H
#define MCRL2_LPS_EXPLORER_UTILITIES_H

#include "mcrl2/atermpp/standard_containers/indexed_set.h"
#include "mcrl2/atermpp/standard_containers/detail/unordered_map_implementation.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/explorer_utilities.h"

namespace mcrl2::lps
{

// Global helpers to construct a rewriter and add real operators for explorer
inline std::set<data::function_symbol> add_real_operators(std::set<data::function_symbol> s)
{
  s.insert(data::less_equal(data::sort_real::real_()));
  s.insert(data::greater_equal(data::sort_real::real_()));
  s.insert(data::sort_real::plus(data::sort_real::real_(), data::sort_real::real_()));
  return s;
}

template <typename Specification>
data::rewriter construct_rewriter(const Specification& lpsspec, data::rewrite_strategy rewrite_strategy, bool remove_unused_rewrite_rules)
{
  if (remove_unused_rewrite_rules)
  {
    return data::rewriter(lpsspec.data(),
      data::used_data_equation_selector(lpsspec.data(), add_real_operators(lps::find_function_symbols(lpsspec)), lpsspec.global_variables(), false),
      rewrite_strategy);
  }
  else
  {
    return data::rewriter(lpsspec.data(), rewrite_strategy);
  }
}

enum class caching { none, local, global };

inline
std::ostream& operator<<(std::ostream& os, caching c)
{
  switch(c)
  {
    case caching::none: os << "none"; break;
    case caching::local: os << "local"; break;
    case caching::global: os << "global"; break;
    default: os.setstate(std::ios_base::failbit);
  }
  return os;
}

inline
std::vector<data::data_expression> make_data_expression_vector(const data::data_expression_list& v)
{
  return std::vector<data::data_expression>(v.begin(), v.end());
}

template <typename Summand>
inline const stochastic_distribution& summand_distribution(const Summand& /* summand */)
{
  static stochastic_distribution empty_distribution;
  return empty_distribution;
}

template <>
inline const stochastic_distribution& summand_distribution(const lps::stochastic_action_summand& summand)
{
  return summand.distribution();
}

inline
const stochastic_distribution& initial_distribution(const lps::specification& /* lpsspec */)
{
  static stochastic_distribution empty_distribution;
  return empty_distribution;
}

inline
const stochastic_distribution& initial_distribution(const lps::stochastic_specification& lpsspec)
{
  return lpsspec.initial_process().distribution();
}

namespace detail
{
// The functions below are used to support the key type in caches.
//
struct cheap_cache_key
{
  data::mutable_indexed_substitution<>& m_sigma;
  const std::vector<data::variable>& m_gamma;

  cheap_cache_key(data::mutable_indexed_substitution<>& sigma, const std::vector<data::variable>& gamma)
    : m_sigma(sigma),
      m_gamma(gamma)
  {}

};

struct cache_equality
{
  bool operator()(const atermpp::aterm& key1, const atermpp::aterm& key2) const
  {
    return key1==key2;
  }

  bool operator()(const atermpp::aterm& key1, const cheap_cache_key& key2) const
  {
    std::vector<data::variable>::const_iterator i=key2.m_gamma.begin();
    for(const atermpp::aterm& d: key1)
    {
      if (d!=key2.m_sigma(*i))
      {
        return false;
      }
      ++i;
    }
    return true;
  }
};

struct cache_hash
{
  std::size_t operator()(const std::pair<const atermpp::aterm,
                                         std::list<atermpp::term_list<mcrl2::data::data_expression>>>& pair) const
  {
    return operator()(pair.first);
  }

  std::size_t operator()(const atermpp::aterm& key) const
  {
    std::size_t hash=0;
    for(const atermpp::aterm& d: key)
    {
      hash=atermpp::detail::combine(hash,d);
    }
    return hash;
  }

  std::size_t operator()(const cheap_cache_key& key) const
  {
    std::size_t hash=0;
    for(const data::variable& v: key.m_gamma)
    {
      hash=atermpp::detail::combine(hash,key.m_sigma(v));
    }
    return hash;
  }
};

} // end namespace detail

using summand_cache_map = atermpp::utilities::unordered_map<atermpp::aterm,
    atermpp::term_list<data::data_expression_list>,
    detail::cache_hash,
    detail::cache_equality,
    std::allocator<std::pair<atermpp::aterm, atermpp::term_list<data::data_expression_list>>>,
    true>;

struct explorer_summand
{
  data::variable_list variables;
  data::data_expression condition;
  lps::multi_action multi_action;
  stochastic_distribution distribution;
  std::vector<data::data_expression> next_state;
  std::size_t index;

  // attributes for caching
  caching cache_strategy;
  std::vector<data::variable> gamma;
  atermpp::function_symbol f_gamma;
  mutable summand_cache_map local_cache;

  template <typename ActionSummand>
  explorer_summand(const ActionSummand& summand, std::size_t summand_index, const data::variable_list& process_parameters, caching cache_strategy_)
    : variables(summand.summation_variables()),
      condition(summand.condition()),
      multi_action(summand.multi_action()),
      distribution(summand_distribution(summand)),
      next_state(make_data_expression_vector(summand.next_state(process_parameters))),
      index(summand_index),
      cache_strategy(cache_strategy_)
  {
    gamma = free_variables(summand.condition(), process_parameters);
    if (cache_strategy_ == caching::global)
    {
      gamma.insert(gamma.begin(), data::variable());
    }
    f_gamma = atermpp::function_symbol("@gamma", gamma.size());
  }

  template <typename T>
  std::vector<data::variable> free_variables(const T& x, const data::variable_list& v)
  {
    using utilities::detail::contains;
    std::set<data::variable> FV = data::find_free_variables(x);
    std::vector<data::variable> result;
    for (const data::variable& vi: v)
    {
      if (contains(FV, vi))
      {
        result.push_back(vi);
      }
    }
    return result;
  }

  void compute_key(atermpp::aterm& key,
                   data::mutable_indexed_substitution<>& sigma) const
  {
    if (cache_strategy == caching::global)
    {
      bool is_first_element = true;
      atermpp::make_term_appl(key, f_gamma, gamma.begin(), gamma.end(),
                                        [&](data::data_expression& result, const data::variable& x)
                                        {
                                          if (is_first_element)
                                          {
                                            is_first_element = false;
                                            result=condition;
                                            return;
                                          }
                                          sigma.apply(x, result);
                                          return;
                                        }
                             );
    }
    else
    {
      atermpp::make_term_appl(key, f_gamma, gamma.begin(), gamma.end(),
                                        [&](data::data_expression& result, const data::variable& x)
                                        {
                                          sigma.apply(x, result);
                                        }
                              );
    }
  }
};

inline
std::ostream& operator<<(std::ostream& out, const explorer_summand& summand)
{
  return out << lps::stochastic_action_summand(
    summand.variables,
    summand.condition,
    summand.multi_action,
    data::make_assignment_list(summand.variables, summand.next_state),
    summand.distribution
  );
}

struct abortable
{
  virtual ~abortable() = default;
  virtual void abort() = 0;
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_UTILITIES_H
