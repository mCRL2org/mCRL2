// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator_factory.h
/// \brief Template class for convenient creation of data enumerators

#ifndef _MCRL2_DATA_ENUMERATOR_FACTORY__HPP_
#define _MCRL2_DATA_ENUMERATOR_FACTORY__HPP_

#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/classic_enumerator.h"

namespace mcrl2 {
  namespace data {

    template < typename Enumerator >
    class enumerator_factory;

    /** \brief Factory for enumerator objects for a single data specification
     *
     * Specialisation for template data::classic_enumerator.
     *
     * Type parameters:
     *  \arg MutableSubstitution model of MutableSubstitution and default for constructed enumerators
     *  \arg Evaluator model of Evaluator and default for constructed enumerators
     *  \arg Selector model of Selector and default for constructed enumerators
     *
     * The type parameters select the default template parameters which are
     * used to construct enumerators. A number of overloads is available for
     * deviating from these defaults on demand.
     *
     * The factory serves as both a convenient way of generating enumerators
     * for a single specification, as well as that it may increase performance
     * by dividing initialisation overhead accross all enumerators that are
     * constructed.
     **/
    template < typename MutableSubstitution, typename Evaluator, typename Selector >
    class enumerator_factory< classic_enumerator< MutableSubstitution, Evaluator, Selector > > {

      public:

       /// \brief The type of objects that represent substitutions
       typedef MutableSubstitution                                                       substitution_type;
        /// \brief The default condition evaluator type for constructed enumerators
       typedef Evaluator                                                                 evaluator_type;
        /// \brief The default selector type for constructed enumerators
       typedef Selector                                                                  selector_type;
       /// \brief The default enumerator type
       typedef classic_enumerator< substitution_type, evaluator_type, selector_type >    enumerator_type;
        /// \brief The type of objects that represent variables
       typedef typename substitution_type::variable_type                                 variable_type;
        /// \brief The type of objects that represent expressions
       typedef typename substitution_type::expression_type                               expression_type;

      protected:

        /// \brief Type of shared context for enumerators created by the factory
        typedef typename enumerator_type::shared_context_type              shared_context_type;

        /// \brief An internal evaluator object
        Evaluator                                m_internal_evaluator;
        /// \brief Reference to a possibly external evaluator object
        Evaluator&                               m_evaluator;
        /// \brief Context shared by enumerators
        boost::shared_ptr< shared_context_type > m_enumeration_context;

        /// \brief Default constructor
        enumerator_factory() : m_evaluator(m_internal_evaluator) {
        }

        /// \brief Constructor with shared context and evaluator instance
        /// \param[in] context the shared context for enumerators
        /// \param[in] evaluator a reference to an evaluator
        enumerator_factory(boost::shared_ptr< shared_context_type > const& context, Evaluator const& evaluator) :
               m_internal_evaluator(evaluator),
               m_evaluator(m_internal_evaluator),
               m_enumeration_context(context) {
        }

      public:

        /// \brief Copy constructor
        /// \param[in] other the original to copy
        enumerator_factory(enumerator_factory const& other) :
               m_internal_evaluator(other.m_evaluator),
               m_evaluator(other.m_evaluator),
               m_enumeration_context(other.m_enumeration_context) {
        }

        /// \brief Constructor with data specification
        enumerator_factory(data_specification const& specification, Evaluator const& evaluator) :
               m_internal_evaluator(evaluator), m_evaluator(evaluator),
               m_enumeration_context(new shared_context_type(specification, m_evaluator)) {
        }

        /// \brief Constructor with data specification (does not copy evaluator)
        enumerator_factory(data_specification const& specification, Evaluator& evaluator) :
               m_internal_evaluator(evaluator), m_evaluator(evaluator),
               m_enumeration_context(new shared_context_type(specification, m_evaluator)) {
        }

        /** \brief Constructor with data specification and evaluator
         * \param[in] specification a data specification that serves as context
         * \note Only available when Evaluator can be constructed from data_specification
         **/
        enumerator_factory(data_specification const& specification) :
               m_internal_evaluator(specification), m_evaluator(m_internal_evaluator),
               m_enumeration_context(new shared_context_type(specification, m_evaluator.get_rewriter())) {
        }

