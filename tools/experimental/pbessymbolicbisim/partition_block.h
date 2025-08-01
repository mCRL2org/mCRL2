// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_PBESSYMBOLICBISIM_PARTITION_BLOCK_H
#define MCRL2_PBESSYMBOLICBISIM_PARTITION_BLOCK_H

#include "mcrl2/pbes/structure_graph.h"

#include "partition_subblock.h"

namespace mcrl2::data
{


class block
{
  using subblock_t = subblock;
  using sg_index_t = pbes_system::structure_graph::index_type;

protected:
  // const pbes_type_t m_spec;
  // invariant: forall b: m_subblocks. !b.is_empty()
  std::shared_ptr<const std::list< subblock_t >> m_subblocks;

  split_cache<block>* m_cache;

public:

  sg_index_t index = pbes_system::undefined_vertex();
  int bfs_level = 0;

  block()
  : m_subblocks(std::make_shared<std::list<subblock_t>>())
  , m_cache(nullptr)
  {}

  block(const std::list< subblock_t >& subblocks, split_cache<block>* cache, sg_index_t i = pbes_system::undefined_vertex())
  : m_subblocks(std::make_shared<std::list<subblock_t>>(subblocks.begin(), subblocks.end()))
  , m_cache(cache)
  , index(i)
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
    bool result = std::any_of(m_subblocks->begin(), m_subblocks->end(), [&other](const subblock_t& sb){ return sb.has_transition(*other.m_subblocks); });
    m_cache->insert_transition(*this, other, result);
    return result;
  }

  std::pair<block, block> split(const block& other, const std::vector<subblock_t>& subblock_list, bool use_optimisations) const
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
    std::list<subblock_t> pos_subblocks;
    std::list<subblock_t> neg_subblocks;
    for(const subblock_t& sb: *m_subblocks)
    {
      std::pair<subblock_t,subblock_t> sb_split = sb.split(*other.m_subblocks, subblock_list);
      if(!sb_split.first.is_empty())
      {
        pos_subblocks.push_back(sb_split.first);
      }
      if(!sb_split.second.is_empty())
      {
        neg_subblocks.push_back(sb_split.second);
      }
    }
    // When not applying optimisations, always insert in the cache (on a
    // succesful split *this will be replace by the new blocks).
    // When applying optimisations, only insert in the cache when *this was
    // stable wrt other (ie one of pos_subblocks or neg_subblocks is empty).
    if(!use_optimisations || pos_subblocks.empty() || neg_subblocks.empty())
    {
      m_cache->insert_refinement(*this, other);
    }
    // If one of the two lists is empty, we just return this and an empty block
    if(pos_subblocks.empty())
    {
      return std::make_pair(block(), *this);
    }
    if(neg_subblocks.empty())
    {
      return std::make_pair(*this, block());
    }
    return std::make_pair(block(pos_subblocks, m_cache, this->index), block(neg_subblocks, m_cache));
  }

  const std::list<subblock_t>& subblocks() const
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
    for(const subblock_t& sb: *m_subblocks)
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
inline std::string pp(const block& b)
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

} // namespace mcrl2::data

namespace std
{

  template<>
  struct hash<mcrl2::data::block>
  {
    // Default constructor, required for const qualified hash functions.
    hash() = default;

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

#endif // MCRL2_PBESSYMBOLICBISIM_PARTITION_BLOCK_H
