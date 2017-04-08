// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file next_state_generator.h

#ifndef MCRL2_LPS_NEXT_STATE_GENERATOR_H
#define MCRL2_LPS_NEXT_STATE_GENERATOR_H

#include <iterator>
#include <string>
#include <vector>
#include <forward_list>
#include <boost/iterator/iterator_facade.hpp>

#include "mcrl2/atermpp/detail/shared_subset.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/state_probability_pair.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/probabilistic_data_expression.h"

namespace mcrl2
{

namespace lps
{

class next_state_generator
{
  public:
    typedef std::list<data::data_expression_list> summand_enumeration_t;
    typedef atermpp::term_appl<data::data_expression> condition_arguments_t;

    typedef data::rewriter rewriter_t;
    typedef data::enumerator_algorithm_with_iterator<> enumerator_t;
    typedef enumerator_t::iterator enumerator_iterator_t;
    typedef std::deque<data::enumerator_list_element_with_substitution<> > enumerator_queue_t;

    typedef data::rewriter::substitution_type substitution_t;

  protected:
    struct action_internal_t
    {
      process::action_label label;
      data::data_expression_vector arguments;
    };

    struct summand_t
    {
      stochastic_action_summand *summand;
      data::variable_list variables;
      data::data_expression condition;
      stochastic_distribution distribution;
      data::data_expression_vector result_state;
      std::vector<action_internal_t> action_label;
      data::data_expression time_tag;

      std::vector<size_t> condition_parameters;
      atermpp::function_symbol condition_arguments_function;
      atermpp::aterm_appl condition_arguments_function_dummy;
      std::map<condition_arguments_t, summand_enumeration_t> enumeration_cache;
    };

    struct pruning_tree_node_t
    {
      atermpp::detail::shared_subset<summand_t> summand_subset;
      std::map<data::data_expression, pruning_tree_node_t> children;
    };

  public:
    class iterator;

    class summand_subset_t
    {
      friend class next_state_generator;
      friend class next_state_generator::iterator;

      public:
        /// \brief Trivial constructor. Constructs an invalid command subset.
        summand_subset_t() {}

        /// \brief Constructs the full summand subset for the given generator.
        summand_subset_t(next_state_generator *generator, bool use_summand_pruning);

        /// \brief Constructs the summand subset containing the given commands.
        summand_subset_t(next_state_generator *generator, const stochastic_action_summand_vector& summands, bool use_summand_pruning);

      private:
        next_state_generator *m_generator;
        bool m_use_summand_pruning;

        std::vector<size_t> m_summands;

        pruning_tree_node_t m_pruning_tree;
        std::vector<size_t> m_pruning_parameters;
        substitution_t m_pruning_substitution;

        static bool summand_set_contains(const std::set<stochastic_action_summand>& summand_set, const summand_t& summand);
        void build_pruning_parameters(const stochastic_action_summand_vector& summands);
        bool is_not_false(const summand_t& summand);
        atermpp::detail::shared_subset<summand_t>::iterator begin(const lps::state& state);
    };

    typedef mcrl2::lps::state_probability_pair<lps::state, lps::probabilistic_data_expression> state_probability_pair;

    class transition_t
    {
      public:
        typedef std::forward_list<state_probability_pair> state_probability_list;

      protected:
        lps::multi_action m_action;
        lps::state m_target_state;
        size_t m_summand_index;
        // The following list contains all but one target states with their probabity.
        // m_target_state is the other state, with the residual probability, such
        // that all probabilities add up to 1.
        state_probability_list m_other_target_states;

      public:
        const lps::multi_action& action() const 
        { 
          return m_action; 
        }

        void set_action(const lps::multi_action& action)
        {
          m_action=action;
        }

        const lps::state& target_state() const { return m_target_state; }
        void set_target_state(const lps::state& target_state)
        {
          m_target_state=target_state;
        }

        size_t summand_index() const { return m_summand_index; }
        void set_summand_index(const size_t summand_index)
        {
          m_summand_index=summand_index;
        }

        const state_probability_list& other_target_states() const { return m_other_target_states; }
        void set_other_target_states(const state_probability_list& other_target_states)
        {
          m_other_target_states=other_target_states;
        }
    };

    class iterator: public boost::iterator_facade<iterator, const transition_t, boost::forward_traversal_tag>
    {
      protected:
        transition_t m_transition;
        next_state_generator* m_generator;
        lps::state m_state;
        substitution_t* m_substitution;

