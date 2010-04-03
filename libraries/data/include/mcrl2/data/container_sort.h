// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/container_sort.h
/// \brief The class container_sort.

#ifndef MCRL2_DATA_CONTAINER_SORT_H
#define MCRL2_DATA_CONTAINER_SORT_H

#include <iostream>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/container_type.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief A container sort
class container_sort_base: public sort_expression
{
  public:
    /// \brief Default constructor.
    container_sort_base()
      : sort_expression(core::detail::constructSortCons())
    {}

    /// \brief Constructor.
    /// \param term A term
    container_sort_base(atermpp::aterm_appl term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortCons(m_term));
    }

    /// \brief Constructor.
    container_sort_base(const container_type& container_name, const sort_expression& element_sort)
      : sort_expression(core::detail::gsMakeSortCons(container_name, element_sort))
    {}

    container_type container_name() const
    {
      return atermpp::arg1(*this);
    }

    sort_expression element_sort() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated class ---//

    } //namespace detail

    /// \brief container sort.
    ///
    /// Container sorts are sorts with a name and an element sort.
    /// An example of a container sort is List(S), where List is the name of
    /// the container, and S is the element sort.
    /// Currently only the containers List, Set, FSet, Bag and FBag are
    /// supported.
    class container_sort: public detail::container_sort_base
    {
      public:

        /// \overload
        container_sort()
          : detail::container_sort_base()
        {}

        /// \overload
        container_sort(const atermpp::aterm_appl& s)
          : detail::container_sort_base(s)
        {}

        /// \overlaod
        container_sort(const container_type& container_name,
                       const sort_expression& element_sort)
          : detail::container_sort_base(container_name, element_sort)
        {}

        /// \brief Returns true iff container name is List.
        ///
        inline
        bool is_list_sort() const
        {
          return container_name() == list_container();
        }

        /// \brief Returns true iff container name is Set.
        ///
        inline
        bool is_set_sort() const
        {
          return container_name() == set_container();
        }

        /// \brief Returns true iff container name is FSet.
        ///
        inline
        bool is_fset_sort() const
        {
          return container_name() == fset_container();
        }

        /// \brief Returns true iff container name is Bag.
        ///
        inline
        bool is_bag_sort() const
        {
          return container_name() == bag_container();
        }

        /// \brief Returns true iff container name is FBag.
        ///
        inline
        bool is_fbag_sort() const
        {
          return container_name() == fbag_container();
        }

    }; // class container_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::term_list<container_sort> container_sort_list;

    /// \brief list of function sorts
    ///
    typedef atermpp::vector<container_sort> container_sort_vector;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