        /** \brief Creates enumerator using a default constructed condition evaluator
         *
         * \param[in] variable a single variable for which to find valuations
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        enumerator_type make(variable_type const& variable, expression_type const& condition = expression_traits< expression_type >::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return enumerator_type(m_enumeration_context, variable, condition, substitution, m_evaluator);
        }

        /** \brief Creates enumerator using a default constructed condition evaluator
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        enumerator_type make(std::set< variable_type > const& variables, expression_type const& condition = expression_traits< expression_type >::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return enumerator_type(m_enumeration_context, variables, condition, substitution, m_evaluator);
        }

        /** \brief Creates enumerator using the default type
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] evaluator a condition evaluator object
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        enumerator_type make(std::set< variable_type > const& variables, Evaluator& evaluator,
                         expression_type const& condition = expression_traits< expression_type >::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return enumerator_type(m_enumeration_context, variables, condition, substitution, evaluator);
        }

        /** \brief Creates enumerator with an alternative condition evaluator component
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] evaluator a condition evaluator object
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        template < typename AlternativeEvaluator >
        classic_enumerator< substitution_type, AlternativeEvaluator, selector_type >
          make(std::set< variable_type > const& variables, AlternativeEvaluator& evaluator,
                         expression_type const& condition = expression_traits< expression_type >::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return classic_enumerator<
                       substitution_type, AlternativeEvaluator, selector_type >
                                                        (m_enumeration_context, variables, condition, substitution, evaluator);
        }

        /** \brief Creates enumerator with an alternative selector component
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] evaluator a condition evaluator object
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        template < typename AlternativeSelector >
        classic_enumerator< substitution_type, evaluator_type, AlternativeSelector >
          make(std::set< variable_type > const& variables, evaluator_type& evaluator,
            expression_type const& condition = expression_traits< expression_type >::true_(),
            substitution_type const& substitution = substitution_type()) const {

          return classic_enumerator< substitution_type, evaluator_type, AlternativeSelector >
                                                        (m_enumeration_context, variables, condition, substitution, evaluator);
        }

        /** \brief Creates enumerator with an alternative selector component
         *
         * The default condition evaluator component is passed.
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        template < typename AlternativeSelector >
        classic_enumerator< substitution_type, evaluator_type, AlternativeSelector >
          make(std::set< variable_type > const& variables,
                         expression_type const& condition = expression_traits< expression_type >::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return classic_enumerator< substitution_type, evaluator_type, AlternativeSelector >
                                                        (m_enumeration_context, variables, condition, substitution, m_evaluator);
        }

        /** \brief Creates enumerator with alternative condition evaluator and selector components
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] evaluator a condition evaluator object
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        template < typename AlternativeEvaluator, typename AlternativeSelector >
        classic_enumerator< substitution_type, AlternativeEvaluator, AlternativeSelector >
          make(std::set< variable_type > const& variables, AlternativeEvaluator& evaluator,
                         expression_type const& condition = sort_bool_::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return classic_enumerator< substitution_type, AlternativeEvaluator, AlternativeSelector >
                                                        (m_enumeration_context, variables, condition, substitution, evaluator);
        }

        /** \brief Creates enumerator with alternative condition evaluator and selector components
         *
         * The default condition evaluator component is passed.
         *
         * \param[in] variables the set of variables for which to find valuations
         * \param[in] condition the enumeration condition
         * \param[in] substitution template for substitutions
         **/
        template < typename AlternativeEvaluator, typename AlternativeSelector >
        classic_enumerator< substitution_type, AlternativeEvaluator, AlternativeSelector >
          make(std::set< variable_type > const& variables,
                         expression_type const& condition = sort_bool_::true_(),
                         substitution_type const& substitution = substitution_type()) const {

          return classic_enumerator< substitution_type, AlternativeEvaluator, AlternativeSelector >
                                                        (m_enumeration_context, variables, condition, substitution, m_evaluator);
        }
    };
  }
}

#endif

