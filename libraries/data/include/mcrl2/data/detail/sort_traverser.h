// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/sort_traverser.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DETAIL_SORT_TRAVERSER_H
#define MCRL2_DATA_DETAIL_SORT_TRAVERSER_H

#include "mcrl2/data/detail/traverser.h"

namespace mcrl2 {

  namespace data {

    class data_expression_with_variables;

    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Derived >
      class sort_traverser : public traverser< Derived >
      {
        public:
          typedef traverser< Derived > super;

          template < typename Expression >
          void enter(Expression const&)
          {}
          template < typename Expression >
          void leave(Expression const&)
          {}

    using super::operator();

          void operator()(function_symbol const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this)(e.sort());
            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(variable const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this)(e.sort());
            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(basic_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(function_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.domain());
            static_cast< Derived& >(*this)(e.codomain());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(container_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.element_sort());

            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).leave(e);
          }

          void operator()(structured_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            for (structured_sort::constructors_const_range r(e.struct_constructors()); !r.empty(); r.advance_begin(1))
            {
              for (structured_sort_constructor::arguments_const_range j(r.front().arguments()); !j.empty(); j.advance_begin(1))
              {
                static_cast< Derived& >(*this)(j.front().sort());
              }
            }

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(sort_expression const& e)
          {
            if (e.is_basic_sort())
            {
              static_cast< Derived& >(*this)(basic_sort(e));
            }
            else if (e.is_container_sort())
            {
              static_cast< Derived& >(*this)(container_sort(e));
            }
            else if (e.is_structured_sort())
            {
              static_cast< Derived& >(*this)(structured_sort(e));
            }
            else if (e.is_function_sort())
            {
              static_cast< Derived& >(*this)(function_sort(e));
            }
          }

          void operator()(alias const& e)
          {
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this)(e.reference());

            static_cast< Derived& >(*this).leave(e);
          }

          void operator()(assignment_expression const& a)
          {
            if(is_assignment(a))
            {
              return static_cast< Derived& >(*this)(assignment(a));
            }
            else if(is_identifier_assignment(a))
            {
              return static_cast< Derived& >(*this)(identifier_assignment(a));
            }
          }

          void operator()(assignment const& a)
          {
            static_cast< super& >(*this)(a);
          }

          void operator()(identifier_assignment const& a)
          {
            static_cast< super& >(*this)(a);
          }

          void operator()(data_equation const& e)
          {
            static_cast< super& >(*this)(e);
          }

          void operator()(data_specification const& e)
          {
            static_cast< Derived& >(*this)(e.sorts());
            // static_cast< Derived& >(*this)(e.aliases());
            static_cast< super& >(*this)(e);
          }

#ifndef NO_TERM_TRAVERSAL
          // \deprecated
          void operator()(atermpp::aterm_appl const& e)
          {
            if (is_alias(e))
            {
              static_cast< Derived& >(*this)(alias(e));
            }
            else {
              static_cast< super& >(*this)(e);
            }
          }

          // \deprecated
          void operator()(atermpp::aterm const& e)
          {
            static_cast< super& >(*this)(e);
          }

          template < typename Expression >
          void operator()(Expression const& e, typename atermpp::detail::disable_if_container< Expression >::type* = 0)
          {
            static_cast< super& >(*this)(e);
          }
#endif // NO_TERM_TRAVERSAL
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_sort_traverser : public selective_traverser< Derived, AdaptablePredicate, detail::sort_traverser >
      {
        typedef selective_traverser< Derived, AdaptablePredicate, detail::sort_traverser > super;

        public:

          selective_sort_traverser()
          { }

          selective_sort_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

