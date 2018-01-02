// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_PBESSYMBOLICBISIm_proof_blocks_H
#define MCRL2_PBESSYMBOLICBISIm_proof_blocks_H

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
#ifndef DBM_PACKAGE_AVAILABLE
  #define THIN       "0"
  #define BOLD       "1"
  #define GREEN(S)  "\033[" S ";32m"
  #define YELLOW(S) "\033[" S ";33m"
  #define RED(S)    "\033[" S ";31m"
  #define NORMAL    "\033[0;0m"
#endif
#include "ppg_parser.h"


namespace mcrl2
{
namespace data
{

class dependency_graph_partition
{
  typedef rewriter::substitution_type substitution_t;
  typedef std::pair< pbes_system::propositional_variable, data_expression > block_t;
  typedef pbes_system::detail::ppg_summand summand_type_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;
  typedef pbes_system::detail::ppg_pbes pbes_type_t;

protected:
  pbes_system::detail::ppg_pbes m_spec;

  rewriter rewr;
  rewriter proving_rewr;

  std::list< block_t >      m_proof_blocks;
  std::list< block_t >      m_other_blocks;
  std::map<pbes_system::propositional_variable, simplifier*>                    simpl;
  smt::solver        smt_solver;

  typedef std::set< std::tuple< block_t, block_t, summand_type_t > > refinement_cache_t;
  refinement_cache_t refinement_cache;
  typedef std::map< std::pair< block_t, block_t >, bool > reachability_cache_t;
  reachability_cache_t reachability_cache;
  typedef std::map< std::tuple< block_t, block_t, summand_type_t >, bool> transition_cache_t;
  transition_cache_t transition_cache;

  std::string pp(const block_t& block)
  {
    std::ostringstream out;
    out << block.first.name() << " - " << data::pp(rewr(block.second));
    return out.str();
  }

  /**
   * \brief Split block phi_k on phi_l wrt summand as
   */
  bool split_block(const block_t& phi_k, const block_t& phi_l, const equation_type_t& eq, const summand_type_t& cl)
  {
    if(refinement_cache.find(std::make_tuple(phi_k, phi_l, cl)) != refinement_cache.end())
    {
      // This split has already been tried before.
      return false;
    }
    // Search the cache for information on this transition
    if(!transition_exists(phi_k, phi_l, cl))
    {
      // Cache entry found
      return false;
    }

    data_expression transition_exists = one_point_rule_rewrite(quantifiers_inside_rewrite(
      make_abstraction(exists_binder(), cl.quantification_domain(),
        sort_bool::and_(
          cl.condition(),
          make_application(phi_l.second, cl.new_state().parameters())
        )
      )));
    transition_exists = rewr(replace_data_expressions(transition_exists, fourier_motzkin_sigma(rewr), true));
    data_expression block2_body = rewr(
        sort_bool::and_(
          make_application(phi_k.second, eq.variable().parameters()),
          sort_bool::not_(transition_exists)
        ));
    if(!is_satisfiable(eq.variable().parameters(), block2_body))
    {
      // There are no (X,v) without transitions in phi_k
      // Therefore, we cannot split
      return false;
    }
    data_expression block2 = 
      make_abstraction(lambda_binder(), eq.variable().parameters(),
        block2_body
      );
    block2 = simpl.at(phi_k.first)->apply(block2);
    data_expression block1 = 
      make_abstraction(lambda_binder(), eq.variable().parameters(),
        sort_bool::and_(
          make_application(phi_k.second, eq.variable().parameters()),
          transition_exists
        )
      );

    block1 = simpl.at(phi_k.first)->apply(block1);
    if(block1 == make_abstraction(lambda_binder(), eq.variable().parameters(), sort_bool::false_()))
    {
      // data_expression is_succ =
      //     rewr(sort_bool::and_(
      //       make_application(phi_k.second, phi_k.first.parameters()),
      //       sort_bool::and_(
      //         cl.condition(),
      //         rewr(make_application(phi_l.second, cl.new_state().parameters()))
      //       )
      //     ))
      //   ;

      // smt::smt_problem problem;
      // for(const variable& v: phi_k.first.parameters() + cl.quantification_domain())
      // {
      //   problem.add_variable(v);
      // }
      // problem.add_assertion(is_succ);
      // smt::data_specification* spec = new smt::smt4_data_specification(m_spec.data());
      // std::string smt_instance(spec->generate_data_specification() + "\n" + spec->generate_smt_problem(problem));
      throw mcrl2::runtime_error("Found an empty block1\n" + pp(phi_k) + "\n" + pp(phi_l) + "\n"
       + pbes_system::detail::pp(cl, eq.is_conjunctive()));
    }
    
    // Update the caches and the partition
    refinement_cache.insert(std::make_tuple(phi_k, phi_l, cl));
    m_proof_blocks.remove(phi_k);
    update_caches(phi_k, std::make_pair(phi_k.first, block1), std::make_pair(phi_k.first, block2));
    transition_cache.insert(std::make_pair(std::make_tuple(std::make_pair(phi_k.first, block1), phi_l, cl), true));
    transition_cache.insert(std::make_pair(std::make_tuple(std::make_pair(phi_k.first, block2), phi_l, cl), false));

    // block_tree* logging_node_phi_k = split_logger->find(rewr(phi_k));
    m_proof_blocks.push_front(std::make_pair(phi_k.first, block1));
    m_proof_blocks.push_front(std::make_pair(phi_k.first, block2));
    // logging_node_phi_k->add_child(rewr(block1));
    // logging_node_phi_k->add_child(rewr(block2));
    return true;
  }

