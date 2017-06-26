// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H

// #define DBM_PACKAGE_AVAILABLE 1

#include <string>
#include <queue>
#include <ctime>
#include <chrono>

#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/linear_inequalities_utilities.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/bdd2dot.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/substitutions/data_expression_assignment.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/utilities/logger.h"

#include "simplifier.h"
#ifdef DBM_PACKAGE_AVAILABLE
  #include "simplifier_dbm.h"
#else
  #include "simplifier_fourier_motzkin.h"
  #define GREEN(C) ""
  #define YELLOW(C) ""
  #define RED(C) ""
  #define NORMAL ""
#endif
#include "find_linear_inequality.h"
#include "enumerate_block_union.h"
#include "block_tree.h"


namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

template <typename Specification>
class symbolic_bisim_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  typedef rewriter::substitution_type substitution_t;
  using super::m_spec;

protected:
  const rewrite_strategy strat;
  rewriter rewr;
  rewriter proving_rewr;

  variable_list                  process_parameters;
  data_expression                invariant;
  std::list<data_expression>      partition;
  data_specification             ad_hoc_data;
  simplifier*                    simpl;

  std::set< std::tuple< data_expression, data_expression, lps::action_summand > > refinement_cache;
  std::map< std::pair< data_expression, data_expression >, bool > reachability_cache;
  std::map< std::tuple< data_expression, data_expression, lps::action_summand >, bool > transition_cache;

  int refinement_cache_hits = 0;
  int refinement_cache_misses = 0;
  int transition_cache_hits = 0;
  int transition_cache_misses = 0;
  block_tree* split_logger;

  std::map< lps::action_summand, variable_list > m_primed_summation_variables_map;
  std::map< lps::action_summand, data_expression_list > m_updates_map;
  std::map< lps::action_summand, substitution_t > m_primed_substitution_map;
  variable_list m_primed_process_parameters;

  /**
   * \brief Pretty print all expressions in a container, delimited
   * with commas and surrounded with square brackets.
   */
  template <typename Container>
  std::string pp_container(Container& expressions, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
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

  /**
   * \brief Reconstructs the rewriter with some useful rules
   */
  void add_ad_hoc_rules()
  {
    ad_hoc_data = merge_data_specifications(m_spec.data(),simplifier::norm_rules_spec());
    rewr = rewriter(ad_hoc_data, strat);
  }

  /**
   * \brief Finds subexpressions over free variables of sort
   * Real and replace them with booleans.
   * \detail This functions searches for linear inequalities over
   * free real variables. It will replace each of them with a boolean
   * variable. A list of the new boolean variables is returned.
   * The parameter [enumeration_condition] contains an expression 
   * that represents that relation between the replaced linear inequalities
   * and the booleans. It has the shape (b1 <=> (x1 < r2)) && (b1 <=> (x2 < r2))...
   * where b1 and b2 are boolean variables, x1 and x2 are real variables and r1 and
   * r2 are real constants.
   */
  variable_list replace_free_reals(data_expression& split_block,
    std::set< data_expression > linear_inequalities, data_expression& enumeration_condition, data_expression& lli)
  {
    set_identifier_generator var_gen;
    var_gen.add_identifiers(find_identifiers(split_block));
    variable_vector replacements;
    for(data_expression d: linear_inequalities)
    {
      variable rep_bool(var_gen("b"), sort_bool::bool_());
      replacements.push_back(rep_bool);
    }
    data_expression replaced_relation = sort_bool::true_();

    uint32_t i = 0;
    for(data_expression d: linear_inequalities)
    {
      split_block = replace_data_expressions(split_block, data_expression_assignment(d, replacements[i]), true);
      enumeration_condition = replace_data_expressions(enumeration_condition, data_expression_assignment(d, replacements[i]), true);
      if(linear_inequalities.size() > 1)
      {
        replaced_relation = lazy::and_(replaced_relation, iff(replacements[i], d));
        if(i == linear_inequalities.size() - 1)
        {
          lli = d;
        }
      }
      i++;
    }
    enumeration_condition = lazy::and_(enumeration_condition, replaced_relation);
    return variable_list(replacements.begin(), replacements.end());
  }

  /** 
   * \brief For all evaluations of variables in vars for which parent_block holds,
   * add split_block to the partition
   * \param vars The set of free variables in split_block
   */ 
  void enumerate(data_expression parent_block, variable_list vars, data_expression split_block, const data_expression& lli, std::set< data_expression >& new_part)
  {

    typedef enumerator_algorithm_with_iterator<rewriter, enumerator_list_element_with_substitution<>, enumerator_identifier_generator, enumerate_filter_print> enumerator_type;
    enumerator_identifier_generator id_generator;
    enumerator_type enumerator(rewr, m_spec.data(), rewr, id_generator, (std::numeric_limits<std::size_t>::max)(), true);

    data::mutable_indexed_substitution<> sigma;
    std::deque<enumerator_list_element_with_substitution<> >
         enumerator_deque(1, enumerator_list_element_with_substitution<>(vars, parent_block));
    enumerate_filter_print filter(lli, rewr);
    for (typename enumerator_type::iterator i = enumerator.begin(sigma, enumerator_deque, filter); i != enumerator.end(filter); ++i)
    {
      i->add_assignments(vars,sigma,rewr);

      // Simplify the new block and add it to the result if it is not empty (false)
      data_expression new_block(simpl->apply(split_block, sigma));
      if(new_block != lambda(process_parameters, sort_bool::false_()))
      {
        new_part.insert(lambda(process_parameters, proving_rewr(atermpp::down_cast<abstraction>(new_block).body())));
      }
    }
  }

  /**
   * \brief Split block phi_k on phi_l wrt summand as
   */
  bool split_block(const data_expression& phi_k, const data_expression& phi_l, const lps::action_summand& as)
  {
    if(refinement_cache.find(std::make_tuple(phi_k, phi_l, as)) != refinement_cache.end())
    {
      refinement_cache_hits++;
      // This split has already been tried before.
      return false;
    }
    refinement_cache_misses++;

    substitution_t sub_primed = m_primed_substitution_map[as];
    variable_list primed_summation_variables = m_primed_summation_variables_map[as];
    data_expression_list updates = m_updates_map[as];

    data_expression arguments_equal = sort_bool::true_();
    // Check whether there is at least one action, since we cannot request
    // arguments from an empty list of actions. The empty multi action represents
    // tau.
    if(!as.multi_action().actions().empty())
    {
      for(const data_expression& expr: as.multi_action().arguments())
      {
        arguments_equal = lazy::and_(arguments_equal, equal_to(expr, rewr(expr, sub_primed)));
      }
    }
    arguments_equal = rewr(arguments_equal);
    // std::cout << "Constructing block expression ..." << std::endl;
    data_expression prime_condition =
      arguments_equal == sort_bool::true_() ?
        variable("b'", sort_bool::bool_()) :
        lazy::and_(
          replace_variables(as.condition(), sub_primed),
          application(phi_l, replace_variables(updates, sub_primed))
        );
    data_expression split_block =
      lambda(process_parameters,
        lazy::and_(
          rewr(application(phi_k,process_parameters)),
          lazy::and_(
            forall(primed_summation_variables,
              lazy::implies(
                rewr(prime_condition),
                exists(as.summation_variables(),
                  rewr(lazy::and_(
                    lazy::and_(
                      as.condition(),
                      rewr(application(phi_l, updates))),
                    arguments_equal
                  ))
                )
              )
            ),
            forall(as.summation_variables(),
              lazy::implies(
                rewr(lazy::and_(
                  as.condition(),
                  rewr(application(phi_l, updates))
                )),
                exists(primed_summation_variables,
                  rewr(lazy::and_(
                    prime_condition,
                    arguments_equal
                  ))
                )
              )
            )
          )
        )
      );

    split_block = one_point_rule_rewrite(quantifiers_inside_rewrite(split_block));
    data_expression result = replace_data_expressions(split_block, fourier_motzkin_sigma(rewr), true);
    split_block = rewr(result);

    // Find the free variables (the primed variables)
    std::set<variable> free_vars = find_free_variables(split_block);
    variable_list free_vars_list;
    std::set< variable > real_free_vars;
    // Find the free variables of type Real
    for(const variable& v: free_vars)
    {
      if(v.sort() == sort_real::real_())
      {
        real_free_vars.insert(v);
      }
      else
      {
        free_vars_list.push_front(v);
      }
    }

    data_expression enumeration_condition(arguments_equal == sort_bool::true_() ? data_expression(sort_bool::true_()) : rewr(application(phi_k,m_primed_process_parameters)));
    data_expression lli(sort_bool::true_());
    if(real_free_vars.size() > 0)
    {
      std::set< data_expression > li = find_linear_inequalities(split_block, real_free_vars);
      free_vars_list = free_vars_list + replace_free_reals(split_block, li, enumeration_condition, lli);
    }

    std::set< data_expression > new_blocks;
    // Enumerate over all values of primed variables to obtain the new blocks
    enumerate(enumeration_condition, free_vars_list, split_block, lli, new_blocks);

    // Update the caches and the partition
    refinement_cache.insert(std::make_tuple(phi_k, phi_l, as));
    if(new_blocks.size() > 1) {
      partition.remove(phi_k);
      update_caches(phi_k, new_blocks);
      block_tree* logging_node_phi_k = split_logger->find(rewr(phi_k));
      for(const data_expression& d: new_blocks)
      {
        partition.push_front(d);
        logging_node_phi_k->add_child(rewr(d));
      }
      return true;
    }
    return false;
  }

  /**
   * \brief Update caches, adding information about the new blocks.
   * \details New entries are deduced from existng entries
   * refering to phi_k. phi_k is the parent of the new blocks.
   */
  void update_caches(data_expression phi_k, std::set<data_expression> new_blocks)
  {
    for(const data_expression& phi_l1: partition)
    {
      for(const lps::action_summand& as1: m_spec.process().action_summands())
      {
        std::map<std::tuple<data_expression, data_expression, lps::action_summand>, bool>::iterator find_result =
          transition_cache.find(std::make_tuple(phi_k, phi_l1, as1));
        if(find_result != transition_cache.end() && !find_result->second)
        {
          // phi_k is stable wrt phi_l1 and as1, so new blocks (which are contained in phi_k)
          // are also stable wrt phi_l1 and as1.
          // We add the new blocks to the refinement cache
          for(const data_expression& new_block: new_blocks)
          {
            transition_cache.insert(std::make_pair(std::make_tuple(new_block, phi_l1, as1), false));
          }
        }
        transition_cache.erase(std::make_tuple(phi_k, phi_l1, as1));

        find_result =
          transition_cache.find(std::make_tuple(phi_l1, phi_k, as1));
        if(find_result != transition_cache.end() && !find_result->second)
        {
          // phi_k is stable wrt phi_l1 and as1, so new blocks (which are contained in phi_k)
          // are also stable wrt phi_l1 and as1.
          // We add the new blocks to the refinement cache
          for(const data_expression& new_block: new_blocks)
          {
            transition_cache.insert(std::make_pair(std::make_tuple(phi_l1, new_block, as1), false));
          }
        }
        transition_cache.erase(std::make_tuple(phi_l1, phi_k, as1));

        if(refinement_cache.find(std::make_tuple(phi_k, phi_l1, as1)) != refinement_cache.end())
        {
          // phi_k is stable wrt phi_l1 and as1, so new blocks (which are contained in phi_k)
          // are also stable wrt phi_l1 and as1.
          // We add the new blocks to the refinement cache
          for(const data_expression& new_block: new_blocks)
          {
            refinement_cache.insert(std::make_tuple(new_block, phi_l1, as1));
          }
          refinement_cache.erase(std::make_tuple(phi_k, phi_l1, as1));
        }
        refinement_cache.erase(std::make_tuple(phi_l1, phi_k, as1));
        // We now check whether phi_l1 has transitions to the new blocks.
        // If not, phi_l1 is also stable wrt the new block
        // for(const data_expression& new_block: new_blocks)
        // {
        //   if(refinement_cache.find(std::make_tuple(phi_l1, new_block, as1)) != refinement_cache.end() &&
        //        !transition_exists(phi_l1, new_block, as1))
        //   {
        //     refinement_cache.insert(std::make_tuple(phi_l1, new_block, as1));
        //   }
        // }
      }
    }
  }

  /**
   * \brief Try every combination of phi_k, phi_l and action summand to
   * see whether the partition can be refined.
   */
  bool refine()
  {
    int i = 0;
    for(const data_expression phi_k: partition)
    {
      int j = 0;
      for(const data_expression phi_l: partition)
      {
        int k = 0;
        for(const lps::action_summand& as: m_spec.process().action_summands())
        {
          // std::cout << "Trying to split " << i << " on " << j << " wrt summand " << k << std::endl;
          if(split_block(phi_k, phi_l, as))
          {
            std::cout << "Split " << rewr(phi_k) << " wrt summand\n" << as << "\non block " << rewr(phi_l) << std::endl;
            return true;
          }
          k++;
        }
        j++;
      }
      i++;
    } // stop when we have investigated all blocks
    return false;
  }

  /**
   * \brief Initialise several data structures based on the information
   * contained in the input LTS.
   */
  void build_summand_maps()
  {
    // Later we will use this to build one substitution per action summand
    substitution_t sub_primed_base;
    for(const variable& v: process_parameters)
    {
      // Create one primed variable per process parameter
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      m_primed_process_parameters.push_front(var);
      sub_primed_base[v] = var;
    }
    m_primed_process_parameters = reverse(m_primed_process_parameters);

    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      // Base the substitution for this action summand on the
      // substitution that was created from the process parameters
      substitution_t sub_primed = sub_primed_base;
      variable_list primed_summation_variables;
      for(const variable& v: as.summation_variables())
      {
        // Create one primed variable per summation variable
        variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
        primed_summation_variables.push_front(var);
        sub_primed[v] = var;
      }
      primed_summation_variables = reverse(primed_summation_variables);

      // A list of updates for each process parameter
      data_expression_list updates;
      for(variable_list::const_iterator it = process_parameters.begin(); it != process_parameters.end(); it++)
      {
        assignment_list::const_iterator assign = std::find_if(as.assignments().begin(), as.assignments().end(), [&it](const assignment& arg) {return arg.lhs() == *it;});
        if(assign != as.assignments().end())
        {
          updates.push_front(assign->rhs());
        }
        else
        {
          updates.push_front(*it);
        }
      }
      updates = reverse(updates);

      m_primed_substitution_map.insert(std::make_pair(as, sub_primed));
      m_primed_summation_variables_map.insert(std::make_pair(as, primed_summation_variables));
      m_updates_map.insert(std::make_pair(as, updates));
    }
  }

  /**
   * \brief Finds the block in blocks that contains the initial state
   */
  template <typename Container>
  data_expression find_initial_block(const Container& blocks, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {
    for(const data_expression& block: blocks)
    {
      if(rewr(application(block, m_spec.initial_process().state(process_parameters))) == sort_bool::true_())
      {
        // std::cout << "Found initial block " << block << std::endl;
        return block;
      }
    }
    throw mcrl2::runtime_error("Initial block not found.");
  }

  /**
   * \brief Checks whether a transition between src and dest exists based on as
   */
  bool transition_exists(const data_expression& src, const data_expression& dest, const lps::action_summand& as)
  {
    // Search the cache for information on this transition
    std::map<std::tuple<data_expression, data_expression, lps::action_summand>, bool>::iterator find_result = transition_cache.find(std::make_tuple(src, dest, as));
    if(find_result != transition_cache.end())
    {
      // Cache entry found
      transition_cache_hits++;
      return find_result->second;
    }
    transition_cache_misses++;

    // This expression expresses whether there is a transition between src and dest based on this summand
    data_expression is_succ =
      exists(m_spec.process().process_parameters() + as.summation_variables(),
        rewr(sort_bool::and_(
          application(src, m_spec.process().process_parameters()),
          sort_bool::and_(
            as.condition(),
            rewr(application(dest, m_updates_map[as]))
          )
        ))
      );

    // Do some rewriting before using Fourier-Motzkin elimination
    is_succ = one_point_rule_rewrite(quantifiers_inside_rewrite(is_succ));
    data_expression succ_result = rewr(replace_data_expressions(is_succ, fourier_motzkin_sigma(rewr), true));
    bool result = succ_result == sort_bool::true_();
    transition_cache.insert(std::make_pair(std::make_tuple(src, dest, as), result));
    if(!result)
    {
      refinement_cache.insert(std::make_tuple(src,dest,as));
    }
    return result;
  }

  /**
   * \brief Computes the reachable blocks.
   * \detail The partition is updated, such that it only contains
   * reachable blocks.
   */
  void find_reachable_blocks()
  {
    std::set<data_expression> unreachable(partition.begin(),partition.end());
    std::queue<data_expression> open_set;

    // Search for the block that contains the initial state
    data_expression initial_block = find_initial_block(unreachable);
    unreachable.erase(unreachable.find(initial_block));
    open_set.push(initial_block);
    partition.clear();
    partition.push_back(initial_block);

    while(!open_set.empty())
    {
      // Take a block from the open set
      const data_expression block = open_set.front();
      open_set.pop();

      std::set<data_expression> new_unreachable = unreachable;
      // Look for possible successors of block in the set of unreachable blocks
      for(const data_expression& potential_succ: unreachable)
      {
        bool transition_found = false;

        std::map< std::pair<data_expression, data_expression>, bool>::iterator previous_result =
          reachability_cache.find(std::make_pair(block, potential_succ));
        if(previous_result != reachability_cache.end())
        {
          // Use the information from the cache
          transition_found = previous_result->second;
        }
        else
        {
          for(const lps::action_summand& as: m_spec.process().action_summands())
          {
            if(transition_exists(block, potential_succ, as))
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
    for(const data_expression& block: unreachable)
    {
      split_logger->mark_deleted(rewr(block));
      std::cout << "  block " << i << "  " << pp(rewr(block)) << std::endl;
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
  lts::lts_lts_t make_lts(const Container& blocks, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {
    lts::lts_lts_t result;
    atermpp::indexed_set<data_expression> indexed_partition;
    for(const data_expression& block: blocks)
    {
      indexed_partition.put(block);
      result.add_state(lts::state_label_lts(lts::state_label_lts::single_label{rewr(block)}));
    }
    atermpp::indexed_set<process::action_list> indexed_actions;
    // tau is always action 0
    indexed_actions.put(lts::action_label_lts::tau_action().actions());
    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      std::pair<std::size_t, bool> action_index = indexed_actions.put(as.multi_action().actions());
      if(action_index.second)
      {
        result.add_action(lts::action_label_lts(as.multi_action()));
      }
    }

    for(const data_expression& src: blocks)
    {
      for(const data_expression& dest: blocks)
      {
        // Find out if there are any transitions at all
        std::map< std::pair<data_expression, data_expression>, bool>::iterator previous_result =
          reachability_cache.find(std::make_pair(src, dest));
        if(previous_result != reachability_cache.end() && !previous_result->second)
        {
          // No transition exists between src and dest
          continue;
        }

        for(const lps::action_summand& as: m_spec.process().action_summands())
        {
          if(transition_exists(src, dest, as))
          {
            lts::transition trans(indexed_partition[src], indexed_actions[as.multi_action().actions()], indexed_partition[dest]);
            result.add_transition(trans);
          }
        }
      }
    }

    // Search for the initial block
    data_expression initial_block = find_initial_block(blocks);
    if(initial_block != data_expression())
    {
      // The intial block was found, set it as intial state
      result.set_initial_state(indexed_partition[initial_block]);
    }
    return result;
  }

  void save_lts()
  {
    const lts::lts_lts_t& lts = make_lts(partition);
    lts.save("out.lts");
  }

  /**
   * \brief Print the current partition to stdout
   */
  template <typename Container>
  void print_partition(const Container& blocks, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
  {
    int i = 0;
    for(const data_expression& block: blocks)
    {
      std::cout << YELLOW(THIN) << "  block " << i << "  " << NORMAL << pp(rewr(block)) << std::endl;
      // detail::BDD2Dot bddwriter;
      // bddwriter.output_bdd(proving_rewr(atermpp::down_cast<abstraction>(block).body()), ("block" + std::to_string(i) + ".dot").c_str());
      i++;
    }
  }

public:
  symbolic_bisim_algorithm(Specification& spec, data_expression inv, const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec)
    , strat(st)
    , rewr(spec.data(),jitty)
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover)
#else
    , proving_rewr(spec.data(), jitty_prover)
#endif
    , invariant(inv)
  {

  }

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;

    process_parameters = m_spec.process().process_parameters();
    partition.push_front(invariant);
    split_logger = new block_tree(invariant);
    add_ad_hoc_rules();
    build_summand_maps();
#ifdef DBM_PACKAGE_AVAILABLE
    simpl = new simplifier_dbm(rewr, proving_rewr, process_parameters, m_spec.data());
#else
    simpl = new simplifier_fourier_motzkin(rewr, proving_rewr);
#endif

    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = std::chrono::high_resolution_clock::now();
    std::cout << "Initial partition:" << std::endl;
    print_partition(partition);
    int num_iterations = 0;
    while(refine())
    {
      std::cout << GREEN(THIN) << "Partition:" << NORMAL << std::endl;
      print_partition(partition);
      find_reachable_blocks();
      num_iterations++;
      std::cout << "End of iteration " << num_iterations << 
      ".\nRefinement cache entries/hits/misses " << refinement_cache.size() << "/" << refinement_cache_hits << "/" << refinement_cache_misses <<
      ".\nTransition cache entries/hits/misses " << transition_cache.size() << "/" << transition_cache_hits << "/" << transition_cache_misses << std::endl;
    }
    std::cout << "Final partition:" << std::endl;
    print_partition(partition);
    std::set< data_expression > final_partition;
    std::for_each(partition.begin(), partition.end(), [&](const data_expression& block){ final_partition.insert(rewr(block)); });
    split_logger->output_dot("split_tree.dot", final_partition);
    std::cout << "Partition refinement completed in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << " seconds" << std::endl;

    save_lts();
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
