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
#include <boost/iterator/iterator_facade.hpp>

#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
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
    typedef data::rewriter::internal_substitution_type substitution_t;

  protected:
    typedef atermpp::aterm_appl rewriter_term_t;
    typedef atermpp::term_list<rewriter_term_t> valuation_t;
    typedef std::list<valuation_t> summand_enumeration_t;
    typedef atermpp::term_appl<rewriter_term_t> condition_arguments_t;

    typedef data::detail::legacy_rewriter rewriter_t;
    typedef data::classic_enumerator<rewriter_t> enumerator_t;

    struct action_internal_t
    {
      lps::action_label label;
      std::vector<rewriter_term_t> arguments;
    };
    friend struct atermpp::aterm_traits<action_internal_t>;

    struct summand_t
    {
      data::variable_list variables;
      rewriter_term_t condition;
      atermpp::aterm_appl result_state;
      std::vector<action_internal_t> action_label;

      std::vector<size_t> condition_parameters;
      atermpp::function_symbol condition_arguments_function;
      atermpp::aterm_appl condition_arguments_function_dummy;
      std::map<condition_arguments_t, summand_enumeration_t> enumeration_cache;
    };
    friend struct atermpp::aterm_traits<summand_t>;

    struct pruning_tree_node_t
    {
      atermpp::shared_subset<summand_t> summand_subset;
      std::map<rewriter_term_t, pruning_tree_node_t> children;
    };
    friend struct atermpp::aterm_traits<pruning_tree_node_t>;

    specification m_specification;
    rewriter_t m_rewriter;
    enumerator_t m_enumerator;

    bool m_use_enumeration_caching;
    bool m_use_summand_pruning;

    data::variable_vector m_process_parameters;
    atermpp::function_symbol m_state_function;
    std::vector<summand_t> m_summands;

    pruning_tree_node_t m_pruning_tree;
    std::vector<size_t> m_pruning_tree_parameters;
    substitution_t m_pruning_tree_substitution;
    rewriter_term_t m_false;

  public:
    typedef atermpp::term_appl<rewriter_term_t> internal_state_t;

    class iterator;
    class transition_t
    {
      friend struct atermpp::aterm_traits<transition_t>;
      friend class next_state_generator::iterator;
      private:
        next_state_generator *m_generator;
        internal_state_t m_state;
        lps::multi_action m_action;

      public:
        lps::state state() const { return m_generator->get_state(m_state); }
        internal_state_t &internal_state() { return m_state; }
        const internal_state_t &internal_state() const { return m_state; }
        lps::multi_action &action() { return m_action; }
        const lps::multi_action &action() const { return m_action; }
    };

    class iterator: public boost::iterator_facade<iterator, const transition_t, boost::forward_traversal_tag>
    {
      protected:
        transition_t m_transition;
        next_state_generator *m_generator;
        internal_state_t m_state;
        substitution_t *m_substitution;

        bool m_use_summand_pruning;
        std::vector<summand_t>::iterator m_summand_iterator;
        std::vector<summand_t>::iterator m_summand_iterator_end;
        atermpp::shared_subset<summand_t>::iterator m_summand_subset_iterator;
        summand_t *m_summand;

        bool m_cached;
        summand_enumeration_t::iterator m_enumeration_cache_iterator;
        summand_enumeration_t::iterator m_enumeration_cache_end;
        enumerator_t::iterator_internal m_enumeration_iterator;
        bool m_caching;
        condition_arguments_t m_enumeration_cache_key;
        summand_enumeration_t m_enumeration_log;

      public:
        iterator()
          : m_generator(0)
        {
        }

        iterator(next_state_generator *generator, internal_state_t state, substitution_t *substitution);

        iterator(next_state_generator *generator, internal_state_t state, substitution_t *substitution, size_t summand_index);

        operator bool() const
        {
          return m_generator != 0;
        }

      private:
        friend class boost::iterator_core_access;

        bool equal(iterator const& other) const
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

    /// \brief Constructor
    /// \param specification The process specification
    /// \param rewriter_strategy The rewriter strategy used
    /// \param extra_function_symbols Additional function symbols the rewriter should not remove. Used by lts/exploration.
    /// \param compute_action_labels If false, action labels for generated transitions are not computed.
    next_state_generator(const specification& specification, const data::rewriter &rewriter, bool use_enumeration_caching = false, bool use_summand_pruning = false);

    ~next_state_generator();

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(state state, substitution_t *substitution)
    {
      return begin(get_internal_state(state), substitution);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(internal_state_t state, substitution_t *substitution)
    {
      return iterator(this, state, substitution);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are generated.
    iterator begin(state state, substitution_t *substitution, size_t summand_index)
    {
      return begin(get_internal_state(state), substitution, summand_index);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are generated.
    iterator begin(internal_state_t state, substitution_t *substitution, size_t summand_index)
    {
      return iterator(this, state, substitution, summand_index);
    }

    /// \brief Returns an iterator pointing to the end of a next state list.
    iterator end()
    {
      return iterator();
    }

    /// \brief Gets the initial state.
    state initial_state() const
    {
      return m_specification.initial_process().state(m_specification.process().process_parameters());
    }

    /// \brief Gets the initial state in internal format.
    internal_state_t internal_initial_state() const
    {
      return get_internal_state(initial_state());
    }

    /// \brief Returns the currently loaded specification.
    const lps::specification& get_specification() const
    {
      return m_specification;
    }

    /// \brief Returns the rewriter associated with this generator.
    rewriter_t &get_rewriter()
    {
      return m_rewriter;
    }

    internal_state_t get_internal_state(state s) const;
    state get_state(internal_state_t internal_state) const;

  private:
    void declare_constructors();
    void build_pruning_tree_order();
    bool is_not_false(summand_t &summand);
    atermpp::shared_subset<summand_t>::iterator summand_subset(internal_state_t state);
};

} // namespace lps

} // namespace mcrl2

namespace atermpp
{
  template<> struct aterm_traits<mcrl2::lps::next_state_generator::action_internal_t>
  {
    static void protect(const mcrl2::lps::next_state_generator::action_internal_t &action) { action.label.protect(); }
    static void unprotect(const mcrl2::lps::next_state_generator::action_internal_t &action) { action.label.unprotect(); }
    static void mark(const mcrl2::lps::next_state_generator::action_internal_t &action) { action.label.mark(); }
  };
  template<> struct aterm_traits<mcrl2::lps::next_state_generator::summand_t>
  {
    static void protect(mcrl2::lps::next_state_generator::summand_t &summand) { summand.condition.protect(); summand.result_state.protect(); summand.condition_arguments_function_dummy.protect(); }
    static void unprotect(mcrl2::lps::next_state_generator::summand_t &summand) { summand.condition.unprotect(); summand.result_state.unprotect(); summand.condition_arguments_function_dummy.unprotect(); }
    static void mark(mcrl2::lps::next_state_generator::summand_t &summand) { summand.condition.mark(); summand.result_state.mark(); summand.condition_arguments_function_dummy.mark(); }
  };
  template<> struct aterm_traits<mcrl2::lps::next_state_generator::transition_t>
  {
    static void protect(const mcrl2::lps::next_state_generator::transition_t &transition) { transition.m_state.protect(); aterm_traits<mcrl2::lps::multi_action>::protect(transition.m_action); }
    static void unprotect(const mcrl2::lps::next_state_generator::transition_t &transition) { transition.m_state.unprotect(); aterm_traits<mcrl2::lps::multi_action>::unprotect(transition.m_action); }
    static void mark(const mcrl2::lps::next_state_generator::transition_t &transition) { transition.m_state.mark(); aterm_traits<mcrl2::lps::multi_action>::mark(transition.m_action); }
  };
  template<> struct aterm_traits<mcrl2::lps::next_state_generator::pruning_tree_node_t>
  {
    static void protect(const mcrl2::lps::next_state_generator::pruning_tree_node_t &node) { aterm_traits<shared_subset<mcrl2::lps::next_state_generator::summand_t> >::protect(node.summand_subset); }
    static void unprotect(const mcrl2::lps::next_state_generator::pruning_tree_node_t &node) { aterm_traits<shared_subset<mcrl2::lps::next_state_generator::summand_t> >::unprotect(node.summand_subset); }
    static void mark(const mcrl2::lps::next_state_generator::pruning_tree_node_t &node) { aterm_traits<shared_subset<mcrl2::lps::next_state_generator::summand_t> >::mark(node.summand_subset); }
  };
}

#endif // MCRL2_LPS_NEXT_STATE_GENERATOR_H
