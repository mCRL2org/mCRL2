// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include <string>
#include <queue>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/join.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/prover/bdd2dot.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"
// #include "mcrl2/smt/cvc4.h"
// #include "mcrl2/smt/solver.h"
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

using namespace mcrl2::log;

class symbolic_bisim_algorithm
{
  typedef rewriter::substitution_type substitution_t;
  typedef std::pair< pbes_system::propositional_variable, data_expression > block_t;
  typedef pbes_system::detail::ppg_summand summand_type_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;
  typedef pbes_system::detail::ppg_pbes pbes_type_t;

protected:
  pbes_system::detail::ppg_pbes m_spec;
  data_specification m_data_spec;

  const rewrite_strategy strat;
  rewriter rewr;
  rewriter proving_rewr;

  variable_list                  process_parameters;
  const data_expression                invariant;
  std::list< block_t >      partition;
  data_specification             ad_hoc_data;
  std::map<pbes_system::propositional_variable, simplifier*>                    simpl;
  // smt::solver        smt_solver;

  typedef std::set< std::tuple< block_t, block_t, summand_type_t > > refinement_cache_t;
  refinement_cache_t refinement_cache;
  typedef std::map< std::pair< block_t, block_t >, bool > reachability_cache_t;
  reachability_cache_t reachability_cache;
  typedef std::map< std::tuple< block_t, block_t, summand_type_t >, bool> transition_cache_t;
  transition_cache_t transition_cache;

  int refinement_cache_hits = 0;
  int refinement_cache_misses = 0;
  int transition_cache_hits = 0;
  int transition_cache_misses = 0;
  int last_minute_transition_check = 0;
  // block_tree* split_logger;

