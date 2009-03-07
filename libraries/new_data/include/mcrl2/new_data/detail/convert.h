// Author(s): Jeroen Keiren
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
      template < typename Container, typename ForwardTraversalIterator >
      inline Container convert(boost::iterator_range< ForwardTraversalIterator > const& r)
      {
        return Container(r.begin(), r.end());
      }

      // Copy to term list from term_list_iterator range
//      template < typename TargetExpression, typename SourceExpression >
//      inline typename atermpp::term_list< TargetExpression >
//      convert(boost::iterator_range< typename atermpp::term_list_iterator< SourceExpression > > const& r)
//      {
//        return atermpp::term_list< TargetExpression >(static_cast< ATermList >(r.begin()));
//      }

      // Copy to term list from term_list_iterator range
      template < typename TargetExpression, typename SourceExpression >
      inline typename atermpp::term_list< TargetExpression >
      convert(typename atermpp::vector< SourceExpression > const& r)
      {
        return atermpp::term_list< TargetExpression >(r.begin(), r.end());
      }

      // Copy to term list from term_list_random_iterator range
      template < typename TargetExpression, typename SourceExpression >
      inline typename atermpp::term_list< TargetExpression >
      convert(boost::iterator_range< typename detail::term_list_random_iterator< SourceExpression > > const& r)
      {
        if (r.end().list().empty()) {
          return atermpp::term_list< TargetExpression >(r.begin().list());
        }

        return atermpp::term_list< TargetExpression >(r.begin(), r.end());
      }
    } // namespace detail
    /// \endcond

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DETAIL_CONVERT_H