  /**
   * \brief Update caches, adding information about the new blocks.
   * \details New entries are deduced from existng entries
   * refering to phi_k. phi_k is the parent of the new blocks.
   */
  void update_caches(const block_t& phi_k, const block_t& block1, const block_t& block2)
  {
    for(std::list< block_t >::const_iterator i = m_proof_blocks.begin() != m_proof_blocks.end() ? m_proof_blocks.begin() : m_other_blocks.begin(); i != m_other_blocks.end();)
    {
      const block_t& phi_l1 = *i;
      for(const summand_type_t& cl1: find_equation(phi_l1.first).summands())
      {
        transition_cache_t::iterator find_result =
          transition_cache.find(std::make_tuple(phi_k, phi_l1, cl1));
        if(find_result != transition_cache.end() && !find_result->second)
        {
          // phi_k has no transition to phi_l1 based on cl1.
          // We add the new blocks to the transition cache
          transition_cache.insert(std::make_pair(std::make_tuple(block1, phi_l1, cl1), false));
          transition_cache.insert(std::make_pair(std::make_tuple(block2, phi_l1, cl1), false));
        }
        transition_cache.erase(std::make_tuple(phi_k, phi_l1, cl1));

        find_result =
          transition_cache.find(std::make_tuple(phi_l1, phi_k, cl1));
        if(find_result != transition_cache.end() && !find_result->second)
        {
          // phi_l1 has no transitions to phi_k based on cl1.
          // We add the new blocks to the transition cache
          transition_cache.insert(std::make_pair(std::make_tuple(phi_l1, block1, cl1), false));
          transition_cache.insert(std::make_pair(std::make_tuple(phi_l1, block2, cl1), false));
        }
        transition_cache.erase(std::make_tuple(phi_l1, phi_k, cl1));

        if(refinement_cache.find(std::make_tuple(phi_k, phi_l1, cl1)) != refinement_cache.end())
        {
          // phi_k is stable wrt phi_l1 and cl1, so new blocks (which are contained in phi_k)
          // are also stable wrt phi_l1 and cl1.
          // We add the new blocks to the refinement cache
          refinement_cache.insert(std::make_tuple(block1, phi_l1, cl1));
          refinement_cache.insert(std::make_tuple(block2, phi_l1, cl1));
          refinement_cache.erase(std::make_tuple(phi_k, phi_l1, cl1));
        }
        refinement_cache.erase(std::make_tuple(phi_l1, phi_k, cl1));
        // We now check whether phi_l1 has transitions to the new blocks.
        // If not, phi_l1 is also stable wrt the new block
        if(refinement_cache.find(std::make_tuple(phi_l1, block1, cl1)) != refinement_cache.end() &&
             !transition_exists(phi_l1, block1, cl1))
        {
          refinement_cache.insert(std::make_tuple(phi_l1, block1, cl1));
        }
        if(refinement_cache.find(std::make_tuple(phi_l1, block2, cl1)) != refinement_cache.end() &&
             !transition_exists(phi_l1, block2, cl1))
        {
          refinement_cache.insert(std::make_tuple(phi_l1, block2, cl1));
        }
        reachability_cache.erase(std::make_pair(phi_k, phi_l1));
        reachability_cache.erase(std::make_pair(phi_l1, phi_k));
      }
      ++i;
      if(i == m_proof_blocks.end())
      {
        i = m_other_blocks.begin();
      }
    }
    for(const summand_type_t& cl: find_equation(phi_k.first).summands())
    {
      transition_cache.erase(std::make_tuple(phi_k, phi_k, cl));
    }
  }

