// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file partition.h


#ifndef MCRL2_PBESSYMBOLICBISIM_PARTITION_H
#define MCRL2_PBESSYMBOLICBISIM_PARTITION_H

#include <queue>
#include <random>

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/join.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/structure_graph_builder.h"

#include "simplifier.h"
#define THIN       "0"
#define BOLD       "1"
#define GREEN(S)  "\033[" S ";32m"
#define YELLOW(S) "\033[" S ";33m"
#define RED(S)    "\033[" S ";31m"
#define NORMAL    "\033[0;0m"
#include "ppg_parser.h"
#include "partition_block.h"


namespace mcrl2
{
namespace data
{

class dependency_graph_partition
{
  typedef block block_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;
  typedef pbes_system::structure_graph::index_type sg_index_t;

protected:
  const pbes_system::detail::ppg_pbes m_spec;

  data_manipulators m_dm;

  std::list< block_t >      m_proof_blocks;
  std::list< block_t >      m_other_blocks;

  split_cache<block>* m_block_cache;
  split_cache<subblock>* m_subblock_cache;
  std::map< pbes_system::detail::ppg_equation, std::size_t > m_rank_map;

  pbes_system::structure_graph& m_structure_graph;
  pbes_system::detail::manual_structure_graph_builder m_sg_builder;
  std::vector<block_t> m_block_index;

  bool m_early_termination;
  bool m_randomize;

  bool is_valid_approximation(const block_t& src, bool is_positive_pg) const
  {
    return (src.is_conjunctive() && is_positive_pg) || (!src.is_conjunctive() && !is_positive_pg);
  }

  bool is_in_strategy(const block_t& src, const block_t& dest) const
  {
    // If there is no winning strategy in src, then all outgoing transitions
    // from src have to be considered.
    sg_index_t strat = m_structure_graph.strategy(src.index);
    return strat == pbes_system::undefined_vertex() || strat == dest.index;
  }

  /**
   * \brief Split block phi_k on phi_l
   */
  bool split_block(const block_t& phi_k, const block_t& phi_l, const std::vector<subblock>& subblocks, bool use_optimisations)
  {
    if(use_optimisations && !is_in_strategy(phi_k, phi_l))
    {
      // This edge is not part of the winning strategy in the parity game
      return false;
    }
    if(m_block_cache->check_refinement(phi_k, phi_l))
    {
      // This split has already been tried before
      return false;
    }
    if(!phi_k.has_transition(phi_l))
    {
      // There is no transition between phi_k and phi_l, so a split will definitely
      // not succeed
      return false;
    }

    mCRL2log(log::verbose) << "Splitting\n" << phi_k << "wrt\n" << phi_l << std::endl;

    // Try to split
    const std::pair<block, block> split_result = phi_k.split(phi_l, subblocks, use_optimisations);
    if(split_result.first.is_empty() || split_result.second.is_empty())
    {
      // Split failed
      return false;
    }

    if(!use_optimisations)
    {
      // Update the caches and the partition
      block_t phi_k_copy(phi_k);
      block_t phi_l_copy(phi_l);
      m_proof_blocks.remove(phi_k);
      std::vector<std::list<block>> block_vec({m_proof_blocks, m_other_blocks});
      elements_iterator< block, std::vector<std::list<block>>> block_it(block_vec);
      m_block_cache->replace_after_split(block_it, block_it.end(), phi_k_copy, split_result.first, split_result.second);
      m_block_cache->insert_transition(split_result.first, phi_l_copy, true);
      m_block_cache->insert_transition(split_result.second, phi_l_copy, false);

      m_proof_blocks.push_front(split_result.first);
      block_t& new_pos_block = m_proof_blocks.front();
      m_proof_blocks.push_front(split_result.second);
      block_t& new_neg_block = m_proof_blocks.front();

      update_structure_graph(new_pos_block, new_neg_block);
    }
    return true;
  }

