// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/next_state_generator.h
/// \brief Iterator interface for generating next states of a linear process specification.

#ifndef MCRL2_LPS_NEXT_STATE_GENERATOR_H
#define MCRL2_LPS_NEXT_STATE_GENERATOR_H

#include <sstream>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  /// \brief Class for generating a state space corresponding to a linear process specification.
  class next_state_generator
  {
    protected:
      const specification& m_specification;
      legacy_rewriter m_rewriter;
      data::enumerator_factory<mcrl2::data::classic_enumerator<> > m_enumerator;

      // Find function symbols not part of the data specification of lps_spec
      std::set<data::function_symbol> find_function_symbols(const specification& lps_spec, bool add_symbols_for_global_variables = true) const
      {
        std::set<data::function_symbol> result;

        atermpp::aterm_appl context(specification_to_aterm(lps_spec));
        atermpp::aterm_appl::const_iterator start = ++context.begin();

        ++start;

        if (add_symbols_for_global_variables)
        {
          data::variable_list variables(atermpp::aterm_appl(*start)(0));

          // Compensate for symbols that could be used as part of an instantiation of free variables
          for (data::variable_list::const_iterator j = variables.begin(); j != variables.end(); ++j)
          {
            data::data_specification::constructors_const_range r = lps_spec.data().constructors(j->sort());
            result.insert(r.begin(), r.end());
            r = lps_spec.data().mappings(j->sort());
            result.insert(r.begin(), r.end());
          }
        }

        // Trick, traverse all but the data specification
        for (atermpp::aterm_appl::const_iterator i = ++start; i != context.end(); ++i)
        {
          data::detail::make_find_helper<data::function_symbol, data::detail::traverser>
                 (std::inserter(result, result.end()))(*i);
        }
        return result;
      }

    public:
      /// \brief A type that represents a transition to a 'next' state.
      struct state_type
      {
        ATermAppl transition;
        ATerm state;

        state_type()
          : transition(0),
            state(0)
        {}

        state_type(const state_type& other)
          : transition(other.transition),
            state(other.state)
        {}

        state_type(ATermAppl transition_, ATerm state_)
          : transition(transition_),
            state(state_)
        {}
      };

      /// \brief Iterator that generates all successor states of a given state.
      // TODO: Note that the NextState and NextStateGenerator classes have a clumsy interface.
      // This has a negative effect on the performance.
      class iterator: public boost::iterator_facade<
              iterator,                               // Derived
              const state_type,                       // Value
              boost::forward_traversal_tag            // CategoryOrTraversal
          >
      {
       protected:
         boost::shared_ptr<NextState> m_next_state;
         boost::shared_ptr<NextStateGenerator> m_generator;
         state_type m_state;

       public:
          /// \brief Default constructor.
          iterator()
          {}

          /// \brief Constructor.
          /// The underlying NextStateGenerator can not handle deadlock summands, therefore an exception
          /// is thrown if the specification contains deadlock summands.
          iterator(const specification& lps_spec,
                   mcrl2::data::enumerator_factory<mcrl2::data::classic_enumerator<> >& enumerator
                  )
            : m_next_state(createNextState(lps_spec, enumerator, false)),
              m_generator(m_next_state->getNextStates(m_next_state->getInitialState())),
              m_state(0, m_next_state->getInitialState())
          {
            if (!lps_spec.process().deadlock_summands().empty())
            {
              throw mcrl2::runtime_error("can not generate next states for a process containing deadlock summands");
            }
         }

          /// \brief Constructor.
          /// The underlying NextStateGenerator can not handle deadlock summands, therefore an exception
          /// is thrown if the specification contains deadlock summands.
          iterator(const specification& lps_spec,
                   mcrl2::data::enumerator_factory<mcrl2::data::classic_enumerator<> >& enumerator,
                   const state_type& state
                  )
            : m_next_state(createNextState(lps_spec, enumerator, false)),
              m_generator(m_next_state->getNextStates(state.state)),
              m_state(state)
          {
            if (!lps_spec.process().deadlock_summands().empty())
            {
              throw mcrl2::runtime_error("can not generate next states for a process containing deadlock summands");
            }
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
            assert(m_next_state.get() != 0);
            return m_state;
          }

          /// \brief Increments the iterator
          void increment()
          {
            if (!m_generator->next(&m_state.transition, &m_state.state))
            {
              // empty m_next_state, to signal that there is no next state
              m_next_state.reset();
            }
          }
      };

      /// \brief Constructor.
      next_state_generator(const specification& lps_spec)
        : m_specification(lps_spec),
          m_rewriter(lps_spec.data(), data::used_data_equation_selector(lps_spec.data(), find_function_symbols(lps_spec))),
          m_enumerator(lps_spec.data(), m_rewriter)
      {}

      /// \brief Returns an iterator for generating the successors of the initial state.
      iterator begin()
      {
        return iterator(m_specification, m_enumerator);
      }

      /// \brief Returns a string representation of the given state.
      std::string print_state(const state_type& state) const
      {
        atermpp::aterm_appl s(reinterpret_cast<ATermAppl>(state.state));
        std::string result("state(");
        int index = 0;
        for(atermpp::aterm_appl::const_iterator i = s.begin(); i != s.end(); ++i)
        {
          if(index++ != 0) { result.append(", "); }
      
          result.append(core::pp(atermpp::aterm(m_rewriter.translate(static_cast<ATerm>(*i)))));
        }
        result.append(")");
        return result;
      }
  };

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_NEXT_STATE_GENERATOR_H
