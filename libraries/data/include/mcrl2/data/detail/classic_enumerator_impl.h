// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enumeration.h
/// \brief Template class for substitution

#ifndef _MCRL2_DATA_DETAIL_CLASSIC_ENUMERATOR__HPP_
#define _MCRL2_DATA_DETAIL_CLASSIC_ENUMERATOR__HPP_

#include <memory>

#include "boost/shared_ptr.hpp"

#include "mcrl2/data/detail/enum/standard.h"
#include "mcrl2/data/detail/convert.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {
  namespace data {

    template < typename MutableSubstitution, typename Evaluator, typename Selector >
    class classic_enumerator;

    template < typename Enumerator >
    class enumerator_factory;

    /// \cond INTERNAL
    namespace detail {

      // trick for accessing data conversions to/from rewritable format 
      template < typename Evaluator >
      struct compatibility_evaluator {
        typedef Evaluator actual_type;
      };

      template < >
      struct compatibility_evaluator< rewriter > : public rewriter {
        typedef compatibility_evaluator actual_type;

        compatibility_evaluator(rewriter const& e) : rewriter(e) {
        }

        atermpp::aterm convert_to(data_expression const& e) const {
          return this->m_rewriter->toRewriteFormat(basic_rewriter< data_expression >::implement(e));
        }

        data_expression convert_from(atermpp::aterm const& e) {
          return basic_rewriter< data_expression >::reconstruct(this->m_rewriter->fromRewriteFormat(e));
        }

        detail::Rewriter& get_rewriter() {
          return *const_cast< detail::Rewriter* >(m_rewriter.get());
        }
      };

      // Shared context
      // Does not contain iterator specific state information. Used for
      // both performance and for making the iterators Multi Pass such that
      // they can be used with Boost.Graph.
      template < typename Evaluator >
      class classic_enumerator_context {

        template < typename X, typename Y, typename Z >
        friend class classic_enumerator_impl;

        // Only for second constructor below
        template < typename E >
        friend class classic_enumerator_context;

        private:

          data_specification const&                                    m_specification;
          typename compatibility_evaluator< Evaluator >::actual_type   m_evaluator;     // Only here for conversion trick
          detail::EnumeratorStandard                                   m_enumerator;    // embedded rewriter should not be part of context

        public:

          /// Limitations in EnumeratorStandard force passing a rewriter
          classic_enumerator_context(data_specification const& specification, Evaluator const& evaluator) :
                    m_specification(specification),
                    m_evaluator(evaluator),
                    m_enumerator(specification, &m_evaluator.get_rewriter()) {
          }

          /// Not for ordinary use, here for extraction trick used in NextState
          template < typename AlternativeEvaluator >
          classic_enumerator_context(classic_enumerator_context< AlternativeEvaluator >& other, AlternativeEvaluator const& evaluator) :
                    m_specification(other.m_specification),
                    m_evaluator(evaluator),
                    m_enumerator(m_specification, &m_evaluator.get_rewriter()) {
          }
      };

      template < typename MutableSubstitution, typename Evaluator, typename Selector >
      class classic_enumerator_impl {

        friend class data::classic_enumerator< MutableSubstitution, Evaluator, Selector >;

        template < typename T >
        friend class data::enumerator_factory;

        private:

          typedef MutableSubstitution                               substitution_type;
          typedef typename MutableSubstitution::expression_type     expression_type;
          typedef typename MutableSubstitution::variable_type       variable_type;
          typedef classic_enumerator_context< Evaluator >           shared_context_type;

        private:

          boost::shared_ptr< shared_context_type >                     m_shared_context;

          // for copy constructor, since it is unsafe to copy EnumeratorSolutionsStandard
          boost::shared_ptr< EnumeratorSolutionsStandard >             m_generator;

          typename compatibility_evaluator< Evaluator >::actual_type&  m_evaluator;

          expression_type                                              m_condition;

          MutableSubstitution                                          m_substitution;

        private:

          // do not use directly, use the create method
          classic_enumerator_impl(boost::shared_ptr< shared_context_type > const& context,
                               expression_type const& c, substitution_type const& s, Evaluator& e) :
                             m_shared_context(context), m_evaluator(context->m_evaluator), m_condition(c), m_substitution(s) {
          }

          bool initialise(std::set< variable_type > const& v) {
            // Apply translation (effectively type normalisation) to variables
            atermpp::aterm_list variables;

            for (typename std::set< variable_type >::const_iterator i = v.begin(); i != v.end(); ++i) {
              variables = atermpp::push_back(variables, m_evaluator.convert_to(*i));
            }

            m_generator.reset(static_cast< EnumeratorSolutionsStandard* >(
                        m_shared_context->m_enumerator.findSolutions(variables, m_evaluator.convert_to(m_condition), false)));

            return increment();
          }

        public:

          // Copy constructor; note that copies share share state due to limitations in the underlying implementation
          classic_enumerator_impl(classic_enumerator_impl const& other) :
                                                 m_shared_context(other.m_shared_context),
                                                 m_generator(other.m_generator),
                                                 m_evaluator(other.m_evaluator),
                                                 m_substitution(other.m_substitution) {
          }

          bool increment() {

            ATermList assignment_list;

            while (m_generator->next(&assignment_list)) {
              if (m_generator->errorOccurred()) {
                throw mcrl2::runtime_error(std::string("Failed enumeration of condition ") + pp(m_condition) + "; cause unknown");
              }

              for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
                                 i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i) {
                m_substitution[static_cast< variable_type >((*i)(0))] =
                               m_evaluator.convert_from((*i)(1));
              }

              if (Selector::test(m_evaluator(m_condition, m_substitution))) {
                return true;
              }
            }

            return false;
          }

          bool equal(classic_enumerator_impl const& other) const {
            return m_substitution == other.m_substitution;
          }

          MutableSubstitution const& dereference() const {
            return m_substitution;
          }

          static void create(boost::shared_ptr< classic_enumerator_impl >& target,
              boost::shared_ptr< shared_context_type > const& context,
                  std::set< variable_type > const& v, expression_type const& c, substitution_type const& s = substitution_type()) {

            target.reset(new classic_enumerator_impl(context, c, s));

            if (!target->initialise(v)) {
              target.reset();
            }
          }

          static void create(boost::shared_ptr< classic_enumerator_impl >& target,
              boost::shared_ptr< shared_context_type > const& context,
                               std::set< variable_type > const& v, expression_type const& c,
                               Evaluator& e, substitution_type const& s = substitution_type()) {

            target.reset(new classic_enumerator_impl(context, c, s, e));

            if (!target->initialise(v)) {
              target.reset();
            }
          }

          static void create(boost::shared_ptr< classic_enumerator_impl >& target,
              data_specification const& specification, std::set< variable_type > const& v,
              expression_type const& c, Evaluator& e, substitution_type const& s = substitution_type()) {

            create(target, boost::shared_ptr< shared_context_type >(new shared_context_type(specification, const_cast< Evaluator& >(e))), v, c, e, s);
          }
      };
    }
    /// \endcond
  } // namespace data
} // namespace mcrl2

#endif