  void update_structure_graph(block_t& new_pos_block, block_t& new_neg_block)
  {
    new_neg_block.index = m_sg_builder.insert_vertex(new_neg_block.is_conjunctive(), new_neg_block.rank(m_rank_map));
    m_block_index.resize(new_neg_block.index + 1);
    m_block_index[new_neg_block.index] = new_neg_block;
    m_block_index[new_pos_block.index] = new_pos_block;
    // Investigate the predecessors of the parent block
    for(sg_index_t pred: m_structure_graph.all_predecessors(new_pos_block.index))
    {
      if(!m_block_index[pred].has_transition(new_pos_block))
      {
        m_sg_builder.remove_edge(pred, new_pos_block.index);
      }
      if(m_block_index[pred].has_transition(new_neg_block))
      {
        m_sg_builder.insert_edge(pred, new_neg_block.index);
      }
    }
    // Investigate the successors of the parent block
    for(sg_index_t succ: m_structure_graph.all_successors(new_pos_block.index))
    {
      if(!new_pos_block.has_transition(m_block_index[succ]))
      {
        m_sg_builder.remove_edge(new_pos_block.index, succ);
      }
      if(new_neg_block.has_transition(m_block_index[succ]))
      {
        m_sg_builder.insert_edge(new_neg_block.index, succ);
      }
    }
    // Check for a self loop on new_neg_block
    if(new_neg_block.has_transition(new_neg_block))
    {
      m_sg_builder.insert_edge(new_neg_block.index, new_neg_block.index);
    }
    // Update the initial vertex if necessary
    if(m_structure_graph.initial_vertex() == new_pos_block.index && new_neg_block.contains_state(m_spec.initial_state()))
    {
      m_sg_builder.set_initial_state(new_neg_block.index);
    }
    m_sg_builder.finalize();
  }

  std::vector<subblock> make_subblock_list() const
  {
    std::vector<subblock> result;
    for(const block_t& b: m_proof_blocks)
    {
      result.insert(result.end(), b.subblocks().begin(), b.subblocks().end());
    }
    for(const block_t& b: m_other_blocks)
    {
      result.insert(result.end(), b.subblocks().begin(), b.subblocks().end());
    }
    return result;
  }

  /**
   * \brief Try every combination of blocks to
   * see whether the partition can be refined.
   */
  bool refine_step(bool use_optimisations, bool is_positive_pg)
  {
    std::vector<subblock> subblocks = make_subblock_list();
    for(const block_t& phi_k: m_proof_blocks)
    {
      if(use_optimisations && is_valid_approximation(phi_k, is_positive_pg))
      {
        continue;
      }
      for(const block_t& phi_l: m_proof_blocks)
      {
        if(split_block(phi_k, phi_l, subblocks, use_optimisations))
        {
          return true;
        }
      }
    } // stop when we have investigated all blocks
    return false;
  }

  /**
   * \brief Finds the block in blocks that contains the initial state
   */
  template <typename Container>
  block_t find_initial_block(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr) const
  {
    for(const block_t& block: blocks)
    {
      if(block.contains_state(m_spec.initial_state()))
      {
        return block;
      }
    }
    throw mcrl2::runtime_error("Initial block not found.");
  }

  /**
   * \brief Computes the reachable blocks.
   * \detail The partition is updated, such that it only contains
   * reachable blocks.
   * \param only_check_proof_blocks Set to true when reachability should only be checked
   * within m_proof_blocks
   */
  void find_reachable_blocks(const bool only_check_proof_blocks)
  {
    std::vector<block_t> unreachable(m_proof_blocks.begin(),m_proof_blocks.end());
    if(!only_check_proof_blocks)
    {
      unreachable.insert(unreachable.end(), m_other_blocks.begin(), m_other_blocks.end());
    }
    if(m_randomize)
    {
      std::shuffle(unreachable.begin(), unreachable.end(), std::random_device());
    }
    std::queue<block_t> open_set;

    // Search for the block that contains the initial state
    block_t initial_block = find_initial_block(unreachable);
    initial_block.bfs_level = 0;
    unreachable.erase(std::find(unreachable.begin(), unreachable.end(), initial_block));
    open_set.push(initial_block);
    m_proof_blocks.clear();
    if(!only_check_proof_blocks)
    {
      m_other_blocks.clear();
    }
    m_proof_blocks.push_back(initial_block);

    while(!open_set.empty())
    {
      // Take a block from the open set
      const block_t block = open_set.front();
      open_set.pop();

      // Look for possible successors of block in the set of unreachable blocks
      for(auto i = unreachable.begin(); i != unreachable.end();)
      {
        if(block.has_transition(*i))
        {
          i->bfs_level = block.bfs_level + 1;
          // A transition was found, so *i is reachable
          open_set.push(*i);
          m_proof_blocks.push_back(*i);
          i = unreachable.erase(i);
        }
        else
        {
          ++i;
        }
      }
    }


    mCRL2log(log::verbose) << RED(THIN) << "Unreachable blocks:" << NORMAL << std::endl;
    int i = 0;
    for(const block_t& block: unreachable)
    {
      if(only_check_proof_blocks)
      {
        // Add the unreachable blocks to m_other_blocks to make
        // sure no blocks get lost
        // When we only checking reachability in m_proof_blocks,
        // the unreachable blocks might still be reachable through
        // some blocks in m_other_blocks
        // Only when we are checking reachability in the whole graph,
        // we can really throw away the unreachable blocks.
        m_other_blocks.push_front(block);
      }
      mCRL2log(log::verbose) << "  block " << i << "\n" << pp(block);
      i++;
    }
  }

