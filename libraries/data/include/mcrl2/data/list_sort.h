// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/list_sort.h
/// \brief The class list_sort.

#ifndef MCRL2_DATA_LIST_SORT_H
#define MCRL2_DATA_LIST_SORT_H

#include <iostream>
#include <boost/range/iterator_range.hpp>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/container_sort.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief list sort.
    ///
    class list_sort: public container_sort
    {

      public:    

        /// \brief Constructor.
        list_sort()
          : container_sort()
        {}

        /// \brief Constructor.
        ///
        /// \param[s] A sort expression.
        /// \pre s is a container sort.
        list_sort(const container_sort& s)
          : container_sort(s)
        {
          assert(s.is_container_sort());
        }

        /// \brief Constructor.
        ///
        /// \param[element_sort] The sort of elements of the list.
        list_sort(const sort_expression& element_sort)
          : container_sort("List", element_sort)
        {}

    }; // class list_sort

    /// \brief list of list sorts
    ///
    typedef atermpp::vector<list_sort> list_sort_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::list_sort)
/// \endcond

#endif // MCRL2_DATA_LIST_SORT_H

