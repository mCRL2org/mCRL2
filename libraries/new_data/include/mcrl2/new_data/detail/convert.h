// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/convert.h
/// \brief Conversion utilities for converting between the containers with
///    expressions and term lists that contain expressions

#ifndef MCRL2_NEW_DATA_DETAIL_CONVERT_H
#define MCRL2_NEW_DATA_DETAIL_CONVERT_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/new_data/detail/container_utility.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {
      template < typename TargetContainer, typename SourceContainer,
                 typename TargetExpression = typename TargetContainer::value_type,
                 typename SourceExpression = typename SourceContainer::value_type >
      struct converter {
        template < typename Container >
        static TargetContainer convert(Container const& l) {
          return TargetContainer(l.begin(), l.end());
        }
      };

      // Copy to from term list to term list
      template < typename TargetExpression, typename SourceExpression >
      struct converter< atermpp::term_list< TargetExpression >,
                        atermpp::term_list< SourceExpression >,
	                TargetExpression, SourceExpression > {

        static atermpp::term_list< TargetExpression >
	convert(atermpp::term_list< SourceExpression > const& r) {
          if (ATisEmpty(r.end().list())) {
            return atermpp::term_list< TargetExpression >(r.begin().list());
          }

          return atermpp::term_list< TargetExpression >(r.begin(), r.end());
        }
      };

      template < typename TargetExpression, typename SourceExpression >
      struct converter< atermpp::term_list< TargetExpression >,
                        boost::iterator_range< atermpp::term_list_iterator< SourceExpression > >,
	                TargetExpression, SourceExpression > {

        static atermpp::term_list< TargetExpression >
	convert(boost::iterator_range< atermpp::term_list_iterator< SourceExpression > > const& r) {

          if (ATisEmpty(r.end().list())) {
            return atermpp::term_list< TargetExpression >(r.begin().list());
          }

          return atermpp::term_list< TargetExpression >(r.begin(), r.end());
        }
      };

      template < typename TargetExpression, typename SourceExpression >
      struct converter< atermpp::term_list< TargetExpression >,
                        boost::iterator_range< detail::term_list_random_iterator< SourceExpression > >,
	                TargetExpression, SourceExpression > {

        static atermpp::term_list< TargetExpression >
	convert(boost::iterator_range< detail::term_list_random_iterator< SourceExpression > > const& r) {

          if (ATisEmpty(r.end().list())) {
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
      return detail::converter< TargetContainer, SourceContainer >::convert(c);
    }

    /// \brief Convert container with expressions to a new container with expressions
    template < typename TargetContainer >
    TargetContainer convert(ATermList l) {
      return detail::converter< TargetContainer,
               atermpp::term_list< typename TargetContainer::value_type > >::
                        convert(atermpp::term_list< typename TargetContainer::value_type >(l));
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DETAIL_CONVERT_H

