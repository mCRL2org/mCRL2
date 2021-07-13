// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include <queue>

#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/bdd2dot.h"
#include "mcrl2/data/enumerator_with_iterator.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/substitutions/data_expression_assignment.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/smt/solver.h"
#include "mcrl2/smt/translation_error.h"

#include "../pbessymbolicbisim/simplifier.h"
#define THIN       "0"
#define BOLD       "1"
#define GREEN(S)  "\033[" S ";32m"
#define YELLOW(S) "\033[" S ";33m"
#define RED(S)    "\033[" S ";31m"
#define NORMAL    "\033[0;0m"
#include "find_linear_inequality.h"
#include "enumerate_block_union.h"

namespace std
{

/// \brief specialization of the standard std::hash function.
template<>
struct hash<std::tuple<mcrl2::data::data_expression, mcrl2::data::data_expression, mcrl2::lps::action_summand> >
{
  std::size_t operator()(const std::tuple<mcrl2::data::data_expression, mcrl2::data::data_expression, mcrl2::lps::action_summand>& x) const
  {
    // The hashing function below is taken from boost (http://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html).
    std::size_t seed=std::hash<atermpp::aterm>()(get<0>(x));
    seed = std::hash<atermpp::aterm>()(get<1>(x)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed = std::hash<atermpp::aterm>()(get<2>(x).multi_action().actions()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    if(!get<2>(x).multi_action().actions().empty())
    {
      seed = std::hash<atermpp::aterm>()(get<2>(x).multi_action().actions().front().arguments()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed = std::hash<atermpp::aterm>()(get<2>(x).assignments()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed = std::hash<atermpp::aterm>()(get<2>(x).summation_variables()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed = std::hash<atermpp::aterm>()(get<2>(x).condition()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

}

namespace mcrl2
{
namespace data
{

core::identifier_string iff_name()
{
  static core::identifier_string iff_name = core::identifier_string("<=>");
  return iff_name;
}

function_symbol iff()
{
  static function_symbol iff(iff_name(), make_function_sort(sort_bool::bool_(), sort_bool::bool_(), sort_bool::bool_()));
  return iff;
}

inline application iff(const data_expression& d1, const data_expression& d2)
{
  return iff()(d1, d2);
}

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
  std::list<data_expression>      partition;
  bool                           m_contains_reals;
  simplifier*                    simpl;
  smt::smt_solver                m_smt_solver;

  typedef std::unordered_set< std::tuple< data_expression, data_expression, lps::action_summand > > refinement_cache_t;
  refinement_cache_t refinement_cache;
  typedef std::map< std::pair< data_expression, data_expression >, bool > reachability_cache_t;
  reachability_cache_t reachability_cache;
  typedef std::unordered_map< std::tuple< data_expression, data_expression, lps::action_summand >, bool> transition_cache_t;
  transition_cache_t transition_cache;

  std::map< lps::action_summand, variable_list > m_primed_summation_variables_map;
  std::map< lps::action_summand, data_expression_list > m_updates_map;
  std::map< lps::action_summand, substitution_t > m_primed_substitution_map;
  variable_list m_primed_process_parameters;

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
    const std::set< data_expression >& linear_inequalities, data_expression& enumeration_condition, data_expression& lli)
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
  void enumerate(const data_expression& parent_block, const variable_list& vars, const lambda& split_block,
    const data_expression& lli, std::set< data_expression >& new_part)
  {

    typedef enumerator_algorithm_with_iterator<rewriter, enumerator_list_element_with_substitution<>, enumerate_filter_print> enumerator_type;
    enumerator_identifier_generator id_generator;
    enumerator_type enumerator(rewr, m_spec.data(), rewr, id_generator, (std::numeric_limits<std::size_t>::max)(), true, enumerate_filter_print(lli, rewr));

    data::mutable_indexed_substitution<> sigma;
    data::enumerator_queue<enumerator_list_element_with_substitution<> >
         enumerator_deque(enumerator_list_element_with_substitution<>(vars, parent_block));
    for (typename enumerator_type::iterator i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
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

  data_expression make_abstraction(const binder_type& b, const variable_list& v, const data_expression& body)
  {
    return v.empty() ? body : abstraction(b, v, body);
  }

  data_expression make_abstraction_reals_inside(const binder_type& b, const variable_list& v, const data_expression& body)
  {
    variable_list non_real_var;
    variable_list real_var;
    for(const variable& var: v)
    {
      if(var.sort() == sort_real::real_())
      {
        real_var.push_front(var);
      }
      else
      {
        non_real_var.push_front(var);
      }
    }
    return make_abstraction(b, non_real_var, make_abstraction(b, real_var, body));
  }

  /**
   * \brief Split block phi_k on phi_l wrt summand as
   */
  bool split_block(const data_expression& phi_k, const data_expression& phi_l, const lps::action_summand& as)
  {
    if(refinement_cache.find(std::make_tuple(phi_k, phi_l, as)) != refinement_cache.end())
    {
      // This split has already been tried before.
      return false;
    }
    // Search the cache for information on this transition
    if(!transition_exists(phi_k, phi_l, as))
    {
      // Cache entry found
      return false;
    }

    substitution_t sub_primed = m_primed_substitution_map[as];
    variable_list primed_summation_variables = m_primed_summation_variables_map[as];
    data_expression_list updates = m_updates_map[as];

    data_expression arguments_equal = sort_bool::true_();
    // Check whether there is at least one action, since we cannot request
    // arguments from an empty list of actions. The empty multi action represents
    // tau.
    if(!as.multi_action().actions().empty())
    {
      for(const data_expression& expr: as.multi_action().actions().front().arguments())
      {
        arguments_equal = lazy::and_(arguments_equal, equal_to(expr, rewr(expr, sub_primed)));
      }
    }
    arguments_equal = rewr(arguments_equal);
    // mCRL2log(log::verbose) << "Constructing block expression ..." << std::endl;
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
            make_abstraction(forall_binder(), primed_summation_variables,
              lazy::implies(
                rewr(prime_condition),
                make_abstraction(exists_binder(), as.summation_variables(),
                  rewr(lazy::and_(
                    lazy::and_(
                      as.condition(),
                      rewr(application(phi_l, updates))),
                    arguments_equal
                  ))
                )
              )
            ),
            make_abstraction(forall_binder(), as.summation_variables(),
              lazy::implies(
                rewr(lazy::and_(
                  as.condition(),
                  rewr(application(phi_l, updates))
                )),
                make_abstraction(exists_binder(), primed_summation_variables,
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
      for(const data_expression& d: new_blocks)
      {
        partition.push_front(d);
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
  void update_caches(const data_expression& phi_k, const std::set<data_expression>& new_blocks)
  {
    for(const data_expression& phi_l1: partition)
    {
      for(const lps::action_summand& as1: m_spec.process().action_summands())
      {
        transition_cache_t::iterator find_result =
          transition_cache.find(std::make_tuple(phi_k, phi_l1, as1));
        if(find_result != transition_cache.end() && !find_result->second)
        {
          // phi_k has no transition to phi_l1 based on as1.
          // We add the new blocks to the transition cache
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
          // phi_l1 has no transitions to phi_k based on as1.
          // We add the new blocks to the transition cache
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
    for(const data_expression& phi_k: partition)
    {
      int j = 0;
      for(const data_expression& phi_l: partition)
      {
        int k = 0;
        for(const lps::action_summand& as: m_spec.process().action_summands())
        {
          // mCRL2log(log::verbose) << "Trying to split " << i << " on " << j << " wrt summand " << k << std::endl;
          if(split_block(phi_k, phi_l, as))
          {
            mCRL2log(log::verbose) << "Split " << rewr(phi_k) << " wrt summand\n" << as << "\non block " << rewr(phi_l) << std::endl;
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
      if(rewr(application(block, m_spec.initial_process().expressions())) == sort_bool::true_())
      {
        // mCRL2log(log::verbose) << "Found initial block " << block << std::endl;
        return block;
      }
    }
    throw mcrl2::runtime_error("Initial block not found.");
  }

  bool is_satisfiable(const variable_list& vars, const data_expression& expr)
  {
    try
    {
      switch(m_smt_solver.solve(vars, expr))
      {
        case smt::answer::SAT: return true;
        case smt::answer::UNSAT: return false;
        // if UNKNOWN, we continue and try the rewriter
        default: ;
      }
    }
    catch(const smt::translation_error& e)
    {
      mCRL2log(log::warning) << e.what() << std::endl;
      throw mcrl2::runtime_error("Solver failed!");
    }

    // The SMT solver failed, so we fallback to the rewriter
    data_expression is_sat = make_abstraction_reals_inside(exists_binder(), vars, expr);
    is_sat = rewr(one_point_rule_rewrite(quantifiers_inside_rewrite(is_sat)));
    if(m_contains_reals)
    {
      is_sat = rewr(replace_data_expressions(is_sat, fourier_motzkin_sigma(rewr), true));
    }
    if(is_sat != sort_bool::true_() && is_sat != sort_bool::false_())
    {
      throw mcrl2::runtime_error("Failed to establish whether " + data::pp(expr) + " is satisfiable");
    }
    return is_sat == sort_bool::true_();
  }

  /**
   * \brief Checks whether a transition between src and dest exists based on as
   */
  bool transition_exists(const data_expression& src, const data_expression& dest, const lps::action_summand& as)
  {
    // Search the cache for information on this transition
    transition_cache_t::iterator find_result = transition_cache.find(std::make_tuple(src, dest, as));
    if(find_result != transition_cache.end())
    {
      // Cache entry found
      return find_result->second;
    }

    // This expression expresses whether there is a transition between src and dest based on this summand
    bool result =
      is_satisfiable(m_spec.process().process_parameters() + as.summation_variables(),
        rewr(sort_bool::and_(
          application(src, m_spec.process().process_parameters()),
          sort_bool::and_(
            as.condition(),
            rewr(application(dest, m_updates_map[as]))
          )
        ))
      );
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


    mCRL2log(log::verbose) << RED(THIN) << "Unreachable blocks:" << NORMAL << std::endl;
    int i = 0;
    for(const data_expression& block: unreachable)
    {
      mCRL2log(log::verbose) << "  block " << i << "  " << pp(rewr(block)) << std::endl;
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
    utilities::indexed_set<data_expression> indexed_partition;
    for(const data_expression& block: blocks)
    {
      indexed_partition.insert(block);
      result.add_state(lts::state_label_lts(lps::state{rewr(block)}));
    }
    utilities::indexed_set<process::action_list> indexed_actions;
    // tau is always action 0
    indexed_actions.insert(lts::action_label_lts::tau_action().actions());
    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      auto action_index = indexed_actions.insert(as.multi_action().actions());
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
            lts::transition trans(indexed_partition.index(src), indexed_actions.index(as.multi_action().actions()), indexed_partition.index(dest));
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
      result.set_initial_state(indexed_partition.index(initial_block));
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
      mCRL2log(log::verbose) << YELLOW(THIN) << "  block " << i << "  " << NORMAL << pp(rewr(block)) << std::endl;
      // detail::BDD2Dot bddwriter;
      // bddwriter.output_bdd(atermpp::down_cast<abstraction>(block).body(), ("block" + std::to_string(i) + ".dot").c_str());
      i++;
    }
  }

  data_specification add_iff_rules(data_specification spec)
  {
    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());

    // Rules for bidirectional implication (iff)
    spec.add_equation(data_equation(variable_list({vb1}), iff(sort_bool::true_(), vb1), vb1));
    spec.add_equation(data_equation(variable_list({vb1}), iff(vb1, sort_bool::true_()), vb1));
    spec.add_equation(data_equation(variable_list({vb1}), iff(sort_bool::false_(), vb1), sort_bool::not_(vb1)));
    spec.add_equation(data_equation(variable_list({vb1}), iff(vb1, sort_bool::false_()), sort_bool::not_(vb1)));
    return spec;
  }

public:
  symbolic_bisim_algorithm(Specification& spec, const simplifier_mode& simplify_strat, const rewrite_strategy& st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec)
    , strat(st)
    , rewr(add_iff_rules(merge_data_specifications(m_spec.data(),simplifier::norm_rules_spec())), st)
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover)
#else
    , proving_rewr(spec.data(), jitty_prover)
#endif
    , m_contains_reals(std::find_if(m_spec.process().process_parameters().begin(), m_spec.process().process_parameters().end(),[](const variable& v){ return v.sort() == sort_real::real_(); }) != m_spec.process().process_parameters().end())
    , simpl(get_simplifier_instance(simplify_strat, rewr, proving_rewr, m_spec.process().process_parameters(), m_spec.data()))
    , m_smt_solver(spec.data())
  {}

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;

    process_parameters = m_spec.process().process_parameters();
    data_expression initial_block = lambda(process_parameters, sort_bool::true_());
    partition.push_front(initial_block);
    build_summand_maps();

    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = std::chrono::high_resolution_clock::now();
    mCRL2log(log::verbose) << "Initial partition:" << std::endl;
    print_partition(partition);
    int num_iterations = 0;
    while(refine())
    {
      mCRL2log(log::verbose) << GREEN(THIN) << "Partition:" << NORMAL << std::endl;
      print_partition(partition);
      find_reachable_blocks();
      num_iterations++;
      mCRL2log(log::status) << "End of iteration " << num_iterations << ", current number of blocks is " << partition.size() << "\n";
    }
    mCRL2log(log::verbose) << "Final partition:" << std::endl;
    print_partition(partition);
    std::set< data_expression > final_partition;
    std::for_each(partition.begin(), partition.end(), [&](const data_expression& block){ final_partition.insert(rewr(block)); });
    mCRL2log(log::info) << "Partition refinement completed in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << " seconds" << std::endl;

    save_lts();
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