  /**
   * \brief Pretty print all expressions in a container, delimited
   * with commas and surrounded with square brackets.
   */
  template <typename Container>
  std::string pp_container(const Container& expressions, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {

    std::ostringstream out;
    out << "[";
    for(typename Container::const_iterator it = expressions.begin(); it != expressions.end(); it++)
    {
      if(it != expressions.begin())
      {
        out << ", ";
      }
      out << pp(*it);
    }
    out << "]";
    return out.str();
  }

  std::string pp(const block_t& block)
  {
    std::ostringstream out;
    out << block.first.name() << " - " << data::pp(rewr(block.second));
    return out.str();
  }

  /**
   * \brief Reconstructs the rewriter with some useful rules
   */
  void add_ad_hoc_rules()
  {
    ad_hoc_data = merge_data_specifications(m_data_spec,simplifier::norm_rules_spec());
    rewr = rewriter(ad_hoc_data, strat);
  }

  struct check_less_nat2pos 
  {
    bool found = false;

    data_expression operator()(const data_expression& expr)
    {
      // if(is_less_application(expr) && sort_nat::is_nat2pos_application(sort_nat::right(expr)))
      // {
      //   found = true;
      // }
      if(is_less_application(expr) && search_data_expression(expr, variable("waitingTrain1_Gate", sort_nat::nat())))
      {
        std::cout << RED(THIN) << "Found matching expr" << NORMAL << expr << std::endl;
        found = true;
      }
      return expr;
    }
  };

  bool check_expr_for_less_nat2pos(const data_expression& expr)
  {
    check_less_nat2pos checker;
    replace_data_expressions(expr, checker, true);
    return checker.found;
  }

  bool split_block(const block_t& phi_k, const block_t& phi_l, const equation_type_t& eq)
  {
    std::vector<summand_type_t> summands;
    for(const summand_type_t& summ: eq.summands())
    {
      if(summ.new_state().name() == phi_l.first.name())
      {
        summands.push_back(summ);
      }
    }
    if(summands.empty())
    {
      return false;
    }
    if(refinement_cache.find(std::make_tuple(phi_k, phi_l, summands[0])) != refinement_cache.end())
    {
      refinement_cache_hits++;
      // This split has already been tried before.
      return false;
    }
    refinement_cache_misses++;
    // Search the cache for information on this transition
    bool transition_found = false;
    for(const summand_type_t& summ: summands)
    {
      if(transition_exists(phi_k, phi_l, summ))
      {
        transition_found = true;
        break;
      }
    }
    if(!transition_found)
    {
      last_minute_transition_check++;
      return false;
    }

    data_expression transition_exists = sort_bool::false_();
    for(const summand_type_t& summ: summands)
    {
      transition_exists = lazy::or_(transition_exists,
        make_abstraction(exists_binder(), summ.quantification_domain(),
          sort_bool::and_(
            summ.condition(),
            make_application(phi_l.second, summ.new_state().parameters())
          )
        )
      );
    }
     
    transition_exists = one_point_rule_rewrite(quantifiers_inside_rewrite(transition_exists));
    transition_exists = rewr(replace_data_expressions(transition_exists, fourier_motzkin_sigma(rewr), true));
    data_expression block2 = 
      make_abstraction(lambda_binder(), eq.variable().parameters(),
        sort_bool::and_(
          make_application(phi_k.second, eq.variable().parameters()),
          sort_bool::not_(transition_exists)
        )
      );
    block2 = simpl.at(phi_k.first)->apply(block2);
    if(rewr(block2) == make_abstraction(lambda_binder(), eq.variable().parameters(), sort_bool::false_()))
    {
      // There are no (X,v) without transitions in phi_k
      // Therefore, we cannot split
      return false;
    }
    data_expression block1 = 
      make_abstraction(lambda_binder(), eq.variable().parameters(),
        sort_bool::and_(
          make_application(phi_k.second, eq.variable().parameters()),
          transition_exists
        )
      );

    block1 = simpl.at(phi_k.first)->apply(block1);
    
    // Update the caches and the partition
    refinement_cache.insert(std::make_tuple(phi_k, phi_l, summands[0]));
    partition.remove(phi_k);
    update_caches(phi_k, std::make_pair(phi_k.first, block1), std::make_pair(phi_k.first, block2));
    for(const summand_type_t& summ: summands)
    {
      transition_cache.insert(std::make_pair(std::make_tuple(std::make_pair(phi_k.first, block2), phi_l, summ), false));
    }

    // block_tree* logging_node_phi_k = split_logger->find(rewr(phi_k));
    partition.push_front(std::make_pair(phi_k.first, block1));
    partition.push_front(std::make_pair(phi_k.first, block2));
    // logging_node_phi_k->add_child(rewr(block1));
    // logging_node_phi_k->add_child(rewr(block2));
    return true;
  }

  /**
   * \brief Split block phi_k on phi_l wrt summand as
   */
  bool split_block(const block_t& phi_k, const block_t& phi_l, const equation_type_t& eq, const summand_type_t& cl)
  {
    if(refinement_cache.find(std::make_tuple(phi_k, phi_l, cl)) != refinement_cache.end())
    {
      refinement_cache_hits++;
      // This split has already been tried before.
      return false;
    }
    refinement_cache_misses++;
    // Search the cache for information on this transition
    if(!transition_exists(phi_k, phi_l, cl))
    {
      // Cache entry found
      last_minute_transition_check++;
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
    
    // Update the caches and the partition
    refinement_cache.insert(std::make_tuple(phi_k, phi_l, cl));
    partition.remove(phi_k);
    update_caches(phi_k, std::make_pair(phi_k.first, block1), std::make_pair(phi_k.first, block2));
    transition_cache.insert(std::make_pair(std::make_tuple(std::make_pair(phi_k.first, block1), phi_l, cl), true));
    transition_cache.insert(std::make_pair(std::make_tuple(std::make_pair(phi_k.first, block2), phi_l, cl), false));

    // block_tree* logging_node_phi_k = split_logger->find(rewr(phi_k));
    partition.push_front(std::make_pair(phi_k.first, block1));
    partition.push_front(std::make_pair(phi_k.first, block2));
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
    for(const block_t& phi_l1: partition)
    {
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
      std::cout << atermpp::pp(v) << std::endl;
      throw mcrl2::runtime_error("Equation for variable " + core::pp(v.name()) + " not found.\naterm: " + atermpp::pp(v));
    }
    return *result;
  }

  /**
   * \brief Try every combination of phi_k, phi_l and action summand to
   * see whether the partition can be refined.
   */
  bool refine()
  {
    bool m_cluster_summands = false;
    for(const block_t& phi_k: partition)
    {
      const equation_type_t& eq = find_equation(phi_k.first);
      for(const block_t& phi_l: partition)
      {
        if(m_cluster_summands)
        {
          if(split_block(phi_k, phi_l, eq))
          {
            return true;
          }
        }
        else
        {
          for(const summand_type_t& cl: eq.summands())
          {
            if(cl.new_state().name() == phi_l.first.name() && split_block(phi_k, phi_l, eq, cl))
            {
              return true;
            }
          }
        }
      }
    } // stop when we have investigated all blocks
    return false;
  }

  bool is_satisfiable(const variable_list& vars, const data_expression& expr)
  {
    // smt::smt_problem problem;
    // for(const variable& v: vars)
    // {
    //   problem.add_variable(v);
    // }
    // problem.add_assertion(expr);
    // return smt_solver.solve(problem);
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
      transition_cache_hits++;
      return find_result->second;
    }
    transition_cache_misses++;

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
    // std::cout << "Searching for initial block" << std::endl;
    for(const block_t& block: blocks)
    {
      // std::cout << block.first << " - " << rewr(application(block.second, m_spec.initial_state.parameters())) << std::endl;
      if(block.first.name() == m_spec.initial_state().name() && rewr(make_application(block.second, m_spec.initial_state().parameters())) == sort_bool::true_())
      {
        // std::cout << "Found initial block " << block << std::endl;
        return block;
      }
    }
    throw mcrl2::runtime_error("Initial block not found.");
  }

  /**
   * \brief Computes the reachable blocks.
   * \detail The partition is updated, such that it only contains
   * reachable blocks.
   */
  void find_reachable_blocks()
  {
    std::set<block_t> unreachable(partition.begin(),partition.end());
    std::queue<block_t> open_set;

    // Search for the block that contains the initial state
    block_t initial_block = find_initial_block(unreachable);
    unreachable.erase(unreachable.find(initial_block));
    open_set.push(initial_block);
    partition.clear();
    partition.push_back(initial_block);

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
          partition.push_back(potential_succ);
        }
        if(previous_result == reachability_cache.end())
        {
          // There was no information in the cache yet, so we add it
          reachability_cache.insert(std::make_pair(std::make_pair(block, potential_succ), transition_found));
        }
      }
      unreachable = new_unreachable;
    }


    std::cout << RED(THIN) << "Unreachable blocks:" << NORMAL << std::endl;
    int i = 0;
    for(const block_t& block: unreachable)
    {
      // split_logger->mark_deleted(rewr(block));
      std::cout << "  block " << i << "  " << pp(block) << std::endl;
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

  void save_bes()
  {
    const bes::boolean_equation_system& bes = make_bes(partition);
    std::ofstream out;
    out.open("out.bes");
    bes.save(out);
    out.close();
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
      std::cout << YELLOW(THIN) << "  block " << i << "  " << NORMAL << pp(block) << std::endl;
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
  symbolic_bisim_algorithm(const pbes_system::pbes& spec, const data_expression& inv, const rewrite_strategy& st = jitty)
    : m_spec(spec)
    , m_data_spec(spec.data())
    , strat(st)
    , rewr(spec.data(),jitty)
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover)
#else
    , proving_rewr(spec.data(), jitty_prover)
#endif
    , invariant(inv)
    // , smt_solver(new smt::smt4_data_specification(spec.data()))
  {}

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;

    for(const equation_type_t& eq: m_spec.equations())
    {
      partition.push_back(std::make_pair(eq.variable(), make_abstraction(lambda_binder(), eq.variable().parameters(), sort_bool::true_())));
    }
    // split_logger = new block_tree(invariant);
    add_ad_hoc_rules();
    m_spec = m_spec.simplify(rewr);
    std::cout << m_spec << std::endl;
    
    for(const equation_type_t& eq: m_spec.equations())
    {
      simpl.insert(std::make_pair(eq.variable(), get_simplifier_instance(rewr, proving_rewr, eq.variable().parameters(), m_data_spec)));
    }

    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = std::chrono::high_resolution_clock::now();
    std::cout << "Initial partition:" << std::endl;
    print_partition(partition);
    find_reachable_blocks();
    int num_iterations = 0;
    while(refine())
    {
      std::cout << GREEN(THIN) << "Partition:" << NORMAL << std::endl;
      print_partition(partition);
      find_reachable_blocks();
      num_iterations++;
      std::cout << "End of iteration " << num_iterations << 
      ".\nRefinement cache entries/hits/misses " << refinement_cache.size() << "/" << refinement_cache_hits << "/" << refinement_cache_misses <<
      ".\nTransition cache entries/hits/misses " << transition_cache.size() << "/" << transition_cache_hits << "/" << transition_cache_misses <<
      ".\nReachability cache entries           " << reachability_cache.size() <<
      ".\nLast minute transition check successes " << last_minute_transition_check <<
      ".\nSplits performed " << refinement_cache_misses - last_minute_transition_check << std::endl;
    }
    std::cout << "Final partition:" << std::endl;
    print_partition(partition);
    // std::set< data_expression > final_partition;
    // std::for_each(partition.begin(), partition.end(), [&](const block_t& block){ final_partition.insert(rewr(block)); });
    // split_logger->output_dot("split_tree.dot", final_partition);
    std::cout << "Partition refinement completed in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << " seconds" << std::endl;

    save_bes();
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
