// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/set_sort.h
/// \brief The class set_sort.

#ifndef MCRL2_DATA_SET_SORT_H
#define MCRL2_DATA_SET_SORT_H

#include <iostream>
#include <boost/range/iterator_range.hpp>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/container_sort.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief list sort.
    ///
    class set_sort: public container_sort
    {

      public:    

        set_sort()
          : container_sort()
        {}

        set_sort(sort_expression element_sort)
          : container_sort("Set", element_sort)
        {}

        /// \overload
        ///
        inline
        std::string container_name() const
        {
          return "Set";
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
          return true;
        }

        /// \overload
        ///
        inline
        bool is_bag_sort() const
        {
          return false;
        }

    }; // class set_sort

    /// \brief list of set sorts
    ///
    typedef atermpp::vector<set_sort> set_sort_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::set_sort)
/// \endcond

#endif // MCRL2_DATA_SET_SORT_H

