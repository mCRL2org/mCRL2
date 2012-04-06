// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/next_state_generator_old.h
/// \brief Iterator interface for generating next states of a linear process specification.

#ifndef MCRL2_LPS_NEXT_STATE_GENERATOR_H
#define MCRL2_LPS_NEXT_STATE_GENERATOR_H

#include <sstream>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

namespace mcrl2
{

namespace lps
{

namespace old
{

/// \brief Class for generating a state space corresponding to a linear process specification.
class next_state_generator
{
  protected:
    specification m_specification;
    data::rewriter::strategy m_rewriter_strategy;
    mcrl2::data::detail::legacy_rewriter m_rewriter;
    NextState* m_NextState;
    NextStateGenerator* m_NextStateGenerator;
    NextStateGenerator* m_NextStateGeneratorPerSummand;

  public:
    /// \brief A type that represents a transition to a 'next' state.
    struct state_type
    {
      multi_action transition;
      ATerm state;

      state_type()
        : transition(),
          state(0)
      {}

      state_type(const state_type& other)
        : transition(other.transition),
          state(other.state)
      {}

      state_type(multi_action transition_, ATerm state_)
        : transition(transition_),
          state(state_)
      {}

      /// \brief Returns the i-th component of the state in the internal format.
      /// \deprecated
      atermpp::aterm operator[](std::size_t i) const
      {
        return atermpp::aterm_appl(reinterpret_cast<ATermAppl>(state))(i);
      }

      /// \brief Returns the label of the transition.
      multi_action label() const
      {
        return transition;
      }

      /// \brief Returns the number of components of the state.
      std::size_t size() const
      {
        return atermpp::term_appl<atermpp::aterm_appl>(reinterpret_cast<ATermAppl>(state)).size();
      }
    };

    /// \brief Iterator that generates all successor states of a given state.
    /// N.B. Concurrent traversals with this iterator are not allowed!
    class iterator: public boost::iterator_facade<
      iterator,                               // Derived
      const state_type,                       // Value
      boost::forward_traversal_tag            // CategoryOrTraversal
      >
    {
      protected:
        NextState* m_next_state;
        NextStateGenerator* m_generator;
        state_type m_state;

      public:
        /// \brief Default constructor.
        iterator()
        {}

        /// \brief Constructor.
        iterator(NextState* next, NextStateGenerator* generator)
          : m_next_state(next),
            m_generator(generator),
            m_state(multi_action(), m_next_state->getInitialState())
        { }

        /// \brief Constructor.
        iterator(NextState* next, NextStateGenerator* generator, const atermpp::aterm& state)
          : m_next_state(next),
            m_generator(generator)
        {
          m_state.state = state;
        }

        operator bool() const
        {
          return m_next_state != 0;
        }

      private:
        friend class boost::iterator_core_access;

        /// \brief Equality operator
        /// \param other An iterator
        /// \return True if the iterators are equal
        bool equal(iterator const& other) const
        {
          return m_next_state == other.m_next_state;
        }

        /// \brief Dereference operator
        /// \return The value that the iterator references
        const state_type& dereference() const
        {
          return m_state;
        }

        /// \brief Increments the iterator
        void increment()
        {
          if (!m_generator->next(m_state.transition, &m_state.state))
          {
            // empty m_next_state, to signal that there is no next state
            m_next_state = 0;
          }
        }
    };

    /// \brief Loads a specification from a file
    /// \param filename The name of a file containing an mCRL2 specification
    void load(const std::string& filename)
    {
      if (m_NextState)
      {
        delete m_NextState;
      }
      m_specification.load(filename);
      m_specification.process().deadlock_summands().clear();
      lps::detail::instantiate_global_variables(m_specification);
      m_rewriter = mcrl2::data::detail::legacy_rewriter(m_specification.data(), data::used_data_equation_selector(m_specification.data(), lps::find_function_symbols(m_specification), m_specification.global_variables()), m_rewriter_strategy);
      m_NextState = createNextState(m_specification, m_rewriter, false);
      m_NextStateGenerator = 0;
      m_NextStateGeneratorPerSummand = 0;
    }

    /// \brief Constructor
    /// \param filename The name of a file containing an mCRL2 specification
    /// \param rewriter_strategy The rewriter strategy used for generating next states
    next_state_generator(const std::string& filename, data::rewriter::strategy rewriter_strategy = data::jitty)
      : m_rewriter_strategy(rewriter_strategy),
        m_rewriter(data::data_specification()),
        m_NextState(0),
        m_NextStateGenerator(0),
        m_NextStateGeneratorPerSummand(0)
    {
      load(filename);
    }

