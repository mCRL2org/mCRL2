// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file partition.h


#ifndef MCRL2_PBESSYMBOLICBISIM_PARTITION_H
#define MCRL2_PBESSYMBOLICBISIM_PARTITION_H

#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/join.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"
#include "mcrl2/pg/ParityGame.h"
#include "mcrl2/smt/cvc4.h"
#include "mcrl2/smt/solver.h"
#include "mcrl2/smt/translation_error.h"
#include "mcrl2/utilities/logger.h"

#include "simplifier.h"
#define THIN       "0"
#define BOLD       "1"
#define GREEN(S)  "\033[" S ";32m"
#define YELLOW(S) "\033[" S ";33m"
#define RED(S)    "\033[" S ";31m"
#define NORMAL    "\033[0;0m"
#include "ppg_parser.h"
#include "block.h"


namespace mcrl2
{
namespace data
{

class dependency_graph_partition
{
  typedef block block_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;

protected:
  const pbes_system::detail::ppg_pbes m_spec;

  data_manipulators m_dm;

  std::list< block_t >      m_proof_blocks;
  std::list< block_t >      m_other_blocks;

  split_cache<block>* m_block_cache;
  split_cache<subblock>* m_subblock_cache;
  std::map< pbes_system::detail::ppg_equation, priority_t > m_rank_map;

  elements_iterator< subblock, std::list< std::list<subblock> >> get_subblock_iterator(std::list< std::list<subblock> >& subblocks)
  {
    for(const block& b: m_proof_blocks)
    {
      subblocks.push_back(b.subblocks());
    }
    for(const block& b: m_other_blocks)
    {
      subblocks.push_back(b.subblocks());
    }
    return elements_iterator< subblock, std::list< std::list<subblock>>>(subblocks);
  }

  /**
   * \brief Split block phi_k on phi_l
   */
  bool split_block(const block_t& phi_k, const block_t& phi_l)
  {
    if(m_block_cache->check_refinement(phi_k, phi_l))
    {
      // This split has already been tried before
      return false;
    }
    if(!phi_k.has_transition(phi_l))
    {
      return false;
    }

    const std::pair<block, block> split_result = phi_k.split(phi_l);
    if(split_result.first.is_empty() || split_result.second.is_empty())
    {
      // Split failed
      return false;
    }

    // mCRL2log(log::verbose) << "Split " << phi_k << "\nwrt " << phi_l << std::endl;

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
    m_proof_blocks.push_front(split_result.second);
    return true;
  }

