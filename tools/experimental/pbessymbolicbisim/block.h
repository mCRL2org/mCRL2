// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file block.h

#ifndef MCRL2_PBESSYMBOLICBISIM_BLOCK_H
#define MCRL2_PBESSYMBOLICBISIM_BLOCK_H

#include <list>

#include "mcrl2/utilities/hash_utility.h"

#include "subblock.h"

namespace mcrl2
{
namespace data
{


class block
{
  typedef subblock subblock_t;

protected:
  // const pbes_type_t m_spec;
  // invariant: forall b: m_subblocks. !b.is_empty()
  std::shared_ptr<const std::list< subblock_t >> m_subblocks;

  split_cache<block>* m_cache;

public:

  block()
  : m_subblocks(std::make_shared<std::list<subblock_t>>())
  , m_cache(nullptr)
  {}

  block(const std::list< subblock >& subblocks, split_cache<block>* cache)
  : m_subblocks(std::make_shared<std::list<subblock_t>>(subblocks.begin(), subblocks.end()))
  , m_cache(cache)
  {}

  /// Move semantics
  block(const block&) noexcept = default;
  block(block&&) noexcept = default;
  block& operator=(const block&) noexcept = default;
  block& operator=(block&&) noexcept = default;

  bool has_transition(const block& other) const
  {
    const auto find_result = m_cache->check_transition(*this, other);
    if(find_result != m_cache->transition_end())
    {
      return find_result->second;
    }
    bool result = std::any_of(m_subblocks->begin(), m_subblocks->end(), [this,&other](const subblock& sb){ return sb.has_transition(*other.m_subblocks); });
    m_cache->insert_transition(*this, other, result);
    return result;
  }

  std::pair<block, block> split(const block& other) const
  {
    if(!has_transition(other))
    {
      return std::make_pair(block(), *this);
    }
    if(m_cache->check_refinement(*this, other))
    {
      return std::make_pair(*this, block());
    }
    // Collect the result of splitting the subblocks in two
    // lists
    // pos_subblocks contains subblocks with a transittion to other
    // neg_subblocks contains subblocks without a transittion to other
    std::list<subblock> pos_subblocks, neg_subblocks;
    for(const subblock& sb: *m_subblocks)
    {
      std::pair<subblock,subblock> sb_split = sb.split(*other.m_subblocks);
      if(!sb_split.first.is_empty())
      {
        pos_subblocks.push_back(sb_split.first);
      }
      if(!sb_split.second.is_empty())
      {
        neg_subblocks.push_back(sb_split.second);
      }
    }
    m_cache->insert_refinement(*this, other);
    // If one of the two lists is empty, we just return this and an empty block
    if(pos_subblocks.empty())
    {
      return std::make_pair(block(), *this);
    }
    if(neg_subblocks.empty())
    {
      return std::make_pair(*this, block());
    }
    return std::make_pair(block(pos_subblocks, m_cache), block(neg_subblocks, m_cache));
  }

  const std::list<subblock>& subblocks() const
  {
    return *m_subblocks;
  }

  bool is_conjunctive() const
  {
    return m_subblocks->front().equation().is_conjunctive();
  }

  pbes_system::fixpoint_symbol fixpoint_symbol() const
  {
    return m_subblocks->front().equation().symbol();
  }

  inline std::size_t rank(const std::map< pbes_system::detail::ppg_equation, std::size_t >& rank_map) const
  {
    return rank_map.at(m_subblocks->front().equation());
  }

  std::string name() const
  {
    std::ostringstream out;
    for(const subblock& sb: *m_subblocks)
    {
      out << sb.equation().variable().name();
    }
    return out.str();
  }

  bool is_empty() const
  {
    return m_subblocks->empty();
  }

  bool contains_state(const pbes_system::propositional_variable_instantiation& state) const
  {
    return std::any_of(m_subblocks->cbegin(), m_subblocks->cend(), [&state](const subblock_t& subblock){ return subblock.contains_state(state); });
  }

  bool operator==(const block& other) const
  {
    return *m_subblocks == *other.m_subblocks;
  }

  bool operator!=(const block& other) const
  {
    return *m_subblocks != *other.m_subblocks;
  }

  bool operator<(const block& other) const
  {
    return *m_subblocks < *other.m_subblocks;
  }
};

std::string pp(const block&);
std::string pp(const block& b)
{
  std::ostringstream out;
  for(const subblock& sb: b.subblocks())
  {
    out << sb << "\n";
  }
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const block& b)
{
  return out << pp(b);
}

} // namespace data
} // namespace mcrl2

namespace std
{

  template<>
  struct hash<mcrl2::data::block>
  {
    // Default constructor, required for const qualified hash functions.
    hash()
    {}

    std::size_t operator()(const mcrl2::data::block& t) const
    {
      const hash< mcrl2::data::subblock > hasher{};
      std::size_t result = 0;
      for(const mcrl2::data::subblock& sb: t.subblocks())
      {
        result = mcrl2::utilities::detail::hash_combine(result, hasher(sb));
      }
      return result;
    }
  };

}

#endif // MCRL2_PBESSYMBOLICBISIM_BLOCK_H
