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
#include "mcrl2/atermpp/list.h"
#include "mcrl2/atermpp/shared_subset.h"

namespace mcrl2
{

namespace lps
{

class next_state_generator
{
  protected:
    // A rewriter_expression_t is a data expression in rewriter-internal format.
    typedef atermpp::aterm_appl rewriter_expression_t;
    typedef atermpp::term_list<rewriter_expression_t> valuation_t;
    typedef atermpp::list<valuation_t> summand_enumeration_t;
    typedef atermpp::term_appl<rewriter_expression_t> condition_arguments_t;

    typedef data::detail::legacy_rewriter rewriter_t;
    typedef data::classic_enumerator<rewriter_t> enumerator_t;

  public:
    typedef data::rewriter::internal_substitution_type substitution_t;
    typedef rewriter_expression_t internal_state_argument_t;
    typedef atermpp::term_appl<internal_state_argument_t> internal_state_t;

  protected:
    struct action_internal_t
    {
      lps::action_label label;
      atermpp::vector<rewriter_expression_t> arguments;
    };
    friend struct atermpp::aterm_traits<action_internal_t>;

    struct summand_t
    {
      action_summand *summand;
      data::variable_list variables;
      rewriter_expression_t condition;
      atermpp::aterm_appl result_state;
      atermpp::vector<action_internal_t> action_label;

      std::vector<size_t> condition_parameters;
      atermpp::function_symbol condition_arguments_function;
      atermpp::aterm_appl condition_arguments_function_dummy;
      atermpp::map<condition_arguments_t, summand_enumeration_t> enumeration_cache;
    };
    friend struct atermpp::aterm_traits<summand_t>;

    struct pruning_tree_node_t
    {
      atermpp::shared_subset<summand_t> summand_subset;
      atermpp::map<internal_state_argument_t, pruning_tree_node_t> children;
    };
    friend struct atermpp::aterm_traits<pruning_tree_node_t>;

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
        summand_subset_t(next_state_generator *generator, const action_summand_vector &summands, bool use_summand_pruning);

      private:
        next_state_generator *m_generator;
        bool m_use_summand_pruning;

        std::vector<size_t> m_summands;

        pruning_tree_node_t m_pruning_tree;
        std::vector<size_t> m_pruning_parameters;
        substitution_t m_pruning_substitution;
        rewriter_expression_t m_false;

        static bool summand_set_contains(const atermpp::set<action_summand> &summand_set, const summand_t &summand);
        void build_pruning_parameters(const action_summand_vector &summands);
        bool is_not_false(summand_t &summand);
        atermpp::shared_subset<summand_t>::iterator begin(internal_state_t state);
    };

    class transition_t
    {
      friend struct atermpp::aterm_traits<transition_t>;
      friend class next_state_generator::iterator;
      private:
        next_state_generator *m_generator;
        internal_state_t m_state;
        lps::multi_action m_action;
        size_t m_summand_index;

      public:
        lps::state state() const { return m_generator->get_state(m_state); }
        internal_state_t &internal_state() { return m_state; }
        const internal_state_t &internal_state() const { return m_state; }
        lps::multi_action &action() { return m_action; }
        const lps::multi_action &action() const { return m_action; }
        size_t summand_index() const { return m_summand_index; }
    };

    class iterator: public boost::iterator_facade<iterator, const transition_t, boost::forward_traversal_tag>
    {
      protected:
        transition_t m_transition;
        next_state_generator *m_generator;
        internal_state_t m_state;
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
        enumerator_t::iterator_internal m_enumeration_iterator;
        bool m_caching;
        condition_arguments_t m_enumeration_cache_key;
        summand_enumeration_t m_enumeration_log;

      public:
        iterator()
          : m_generator(0)
        {
        }

        iterator(next_state_generator *generator, internal_state_t state, substitution_t *substitution, summand_subset_t &summand_subset);

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

  protected:
    specification m_specification;
    rewriter_t m_rewriter;
    enumerator_t m_enumerator;

    bool m_use_enumeration_caching;

    data::variable_vector m_process_parameters;
    atermpp::function_symbol m_state_function;
    atermpp::vector<summand_t> m_summands;
    internal_state_t m_initial_state;

    summand_subset_t m_all_summands;

  public:
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
      return iterator(this, state, substitution, m_all_summands);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors using summands from \a summand_subset are generated.
    iterator begin(state state, substitution_t *substitution, summand_subset_t &summand_subset)
    {
      return begin(get_internal_state(state), substitution, summand_subset);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(internal_state_t state, substitution_t *substitution, summand_subset_t &summand_subset)
    {
      return iterator(this, state, substitution, summand_subset);
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
      return get_state(internal_initial_state());
    }

    /// \brief Gets the initial state in internal format.
    internal_state_t internal_initial_state() const
    {
      return m_initial_state;
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

    /// \brief Converts a state arguments to internal state arguments.
    internal_state_argument_t get_internal_state_argument(data::data_expression argument) const
    {
      return m_rewriter.convert_to(argument);
    }

    /// \brief Converts a internal state arguments to state arguments.
    data::data_expression get_state_argument(internal_state_argument_t internal_argument) const
    {
      return m_rewriter.convert_from(internal_argument);
    }

    /// \brief Constructs internal states out of internal state arguments.
    internal_state_t get_internal_state(const internal_state_argument_t *internal_arguments) const
    {
      return internal_state_t(m_state_function, internal_arguments, internal_arguments + m_state_function.arity());
    }

    /// \brief Converts states to internal states.
    internal_state_t get_internal_state(state s) const;

    /// \brief Converts internal states to states.
    state get_state(internal_state_t internal_state) const;

    /// \brief Returns the function symbol used to construct internal states.
    atermpp::function_symbol internal_state_function() const
    {
      return m_state_function;
    }

    /// \brief Returns a reference to the summand subset containing all summands.
    summand_subset_t &full_subset()
    {
      return m_all_summands;
    }

  private:
    void declare_constructors();
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
