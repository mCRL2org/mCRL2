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

      // Shared context
      // Does not contain iterator specific state information. Used for
      // both performance and for making the iterators Multi Pass such that
      // they can be used with Boost.Graph.
      class classic_enumerator_context {

        template < typename MutableSubstitution, typename Evaluator, typename Selector >
        friend class classic_enumerator_impl;

        private:

          data_specification         m_specification;
          detail::EnumeratorStandard m_enumerator;    // embedded rewriter should not be part of context

        public:

          /// Limitations in EnumeratorStandard force passing a rewriter
          /// e.get_rewriter() is not required but at this moment data::rewriter is the currently only model of Evaluator
          template < typename Evaluator >
          classic_enumerator_context(data_specification const& specification, Evaluator const& evaluator) :
                    m_specification(specification),
                    m_enumerator(specification, &const_cast< detail::Rewriter& >(evaluator.get_rewriter())) {
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
          typedef classic_enumerator_context                        shared_context_type;

        private:

          boost::shared_ptr< shared_context_type >         m_shared_context;

          // for copy constructor, since it is unsafe to copy EnumeratorSolutionsStandard
          boost::shared_ptr< EnumeratorSolutionsStandard > m_generator;

          Evaluator                                        m_evaluator;

          expression_type                                  m_condition;

          MutableSubstitution                              m_substitution;

        private:

          // do not use directly, use the create method
          classic_enumerator_impl(boost::shared_ptr< shared_context_type > const& context,
                                      expression_type const& c, substitution_type const& s) :
                             m_shared_context(context), m_evaluator(context->m_specification), m_condition(c), m_substitution(s) {

            m_evaluator(c); // adds the proper rewrite rules (for legacy Enumerator/Rewriter)
          }

          // do not use directly, use the create method
          classic_enumerator_impl(boost::shared_ptr< shared_context_type > const& context,
                               expression_type const& c, substitution_type const& s, Evaluator const& e) :
                             m_shared_context(context), m_evaluator(e), m_condition(c), m_substitution(s) {

            m_evaluator(c); // adds the proper rewrite rules (for legacy Enumerator/Rewriter)
          }

          bool initialise(std::set< variable_type > const& v) {
            // trick for accessing data implementation
            struct local : public basic_rewriter< expression_type > {
              local(basic_rewriter< expression_type > const& e) : basic_rewriter< expression_type >(e) {
              }

              atermpp::aterm translate(data_expression const& e) const {
                return basic_rewriter< expression_type >::m_rewriter->toRewriteFormat(basic_rewriter< expression_type >::implement(e));
              }
            };

            local converter(m_evaluator);

            // Apply translation (effectively type normalisation) to variables
            atermpp::aterm_list variables;

            for (typename std::set< variable_type >::const_iterator i = v.begin(); i != v.end(); ++i) {
              variables = atermpp::push_back(variables, converter.translate(*i));
            }

            m_generator.reset(static_cast< EnumeratorSolutionsStandard* >(
                        m_shared_context->m_enumerator.findSolutions(variables, converter.translate(m_condition), false)));

            return increment();
          }

        public:

          // Copy constructor; note that copies share share state due to limitations in the underlying implementation
          classic_enumerator_impl(classic_enumerator_impl const& other) :
                                                 m_shared_context(other.m_shared_context),
                                                 m_substitution(other.m_substitution),
                                                 m_generator(other.m_generator) {
          }

          bool increment() {
            // trick for accessing data implementation
            struct local : public basic_rewriter< expression_type > {
              local(basic_rewriter< expression_type > const& e) : basic_rewriter< expression_type >(e) {
              }

              expression_type translate(atermpp::aterm const& e) {
                return basic_rewriter< expression_type >::reconstruct(basic_rewriter< expression_type >::m_rewriter->fromRewriteFormat(e));
              }
            };

            local converter(m_evaluator);

            ATermList assignment_list;

            while (m_generator->next(&assignment_list)) {
              if (m_generator->errorOccurred()) {
                throw mcrl2::runtime_error(std::string("Failed enumeration of condition ") + pp(m_condition) + "; cause unknown");
              }

              for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
                                 i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i) {
                m_substitution[static_cast< variable_type >((*i)(0))] =
                               converter.translate((*i)(1));
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
             data_specification const& specification, std::set< variable_type > const& v,
             expression_type const& c, substitution_type const& s = substitution_type()) {

            /// Limitations in EnumeratorStandard force passing a rewriter
            Evaluator evaluator(specification);

            create(target, boost::shared_ptr< shared_context_type >(
                  new shared_context_type(specification, evaluator)), v, c, evaluator, s);
          }

          static void create(boost::shared_ptr< classic_enumerator_impl >& target,
              boost::shared_ptr< shared_context_type > const& context,
                               std::set< variable_type > const& v, expression_type const& c,
                               Evaluator const& e, substitution_type const& s = substitution_type()) {

            target.reset(new classic_enumerator_impl(context, c, s, e));

            if (!target->initialise(v)) {
              target.reset();
            }
          }

          static void create(boost::shared_ptr< classic_enumerator_impl >& target,
              data_specification const& specification, std::set< variable_type > const& v,
              expression_type const& c, Evaluator const& e, substitution_type const& s = substitution_type()) {

            create(target, boost::shared_ptr< shared_context_type >(new shared_context_type(specification, const_cast< Evaluator& >(e))), v, c, e, s);
          }
      };
    }
    /// \endcond
  } // namespace data
} // namespace mcrl2

#endif

