// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/binding_aware_traverser.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DETAIL_BINDING_AWARE_TRAVERSER_H
#define MCRL2_DATA_DETAIL_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/data/detail/traverser.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Derived >
      class binding_aware_traverser : public traverser< Derived > {

        typedef traverser< Derived > super;

        protected:

          std::multiset< variable > m_bound;

          template < typename Container >
          void increase_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
          {
            for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
            {
              m_bound.insert(*i);
            }
          }

          template < typename Container >
          void decrease_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
          {
            for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
            {
              m_bound.erase(m_bound.find(*i));
            }
          }

          template < typename Abstraction >
          void visit(Abstraction const& a) {
            increase_bind_count(a.variables());

            super::visit(a);

            decrease_bind_count(a.variables());
          }

        public:

          using super::operator();

          void operator()(where_clause const& w)
          {
            increase_bind_count(make_assignment_left_hand_side_range(w.declarations()));

            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(w));
            static_cast< Derived& >(*this).enter(w);
            super::operator()(make_assignment_left_hand_side_range(w.declarations()));
            static_cast< Derived& >(*this)(w.body());
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(w));
            static_cast< Derived& >(*this).leave(w);

            decrease_bind_count(make_assignment_left_hand_side_range(w.declarations()));
          }

          void operator()(lambda const& a)
          {
            visit(a);
          }

          void operator()(exists const& a)
          {
            visit(a);
          }

          void operator()(forall const& a)
          {
            visit(a);
          }

          bool is_bound(variable const& v) const
          {
            return m_bound.find(v) != m_bound.end();
          }

          binding_aware_traverser()
          { }

          template < typename Container >
          binding_aware_traverser(Container const& bound_by_context,
                                    typename detail::enable_if_container< Container, variable >::type* = 0) :
                              m_bound(bound_by_context.begin(), bound_by_context.end())
          { }

          virtual ~binding_aware_traverser()
          { }
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_binding_aware_traverser : public selective_traverser< Derived, AdaptablePredicate, detail::binding_aware_traverser >
      {
        typedef selective_traverser< Derived, AdaptablePredicate, detail::binding_aware_traverser > super;

        public:

          selective_binding_aware_traverser()
          { }

          selective_binding_aware_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