    /// \brief Constructor
    next_state_generator(const specification& lps_spec, data::rewriter::strategy rewriter_strategy = data::jitty)
      : m_specification(lps_spec),
        m_rewriter_strategy(rewriter_strategy),
        m_rewriter(lps_spec.data(), data::used_data_equation_selector(lps_spec.data(), lps::find_function_symbols(lps_spec), lps_spec.global_variables()), rewriter_strategy),
        m_NextStateGenerator(0),
        m_NextStateGeneratorPerSummand(0)
    {
      m_specification.process().deadlock_summands().clear();
      lps::detail::instantiate_global_variables(m_specification);
      m_NextState = createNextState(m_specification, m_rewriter, false);
#ifdef MCRL2_REWRITE_RULE_SELECTION_DEBUG
std::clog << "--- rewrite rule selection specification ---\n";
std::clog << lps::pp(lps_spec) << std::endl;
std::clog << "--- rewrite rule selection function symbols ---\n";
std::clog << core::detail::print_set(lps::find_function_symbols(lps_spec), data::stream_printer()) << std::endl;
#endif
    }

    /// \brief Destructor
    ~next_state_generator()
    {
      if (m_NextState)
      {
        delete m_NextState;
      }
      if (m_NextStateGenerator)
      {
        delete m_NextStateGenerator;
      }
      if (m_NextStateGeneratorPerSummand)
      {
        delete m_NextStateGeneratorPerSummand;
      }
    }

    /// \brief Returns an iterator for generating the successors of the initial state.
    iterator begin()
    {
      m_NextStateGenerator = m_NextState->getNextStates(m_NextState->getInitialState(), m_NextStateGenerator);
      return iterator(m_NextState, m_NextStateGenerator);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    iterator begin(const atermpp::aterm& state)
    {
      m_NextStateGenerator = m_NextState->getNextStates(state, m_NextStateGenerator);
      return iterator(m_NextState, m_NextStateGenerator, state);
    }

    /// \brief Returns an iterator for generating the successors of the initial state.
    /// Only the successors with respect to the summand with the given index are
    /// generated.
    iterator begin(size_t summand_index)
    {
      m_NextStateGeneratorPerSummand = m_NextState->getNextStates(m_NextState->getInitialState(), summand_index, m_NextStateGeneratorPerSummand);
      return iterator(m_NextState, m_NextStateGeneratorPerSummand);
    }

    /// \brief Returns an iterator for generating the successors of the given state.
    /// Only the successors with respect to the summand with the given index are
    /// generated.
    iterator begin(const atermpp::aterm& state, size_t summand_index)
    {
      m_NextStateGeneratorPerSummand = m_NextState->getNextStates(state, summand_index, m_NextStateGeneratorPerSummand);
      return iterator(m_NextState, m_NextStateGeneratorPerSummand, state);
    }

    /// \brief Returns the initial state of the specification.
    atermpp::aterm initial_state() const
    {
      return  m_NextState->getInitialState();
    }

    /// \brief Returns the i-th component of the state s.
    data::data_expression state_component(const state_type& s, std::size_t i) const
    {
      ATerm t = s[i];
      return atermpp::aterm_appl(m_rewriter.convert_from(t));
    }

    /// \brief Converts a value in next state format to a data expression.
    data::data_expression aterm2expression(const atermpp::aterm& x) const
    {
      ATerm a = x;
      atermpp::aterm_appl result = m_rewriter.convert_from(a);
      return result;
    }

    /// \brief Converts a data expression to a value in next state format.
    atermpp::aterm expression2aterm(const data::data_expression& x) const
    {
      return m_rewriter.convert_to(x);
    }

    /// \brief Returns a string representation of the given state.
    std::string print_state(const state_type& state) const
    {
      std::string result("state(");
      for (unsigned int index = 0; index < state.size(); index++)
      {
        if (index != 0)
        {
          result.append(", ");
        }
        result.append(data::pp(state_component(state, index)));
      }
      result.append(")");
      return result;
    }

    /// \brief Returns the currently loaded specification.
    const lps::specification& get_specification() const
    {
      return m_specification;
    }
};
} // namespace old

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_NEXT_STATE_GENERATOR_H