  const equation_type_t& find_equation(const pbes_system::propositional_variable& v)
  {
    std::vector<pbes_system::detail::ppg_equation>::const_iterator result = std::find_if(m_spec.equations().begin(), m_spec.equations().end(), [&](const equation_type_t& e){return e.variable() == v;});
    if(result == m_spec.equations().end())
    {
      throw mcrl2::runtime_error("Equation for variable " + core::pp(v.name()) + " not found.\naterm: " + atermpp::pp(v));
    }
    return *result;
  }

  /**
   * \brief Try every combination of phi_k, phi_l and action summand to
   * see whether the partition can be refined.
   */
  bool refine_step()
  {
    for(const block_t& phi_k: m_proof_blocks)
    {
      const equation_type_t& eq = find_equation(phi_k.first);
      for(const block_t& phi_l: m_proof_blocks)
      {
        for(const summand_type_t& cl: eq.summands())
        {
          if(cl.new_state().name() == phi_l.first.name() && split_block(phi_k, phi_l, eq, cl))
          {
            return true;
          }
        }
      }
    } // stop when we have investigated all blocks
    return false;
  }

  bool is_satisfiable(const variable_list& vars, const data_expression& expr)
  {
    smt::smt_problem problem;
    for(const variable& v: vars)
    {
      problem.add_variable(v);
    }
    problem.add_assertion(expr);
    try
    {
      return smt_solver.solve(problem);
    }
    catch(const smt::translation_error&)
    {}

    // The SMT solver failed, so we fallback to the rewriter
    data_expression is_sat = make_abstraction(exists_binder(), vars, expr);
    is_sat = rewr(one_point_rule_rewrite(quantifiers_inside_rewrite(is_sat)));
    data_expression succ_result;
    succ_result = rewr(replace_data_expressions(is_sat, fourier_motzkin_sigma(rewr), true));
    return succ_result == sort_bool::true_();
  }

  /**
   * \brief Checks whether a transition between src and dest exists based on as
   */
  bool transition_exists(const block_t& src, const block_t& dest, const summand_type_t& cl)
  {
    // Check whether the predicate variable of the clause is actually matching
    if(cl.new_state().name() != dest.first.name())
    {
      return false;
    }
    // Search the cache for information on this transition
    transition_cache_t::iterator find_result = transition_cache.find(std::make_tuple(src, dest, cl));
    if(find_result != transition_cache.end())
    {
      // Cache entry found
      return find_result->second;
    }

    // This expression expresses whether there is a transition between src and dest based on this summand
    data_expression is_succ =
        rewr(sort_bool::and_(
          make_application(src.second, src.first.parameters()),
          sort_bool::and_(
            cl.condition(),
            rewr(make_application(dest.second, cl.new_state().parameters()))
          )
        ))
      ;

    bool result = is_satisfiable(src.first.parameters() + cl.quantification_domain(), is_succ);

    transition_cache.insert(std::make_pair(std::make_tuple(src, dest, cl), result));
    if(!result)
    {
      refinement_cache.insert(std::make_tuple(src,dest,cl));
    }
    return result;
  }