  /**
   * \brief Try every combination of phi_k, phi_l and action summand to
   * see whether the partition can be refined.
   */
  bool refine_step()
  {
    for(const block_t& phi_k: m_proof_blocks)
    {
      for(const block_t& phi_l: m_proof_blocks)
      {
        if(split_block(phi_k, phi_l))
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
  block_t find_initial_block(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr)
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
  void find_reachable_blocks(const bool& only_check_proof_blocks)
  {
    std::list<block_t> unreachable(m_proof_blocks.begin(),m_proof_blocks.end());
    if(!only_check_proof_blocks)
    {
      unreachable.insert(unreachable.end(), m_other_blocks.begin(), m_other_blocks.end());
    }
    std::queue<block_t> open_set;

    // Search for the block that contains the initial state
    block_t initial_block = find_initial_block(unreachable);
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
      for(std::list<block_t>::const_iterator i = unreachable.begin(); i != unreachable.end();)
      {
        if(block.has_transition(*i))
        {
          // A transition was found, so potential_succ is reachable
          open_set.push(*i);
          m_proof_blocks.push_back(*i);
          unreachable.erase(i++);
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
      // split_logger->mark_deleted(rewr(block));
      mCRL2log(log::verbose) << "  block " << i << "\n" << pp(block) << std::endl;
      i++;
    }
  }

  /**
   * \brief Build the LTS that follows from the current
   * partition.
   * \detail Build the LTS that contains one state for each
   * block in the partition. If the partition is stable,
   * then this LTS is minimal under bisimulation.
   */
  template <typename Container>
  bes::boolean_equation_system make_bes(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr)
  {
    set_identifier_generator id_gen;
    std::map< block_t, bes::boolean_variable > var_map;
    for(const block_t& block: blocks)
    {
      var_map.insert(std::make_pair(block, bes::boolean_variable(id_gen(std::string(block.name())))));
    }

    std::vector< bes::boolean_equation > equations;
    for(const block_t& src: blocks)
    {
      std::set<bes::boolean_expression> right_hand_side;
      for(const block_t& dest: blocks)
      {
        if(src.has_transition(dest))
        {
          right_hand_side.insert(var_map[dest]);
        }
      }

      bes::boolean_expression rhs_expr = src.is_conjunctive() ? bes::join_and(right_hand_side.begin(), right_hand_side.end()) : bes::join_or(right_hand_side.begin(), right_hand_side.end());
      equations.emplace_back(src.fixpoint_symbol(), var_map[src], rhs_expr);
    }

    // Search for the initial block
    const block_t& initial_block = find_initial_block(blocks);
    return bes::boolean_equation_system(equations, var_map[initial_block]);
  }

  void make_rank_map()
  {
    pbes_system::fixpoint_symbol previous_symbol(pbes_system::fixpoint_symbol::nu());
    priority_t rank = 0;
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
  void make_pg(ParityGame& pg, const std::list<block>& blocks) const
  {
    StaticGraph::edge_list edges;
    ParityGameVertex* node_attributes = new ParityGameVertex[blocks.size()];
    int src_index = 0;
    for(const auto& src: blocks)
    {
      node_attributes[src_index].player = src.is_conjunctive() ? player_t::PLAYER_ODD : player_t::PLAYER_EVEN;
      node_attributes[src_index].priority = src.rank(m_rank_map);

      bool has_outgoing_edge = false;
      int dest_index = 0;
      for(const auto& dest: blocks)
      {
        if(src.has_transition(dest))
        {
          edges.emplace_back(src_index, dest_index);
          has_outgoing_edge = true;
        }
        dest_index++;
      }

      if(!has_outgoing_edge)
      {
        // All nodes should have an outgoing edge, since SRF requires
        // a summand to either X_true or X_false in every right-hand side
        throw mcrl2::runtime_error("Did not find an outgoing edge for the block " + pp(src));
      }
      src_index++;
    }

    StaticGraph underlying_graph;
    underlying_graph.assign(edges, StaticGraph::EdgeDirection::EDGE_BIDIRECTIONAL);
    pg.assign(underlying_graph, node_attributes);
  }

  /**
   * \brief Print the current partition to stdout
   */
  template <typename Container>
  void print_partition(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr)
  {
    int i = 0;
    for(const block_t& block: blocks)
    {
      mCRL2log(log::verbose) << YELLOW(THIN) << "  block " << i << "\n" << NORMAL << block;
      ++i;
    }
  }

  void make_initial_partition()
  {
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

public:

  dependency_graph_partition(const pbes_system::detail::ppg_pbes& spec, const rewriter& r, const rewriter& pr, const simplifier_mode& simpl_mode)
  : m_spec(spec)
  , m_dm(r, pr, spec.data())
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
    make_initial_partition();
    make_rank_map();
    print_partition(m_proof_blocks);
    // Make sure the initial block is the first in the list
    const block_t& initial_block = find_initial_block(m_proof_blocks);
    m_proof_blocks.erase(std::find(m_proof_blocks.begin(), m_proof_blocks.end(), initial_block));
    m_proof_blocks.push_front(initial_block);
  }

  std::list< block_t > get_proof_blocks()
  {
    return m_proof_blocks;
  }

  std::list< block_t > get_other_blocks()
  {
    return m_other_blocks;
  }

  void set_proof(const std::set< verti >& proof_nodes)
  {
    // Move all the blocks to the other set while keeping the order m_proof_block ++ m_other_blocks
    std::list< block_t > all_blocks = m_proof_blocks;
    // all_blocks.swap(m_proof_blocks);
    all_blocks.insert(all_blocks.end(), m_other_blocks.begin(), m_other_blocks.end());
    m_proof_blocks.clear();
    m_other_blocks.clear();

    // Move the requested blocks to the main set
    verti i = 0;
    for(const block_t& b: all_blocks)
    {
      if(proof_nodes.find(i) != proof_nodes.end())
      {
        m_proof_blocks.push_back(b);
      }
      else
      {
        m_other_blocks.push_back(b);
      }
      i++;
    }
  }

  void get_reachable_pg(ParityGame& pg)
  {
    make_pg(pg, m_proof_blocks);
  }

  void save_bes()
  {
    const bes::boolean_equation_system& bes = make_bes(m_proof_blocks);
    std::ofstream out;
    out.open("out.bes");
    bes.save(out);
    out.close();
  }

  /**
   * Refine the partition num_steps times
   * When num_steps is 0, the algorithm only
   * terminates when the partition is stable.
   * \return true when the partition was already stable
   */
  bool refine_n_steps(std::size_t num_steps)
  {
    // mCRL2log(log::verbose) << "Initial partition:" << std::endl;
    // print_partition(m_proof_blocks);
    std::size_t num_iterations = 0;
    while(refine_step())
    {
      mCRL2log(log::verbose) << GREEN(THIN) << "Partition proof blocks:" << NORMAL << std::endl;
      print_partition(m_proof_blocks);
      mCRL2log(log::verbose) << GREEN(THIN) << "Partition other blocks:" << NORMAL << std::endl;
      print_partition(m_other_blocks);
      num_iterations++;
      if(num_steps != 0 && num_iterations >= num_steps)
      {
        break;
      }
      // if(num_steps == 0)
      // {
      //   make_bes(m_proof_blocks);
      // }
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
    if(num_iterations == 0)
    {
      mCRL2log(log::verbose) << "Final partition:" << std::endl;
      print_partition(m_proof_blocks);
    }
    return num_iterations == 0;
  }

};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIM_PARTITION_H