  void make_rank_map()
  {
    pbes_system::fixpoint_symbol previous_symbol(pbes_system::fixpoint_symbol::nu());
    std::size_t rank = 0;
    for(const equation_type_t& eq: m_spec.equations())
    {
      if(eq.symbol() != previous_symbol)
      {
        previous_symbol = eq.symbol();
        rank++;
      }
      m_rank_map.insert(std::make_pair(eq, rank));
    }
  }

  inline
  void make_initial_structure_graph()
  {
    m_block_index.resize(m_proof_blocks.size());
    for(block& b: m_proof_blocks)
    {
      b.index = m_sg_builder.insert_vertex(b.is_conjunctive(), b.rank(m_rank_map));
      m_block_index[b.index] = b;
    }
    for(const block& src: m_proof_blocks)
    {
      bool has_outgoing_edge = false;
      for(const block& dest: m_proof_blocks)
      {
        if(src.has_transition(dest))
        {
          m_sg_builder.insert_edge(src.index, dest.index);
          has_outgoing_edge = true;
        }
      }

      if(!has_outgoing_edge)
      {
        // All nodes should have an outgoing edge, since SRF requires
        // a summand to either X_true or X_false in every right-hand side
        throw mcrl2::runtime_error("Did not find an outgoing edge for the block " + pp(src));
      }
    }
    m_sg_builder.set_initial_state(find_initial_block(m_proof_blocks).index);
    m_sg_builder.finalize();
  }

  /**
   * \brief Print the current partition to stdout
   */
  template <typename Container>
  void print_partition(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr) const
  {
    int i = 0;
    for(const block_t& block: blocks)
    {
      mCRL2log(log::verbose) << YELLOW(THIN) << "  block " << i << NORMAL << " index: " << block.index << "\n" << block;
      ++i;
    }
  }

  void make_initial_partition(bool fine_initial)
  {
    if(fine_initial)
    {
      // Create on block for every equation
      for(const equation_type_t& eq: m_spec.equations())
      {
        m_proof_blocks.emplace_back(std::list<subblock>({subblock(eq, m_dm, m_subblock_cache)}), m_block_cache);
      }
    }
    else
    {
      // For each PBES rank, create one block with conjunctive subblocks and one
      // block with disjunctive subblocks.
      pbes_system::fixpoint_symbol previous_symbol;
      std::list<subblock> conjunctive_subblocks;
      std::list<subblock> disjunctive_subblocks;
      for(std::vector<equation_type_t>::const_iterator it = m_spec.equations().begin(); it != m_spec.equations().end();)
      {
        previous_symbol = it->symbol();
        (it->is_conjunctive() ? conjunctive_subblocks : disjunctive_subblocks).emplace_back(*it, m_dm, m_subblock_cache);

        ++it;
        if(it == m_spec.equations().end() || it->symbol() != previous_symbol)
        {
          if(!conjunctive_subblocks.empty())
          {
            m_proof_blocks.emplace_back(conjunctive_subblocks, m_block_cache);
            conjunctive_subblocks = std::list<subblock>();
          }
          if(!disjunctive_subblocks.empty())
          {
            m_proof_blocks.emplace_back(disjunctive_subblocks, m_block_cache);
            disjunctive_subblocks = std::list<subblock>();
          }
        }
      }
    }
  }

public:

  dependency_graph_partition(const pbes_system::detail::ppg_pbes& spec,
    const rewriter& r, const rewriter& pr, const simplifier_mode& simpl_mode,
    pbes_system::structure_graph& sg, bool fine_initial, bool early_termination, bool randomize)
  : m_spec(spec)
  , m_dm(r, pr, spec.data())
  , m_structure_graph(sg)
  , m_sg_builder(sg)
  , m_early_termination(early_termination)
  , m_randomize(randomize)
  {
    m_dm.contains_reals = false;
    for(const equation_type_t& eq: m_spec.equations())
    {
      m_dm.simpl->insert(std::make_pair(eq.variable(), get_simplifier_instance(simpl_mode, m_dm.rewr, m_dm.proving_rewr, eq.variable().parameters(), m_spec.data())));
      m_dm.contains_reals |= std::find_if(eq.variable().parameters().begin(), eq.variable().parameters().end(),
        [](const variable& var) { return var.sort() == sort_real::real_();}) != eq.variable().parameters().end();
    }
    m_block_cache = new split_cache<block>();
    m_subblock_cache = new split_cache<subblock>();
    make_initial_partition(fine_initial);
    make_rank_map();
    make_initial_structure_graph();
    print_partition(m_proof_blocks);
  }

  std::size_t size() const
  {
    return m_proof_blocks.size() + m_other_blocks.size();
  }

  void set_proof(const std::set< sg_index_t >& proof_nodes)
  {
    // Move all the blocks to the other set while keeping the order m_proof_blocks ++ m_other_blocks
    std::vector< block_t > all_blocks(m_proof_blocks.begin(), m_proof_blocks.end());
    // all_blocks.swap(m_proof_blocks);
    all_blocks.insert(all_blocks.end(), m_other_blocks.begin(), m_other_blocks.end());
    m_proof_blocks.clear();
    m_other_blocks.clear();

    // Move the requested blocks to the main set
    for(const block_t& b: all_blocks)
    {
      if(proof_nodes.find(b.index) != proof_nodes.end())
      {
        m_proof_blocks.push_back(b);
      }
      else
      {
        m_other_blocks.push_back(b);
      }
    }
  }

  void print() const
  {
    mCRL2log(log::verbose) << GREEN(THIN) << "Partition proof blocks:" << NORMAL << std::endl;
    print_partition(m_proof_blocks);
    mCRL2log(log::verbose) << GREEN(THIN) << "Partition other blocks:" << NORMAL << std::endl;
    print_partition(m_other_blocks);
  }

  /**
   * Refine the partition num_steps times
   * When num_steps is 0, the algorithm only
   * terminates when the partition is stable.
   * \return true when the partition was already stable
   */
  bool refine_n_steps(std::size_t num_steps, bool is_positive_pg)
  {
    // mCRL2log(log::verbose) << "Initial partition:" << std::endl;
    // print_partition(m_proof_blocks);

    // First check whether the current proof graph is stable under optimisations.
    // This will not affect the cache.
    if(m_early_termination && !refine_step(true, is_positive_pg))
    {
      return true;
    }
    std::size_t num_iterations = 0;
    while(refine_step(false, is_positive_pg))
    {
      print();
      num_iterations++;
      if(num_steps != 0 && num_iterations >= num_steps)
      {
        break;
      }
      // Check reachability only in m_proof_blocks.
      // Blocks that are not reachable in there will
      // be moved to m_other_blocks, because it is not
      // certain that they are unreachable when considering
      // the whole graph.
      find_reachable_blocks(true);
      mCRL2log(log::verbose) << "End of a refinement step " << num_iterations << std::endl;
    }
    // Check reachability in the full graph.
    // In this case, unreachable blocks will really be
    // thrown away.
    find_reachable_blocks(false);
    if(num_iterations == 0 || num_steps == 0)
    {
      mCRL2log(log::verbose) << "Final partition:" << std::endl;
      print_partition(m_proof_blocks);
    }
    if(num_steps == 0)
    {
      mCRL2log(log::info) << "Number of iterations " << num_iterations+1 << std::endl;
    }
    return num_iterations == 0;
  }

};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIM_PARTITION_H