  /**
   * \brief Finds the block in blocks that contains the initial state
   */
  template <typename Container>
  block_t find_initial_block(const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr)
  {
    for(const block_t& block: blocks)
    {
      if(block.first.name() == m_spec.initial_state().name() && rewr(make_application(block.second, m_spec.initial_state().parameters())) == sort_bool::true_())
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
    std::set<block_t> unreachable(m_proof_blocks.begin(),m_proof_blocks.end());
    if(!only_check_proof_blocks)
    {
      unreachable.insert(m_other_blocks.begin(), m_other_blocks.end());
    }
    std::queue<block_t> open_set;

    // Search for the block that contains the initial state
    block_t initial_block = find_initial_block(unreachable);
    unreachable.erase(unreachable.find(initial_block));
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

      std::set<block_t> new_unreachable = unreachable;
      // Look for possible successors of block in the set of unreachable blocks
      for(const block_t& potential_succ: unreachable)
      {
        bool transition_found = false;

        reachability_cache_t::iterator previous_result =
          reachability_cache.find(std::make_pair(block, potential_succ));
        if(previous_result != reachability_cache.end())
        {
          // Use the information from the cache
          transition_found = previous_result->second;
        }
        else
        {
          for(const summand_type_t& cl: find_equation(block.first).summands())
          {
            if(transition_exists(block, potential_succ, cl))
            {
              transition_found = true;
              break;
            }
          }
        }

        if(transition_found)
        {
          // A transition was found, so potential_succ is reachable
          new_unreachable.erase(new_unreachable.find(potential_succ));
          open_set.push(potential_succ);
          m_proof_blocks.push_back(potential_succ);
        }
        if(previous_result == reachability_cache.end())
        {
          // There was no information in the cache yet, so we add it
          reachability_cache.insert(std::make_pair(std::make_pair(block, potential_succ), transition_found));
        }
      }
      unreachable = new_unreachable;
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
      mCRL2log(log::verbose) << "  block " << i << "  " << pp(block) << std::endl;
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
      var_map.insert(std::make_pair(block, bes::boolean_variable(id_gen(std::string(block.first.name())))));
    }

    std::vector< bes::boolean_equation > equations;
    for(const equation_type_t& eq: m_spec.equations())
    {
      for(const block_t& src: blocks)
      {
        if(eq.variable() == src.first)
        {

          std::set<bes::boolean_expression> right_hand_side;
          for(const block_t& dest: blocks)
          {
            // Find out if there are any transitions at all
            reachability_cache_t::iterator previous_result =
              reachability_cache.find(std::make_pair(src, dest));
            if(previous_result != reachability_cache.end() && !previous_result->second)
            {
              // No transition exists between src and dest
              continue;
            }

            for(const summand_type_t& cl: find_equation(src.first).summands())
            {
              if(transition_exists(src, dest, cl))
              {
                right_hand_side.insert(var_map[dest]);
              }
            }
          }

          bes::boolean_expression rhs_expr = eq.is_conjunctive() ? bes::join_and(right_hand_side.begin(), right_hand_side.end()) : bes::join_or(right_hand_side.begin(), right_hand_side.end());
          equations.push_back(bes::boolean_equation(eq.symbol(), var_map[src], rhs_expr));
        }
      }
    }

    // Search for the initial block
    const block_t& initial_block = find_initial_block(blocks);
    return bes::boolean_equation_system(equations, var_map[initial_block]);
  }

