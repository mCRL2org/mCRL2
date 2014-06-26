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

// #define MCRL2_USE_NEW_ENUMERATOR

#include <iterator>
#include <string>
#include <vector>
#include <boost/iterator/iterator_facade.hpp>

#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator.h"
#ifdef MCRL2_NEXT_STATE_LOG_EQUALITIES
#include "mcrl2/data/find_equalities.h"
#endif
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/atermpp/shared_subset.h"

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
#ifdef MCRL2_USE_NEW_ENUMERATOR
    typedef data::enumerator_algorithm_with_iterator<rewriter_t, data::mutable_indexed_substitution<>, data::enumerator_list_element_with_substitution<data::data_expression>, data::is_not_false> enumerator_t;
    typedef enumerator_t::iterator enumerator_iterator_t;
#else
    typedef data::classic_enumerator<rewriter_t> enumerator_t;
    typedef enumerator_t::iterator enumerator_iterator_t;
#endif

    typedef data::rewriter::substitution_type substitution_t;

  protected:
    struct action_internal_t
    {
      process::action_label label;
      data::data_expression_vector arguments;
    };

    struct summand_t
    {
      action_summand *summand;
      data::variable_list variables;
      data::data_expression condition;
      data::data_expression_vector result_state;
      std::vector<action_internal_t> action_label;

      std::vector<size_t> condition_parameters;
      atermpp::function_symbol condition_arguments_function;
      atermpp::aterm_appl condition_arguments_function_dummy;
      std::map<condition_arguments_t, summand_enumeration_t> enumeration_cache;
    };

    struct pruning_tree_node_t
    {
      atermpp::shared_subset<summand_t> summand_subset;
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
        summand_subset_t(next_state_generator *generator, const action_summand_vector& summands, bool use_summand_pruning);

      private:
        next_state_generator *m_generator;
        bool m_use_summand_pruning;

        std::vector<size_t> m_summands;

        pruning_tree_node_t m_pruning_tree;
        std::vector<size_t> m_pruning_parameters;
        substitution_t m_pruning_substitution;

        static bool summand_set_contains(const std::set<action_summand>& summand_set, const summand_t& summand);
        void build_pruning_parameters(const action_summand_vector& summands);
        bool is_not_false(const summand_t& summand);
        atermpp::shared_subset<summand_t>::iterator begin(const data::data_expression_vector& state);
    };

    class transition_t
    {
      friend class next_state_generator::iterator;
      private:
        next_state_generator *m_generator;
        data::data_expression_vector m_state;
        lps::multi_action m_action;
        size_t m_summand_index;

      public:
        lps::state state() const
        {
          return m_generator->get_state(m_state);
        }

        data::data_expression_vector& internal_state() { return m_state; }
        const data::data_expression_vector& internal_state() const { return m_state; }
        lps::multi_action& action() { return m_action; }
        const lps::multi_action& action() const { return m_action; }
        size_t summand_index() const { return m_summand_index; }
    };

    class iterator: public boost::iterator_facade<iterator, const transition_t, boost::forward_traversal_tag>
    {
      protected:
        transition_t m_transition;
        next_state_generator *m_generator;
        data::data_expression_vector m_state;
        substitution_t *m_substitution;

        bool m_single_summand;
        size_t m_single_summand_index;
        bool m_use_summand_pruning;
        std::vector<size_t>::iterator m_summand_iterator;
        std::vector<size_t>::iterator m_summand_iterator_end;
        atermpp::shared_subset<summand_t>::iterator m_summand_subset_iterator;
        summand_t *m_summand;

        bool m_cached;
        summand_enumeration_t::iterator m_enumeration_cache_iterator;
        summand_enumeration_t::iterator m_enumeration_cache_end;
        enumerator_iterator_t m_enumeration_iterator;
        bool m_caching;
        condition_arguments_t m_enumeration_cache_key;
        summand_enumeration_t m_enumeration_log;

        std::deque < data::enumerator_list_element_with_substitution< data::data_expression> > m_enumeration_queue;

        /// \brief Enumerate <variables, phi> with substitution sigma.
        void enumerate(const data::variable_list& variables, const data::data_expression& phi, data::mutable_indexed_substitution<>& sigma)
        {
#ifdef MCRL2_NEXT_STATE_LOG_EQUALITIES
          data::detail::find_equalities_traverser_inst f;
          f(phi);
          auto const& equalities = f.top().equalities;
          for (auto i = variables.begin(); i != variables.end(); ++i)
          {
            auto j = equalities.find(*i);
            if (j != equalities.end())
            {
              std::cout << "EQUALITY: " << j->first << " -> " << core::detail::print_set(j->second) << std::endl;
            }
          }
#endif
          m_enumeration_queue.clear();
          m_enumeration_queue.push_back(data::enumerator_list_element_with_substitution<data::data_expression>(variables, phi));
#ifdef MCRL2_USE_NEW_ENUMERATOR
          m_enumeration_iterator = m_generator->m_enumerator.begin(sigma, m_enumeration_queue, data::is_not_false());
#else
          m_enumeration_iterator = m_generator->m_enumerator.begin(sigma, m_enumeration_queue);
#endif
        }


      public:
        iterator()
          : m_generator(0)
#ifdef MCRL2_USE_NEW_ENUMERATOR
            , m_enumeration_iterator(data::is_not_false())
#endif
        {
        }

        iterator(next_state_generator *generator, const data::data_expression_vector& state, substitution_t *substitution, summand_subset_t& summand_subset);

        iterator(next_state_generator *generator, const data::data_expression_vector& state, substitution_t *substitution, size_t summand_index);

        operator bool() const
        {
          return m_generator != 0;
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
    specification m_specification;
    rewriter_t m_rewriter;
    substitution_t m_substitution;
    enumerator_t m_enumerator;

    bool m_use_enumeration_caching;

    data::variable_vector m_process_parameters;
    std::vector<summand_t> m_summands;
    data::data_expression_vector m_initial_state;

    summand_subset_t m_all_summands;

  public:
    /// \brief Constructor
    /// \param specification The process specification
    /// \param rewriter The rewriter used
    /// \param use_enumeration_caching Cache intermediate enumeration results
    /// \param use_summand_pruning Preprocess summands using pruning strategy.
    next_state_generator(const specification& specification, const data::rewriter& rewriter, bool use_enumeration_caching = false, bool use_summand_pruning = false);

    ~next_state_generator();

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const state& state)
    {
      return begin(get_internal_state(state));
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const data::data_expression_vector& state)
    {
      return iterator(this, state, &m_substitution, m_all_summands);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors using summands from \a summand_subset are generated.
    iterator begin(const state& state, summand_subset_t& summand_subset)
    {
      return begin(get_internal_state(state), summand_subset);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const data::data_expression_vector& state, summand_subset_t& summand_subset)
    {
      return iterator(this, state, &m_substitution, summand_subset);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are generated.
    iterator begin(const state& state, size_t summand_index)
    {
      return begin(get_internal_state(state), summand_index);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are generated.
    iterator begin(const data::data_expression_vector& state, size_t summand_index)
    {
      return iterator(this, state, &m_substitution, summand_index);
    }

    /// \brief Returns an iterator pointing to the end of a next state list.
    iterator end()
    {
      return iterator();
    }

    /// \brief Gets the initial state.
    state initial_state() const
    {
      return get_state(internal_initial_state());
    }

    /// \brief Gets the initial state in internal format.
    data::data_expression_vector internal_initial_state() const
    {
      return m_initial_state;
    }

    /// \brief Returns the currently loaded specification.
    const lps::specification& get_specification() const
    {
      return m_specification;
    }

    /// \brief Returns the rewriter associated with this generator.
    rewriter_t& get_rewriter()
    {
      return m_rewriter;
    }

    /// \brief Converts states to internal states.
    data::data_expression_vector get_internal_state(const state& s) const;

    /// \brief Converts internal states to states.
    state get_state(const data::data_expression_vector& internal_state) const;

    /// \brief Returns a reference to the summand subset containing all summands.
    summand_subset_t& full_subset()
    {
      return m_all_summands;
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_NEXT_STATE_GENERATOR_H
