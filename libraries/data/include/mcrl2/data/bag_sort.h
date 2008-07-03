// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/bag_sort.h
/// \brief The class bag_sort.

#ifndef MCRL2_DATA_BAG_SORT_H
#define MCRL2_DATA_BAG_SORT_H

#include <iostream>
#include <boost/range/iterator_range.hpp>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/container_sort.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief list sort.
    ///
    class bag_sort: public container_sort
    {

      public:    

        /// \brief Constructor
        ///
        bag_sort()
          : container_sort()
        {}

        /// \brief Constructor
        ///
        /// \param [in] element_sort The sort of elements in the bag.
        bag_sort(sort_expression element_sort)
          : container_sort("Bag", element_sort)
        {}

        /// \overload
        ///
        inline
        std::string container_name() const
        {
          return "Bag";
        }

        /// \overload
        ///
        inline
        bool is_list_sort() const
        {
          return false;
        }

        /// \overload
        ///
        inline
        bool is_set_sort() const
        {
          return false;
        }

        /// \overload
        ///
        inline
        bool is_bag_sort() const
        {
          return true;
        }

    }; // class bag_sort

    /// \brief list of bag sorts
    ///
    typedef atermpp::vector<bag_sort> bag_sort_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::bag_sort)
/// \endcond

#endif // MCRL2_DATA_BAG_SORT_H