  template <typename Container>
  void make_pg(ParityGame& pg, const Container& blocks, typename atermpp::enable_if_container<Container, block_t>::type* = nullptr)
  {
    std::map< block_t, int > node_map;
    int i = 0;
    for(const block_t& block: blocks)
    {
      node_map.insert(std::make_pair(block, i));
      i++;
    }
    std::map< pbes_system::detail::ppg_equation, priority_t > rank_map;
    pbes_system::fixpoint_symbol last_symbol(pbes_system::fixpoint_symbol::nu());
    priority_t rank = 0;
    for(const equation_type_t& eq: m_spec.equations())
    {
      if(eq.symbol() != last_symbol)
      {
        last_symbol = eq.symbol();
        rank++;
      }
      rank_map.insert(std::make_pair(eq, rank));
    }

    StaticGraph::edge_list edges;
    ParityGameVertex* node_attributes = new ParityGameVertex[blocks.size()];
    for(const block_t& src: blocks)
    {
      const equation_type_t& eq = find_equation(src.first);
      node_attributes[node_map[src]].player = eq.is_conjunctive() ? player_t::PLAYER_ODD : player_t::PLAYER_EVEN;
      node_attributes[node_map[src]].priority = rank_map[eq];

      bool has_outgoing_edge = false;
      for(const block_t& dest: blocks)
      {
        // Find out if there are any transitions at all
        reachability_cache_t::iterator previous_result =
          reachability_cache.find(std::make_pair(src, dest));
        if(previous_result != reachability_cache.end() && !previous_result->second)
        {
          // No transition exists between src and dest
          continue;
        }

        bool transition_found = false;
        for(const summand_type_t& cl: eq.summands())
        {
          if(transition_exists(src, dest, cl))
          {
            edges.push_back(std::make_pair(node_map[src], node_map[dest]));
            has_outgoing_edge = true;
            transition_found = true;
            break;
          }
        }
        if(previous_result == reachability_cache.end())
        {
          // There was no information in this cache yet, so we add it
          reachability_cache.insert(std::make_pair(std::make_pair(src, dest), transition_found));
        }
      }

      if(!has_outgoing_edge)
      {
        // All nodes should have an outgoing edge, since SRF requires
        // a summand to either X_true or X_false in every right-hand side
        throw mcrl2::runtime_error("Did not find an outgoing edge for the block " + pp(src));
      }
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
      mCRL2log(log::verbose) << YELLOW(THIN) << "  block " << i << "  " << NORMAL << pp(block) << std::endl;
      // detail::BDD2Dot bddwriter;
      // bddwriter.output_bdd(atermpp::down_cast<abstraction>(block).body(), ("block" + std::to_string(i) + ".dot").c_str());
      i++;
    }
  }

  data_expression make_abstraction(const binder_type& b, const variable_list& vars, const data_expression& expr)
  {
    return vars.empty() ? expr : abstraction(b, vars, expr);
  }

  template <typename Container>
  data_expression make_application(const data_expression& func, const Container& args, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {
    return args.empty() ? func : application(func, args);
  }

public:

  dependency_graph_partition(const pbes_system::detail::ppg_pbes& spec, const rewriter& r, const rewriter& pr)
  : m_spec(spec)
  , rewr(r)
  , proving_rewr(pr)
  , smt_solver(new smt::smt4_data_specification(spec.data()))
  {
    for(const equation_type_t& eq: m_spec.equations())
    {
      simpl.insert(std::make_pair(eq.variable(), get_simplifier_instance(rewr, proving_rewr, eq.variable().parameters(), m_spec.data())));
    }
    for(const equation_type_t& eq: m_spec.equations())
    {
      m_proof_blocks.push_back(std::make_pair(eq.variable(), make_abstraction(lambda_binder(), eq.variable().parameters(), sort_bool::true_())));
    }
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
    std::list< block_t > all_blocks;
    all_blocks.swap(m_proof_blocks);
    all_blocks.insert(all_blocks.end(), m_other_blocks.begin(), m_other_blocks.end());
    // m_proof_blocks.clear();
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
      // Check reachability only in m_proof_blocks
      find_reachable_blocks(true);
      mCRL2log(log::verbose) << "End of a refinement step " << num_iterations << std::endl;
    }
    // Check reachability in the full graph
    find_reachable_blocks(false);
    if(num_iterations == 0)
    {
      mCRL2log(log::verbose) << "Final partition:" << std::endl;
      print_partition(m_proof_blocks);
    }
    // std::set< data_expression > final_partition;
    // std::for_each(partition.begin(), partition.end(), [&](const block_t& block){ final_partition.insert(rewr(block)); });
    // split_logger->output_dot("split_tree.dot", final_partition);
    return num_iterations == 0;
  }

};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIm_proof_blocks_H
