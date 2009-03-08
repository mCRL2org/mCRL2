// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/convert.h
/// \brief Conversion utilities for converting between the old aterm format
///        and the new c++ implementation.

#ifndef MCRL2_NEW_DATA_DETAIL_CONVERT_H
#define MCRL2_NEW_DATA_DETAIL_CONVERT_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/new_data/detail/container_utility.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {
      template < typename TargetContainer >
      struct converter {
        template < typename ForwardTraversalIterator >
        static TargetContainer convert(boost::iterator_range< ForwardTraversalIterator > const& r) {
          return TargetContainer(r.begin(), r.end());
        }

        template < typename Container >
        static TargetContainer convert(Container l,
          typename boost::enable_if< typename detail::is_container< Container >::type >::type* = 0) {

          return TargetContainer(l.begin(), l.end());
        }

        template < typename ForwardTraversalIterator >
        static TargetContainer convert(ATermList l) {
          return Container(atermpp::term_list< typename TargetContainer::value_type >(l),
                           atermpp::term_list< typename TargetContainer::value_type >());
        }
      };

      // Copy to term list
      template < typename TargetExpression >
      struct converter< atermpp::term_list< TargetExpression > > {
        template < typename ForwardTraversalIterator >
        struct selector {
          static atermpp::term_list< TargetExpression >
          convert(boost::iterator_range< ForwardTraversalIterator > const& r) {
            return atermpp::term_list< TargetExpression >(r.begin(), r.end());
          }
        };

        template < typename ForwardTraversalIterator >
        static atermpp::term_list< TargetExpression > convert(boost::iterator_range< ForwardTraversalIterator > const& r) {
          return selector< ForwardTraversalIterator >::convert(r);
        }

        template < typename Container >
        static atermpp::term_list< TargetExpression > convert(Container l,
          typename boost::enable_if< typename detail::is_container< Container >::type >::type* = 0) {

          return atermpp::term_list< TargetExpression >(l.begin(), l.end());
        }

        template < typename ForwardTraversalIterator >
        static atermpp::term_list< TargetExpression > convert(ATermList l) {
          return atermpp::term_list< TargetExpression >(l);
        }
      };

      // Copy to term list from term_list_iterator range
      template < typename TargetExpression >
      template < typename SourceExpression >
      struct converter< atermpp::term_list< TargetExpression > >::selector< atermpp::term_list_iterator< SourceExpression > > {
        template < typename ForwardTraversalIterator >
        static atermpp::term_list< TargetExpression >
        convert(typename boost::iterator_range< ForwardTraversalIterator > const& r) {
          if (ATisEmpty(r.end().list())) {
            return atermpp::term_list< TargetExpression >(r.begin().list());
          }

          return atermpp::term_list< TargetExpression >(r.begin(), r.end());
        }
      };

      // Copy to term list from term_list_iterator range
      template < typename TargetExpression >
      template < typename SourceExpression >
      struct converter< atermpp::term_list< TargetExpression > >::selector< detail::term_list_random_iterator< SourceExpression > > {
        template < typename ForwardTraversalIterator >
        static atermpp::term_list< TargetExpression >
        convert(typename boost::iterator_range< ForwardTraversalIterator > const& r) {
          if (r.end().list().empty()) {
            return atermpp::term_list< TargetExpression >(r.begin().list());
          }

          return atermpp::term_list< TargetExpression >(r.begin(), r.end());
        }
      };

    } // namespace detail
    /// \endcond

    /// \brief Convert container with expressions to a new container with expressions
    template < typename TargetContainer, typename SourceContainer >
    TargetContainer convert(SourceContainer const& c) {
      return detail::converter< TargetContainer >::convert(c);
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DETAIL_CONVERT_H