        bool m_single_summand;
        size_t m_single_summand_index;
        bool m_use_summand_pruning;
        std::vector<size_t>::iterator m_summand_iterator;
        std::vector<size_t>::iterator m_summand_iterator_end;
        atermpp::detail::shared_subset<summand_t>::iterator m_summand_subset_iterator;
        summand_t *m_summand;

        bool m_cached;
        summand_enumeration_t::iterator m_enumeration_cache_iterator;
        summand_enumeration_t::iterator m_enumeration_cache_end;
        enumerator_iterator_t m_enumeration_iterator;
        bool m_caching;
        condition_arguments_t m_enumeration_cache_key;
        summand_enumeration_t m_enumeration_log;

        enumerator_queue_t* m_enumeration_queue;

        /// \brief Enumerate <variables, phi> with substitution sigma.
        void enumerate(const data::variable_list& variables, const data::data_expression& phi, data::mutable_indexed_substitution<>& sigma)
        {
          m_enumeration_queue->clear();
          m_enumeration_queue->push_back(data::enumerator_list_element_with_substitution<>(variables, phi));
          try
          {
            m_enumeration_iterator = m_generator->m_enumerator.begin(sigma, *m_enumeration_queue);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nProblem occurred when enumerating variables " + data::pp(variables) + " in " + data::pp(phi));
          }
        }


      public:
        iterator()
          : m_generator(nullptr)
        {
        }

        iterator(next_state_generator* generator, const lps::state& state, substitution_t* substitution, summand_subset_t& summand_subset, enumerator_queue_t* enumeration_queue);

        iterator(next_state_generator* generator, const lps::state& state, substitution_t* substitution, size_t summand_index, enumerator_queue_t* enumeration_queue);

        operator bool() const
        {
          return m_generator != nullptr;
        }

      private:
        friend class boost::iterator_core_access;

        bool equal(const iterator& other) const
        {
          return (!(bool)*this && !(bool)other) || (this == &other);
        }

        const transition_t& dereference() const
        {
          return m_transition;
        }

        void increment();
        bool summand_finished();
    };

  protected:
    stochastic_specification m_specification;
    rewriter_t m_rewriter;
    substitution_t m_substitution;
    enumerator_t m_enumerator;

    bool m_use_enumeration_caching;

    data::variable_vector m_process_parameters;
    std::vector<summand_t> m_summands;
    transition_t::state_probability_list m_initial_states;

    summand_subset_t m_all_summands;

  public:
    /// \brief Constructor
    /// \param spec The process specification
    /// \param rewriter The rewriter used
    /// \param use_enumeration_caching Cache intermediate enumeration results
    /// \param use_summand_pruning Preprocess summands using pruning strategy.
    next_state_generator(const stochastic_specification& spec, 
                         const data::rewriter& rewriter, 
                         bool use_enumeration_caching = false, 
                         bool use_summand_pruning = false);

    ~next_state_generator();

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const state& state, enumerator_queue_t* enumeration_queue)
    {
      return iterator(this, state, &m_substitution, m_all_summands, enumeration_queue);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const state& state, summand_subset_t& summand_subset, enumerator_queue_t* enumeration_queue)
    {
      return iterator(this, state, &m_substitution, summand_subset, enumeration_queue);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are generated.
    iterator begin(const state& state, size_t summand_index, enumerator_queue_t* enumeration_queue)
    {
      return iterator(this, state, &m_substitution, summand_index, enumeration_queue);
    }

    /// \brief Returns an iterator pointing to the end of a next state list.
    iterator end()
    {
      return iterator();
    }

    /// \brief Gets the initial state.
    const transition_t::state_probability_list& initial_states() const
    {
      return m_initial_states;
    }

    /// \brief Returns the rewriter associated with this generator.
    rewriter_t& get_rewriter()
    {
      return m_rewriter;
    }

    /// \brief Returns a reference to the summand subset containing all summands.
    summand_subset_t& full_subset()
    {
      return m_all_summands;
    }

    // Calculate the set of states with associated probabilities from a symbolic state
    // and an associated stochastic distribution for the free variables in that state.
    // The result is a list of closed states with associated probabilities.
    const transition_t::state_probability_list calculate_distribution(
                         const stochastic_distribution& dist,
                         const data::data_expression_vector& state_args,
                         substitution_t& sigma);
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_NEXT_STATE_GENERATOR_H
