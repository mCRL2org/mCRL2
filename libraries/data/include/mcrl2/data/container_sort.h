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
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/detail/construction_utility.h"

namespace mcrl2 {

  namespace data {

    /// \brief container sort.
    ///
    /// Container sorts are sorts with a name and an element sort.
    class container_sort: public sort_expression
    {
      protected:

        // base class for abstraction types
        struct variant : public atermpp::aterm_appl {
          variant(atermpp::aterm_appl const& e) : atermpp::aterm_appl(e)
          {}
        };

      public:

        // Type for list variant
        struct list : public detail::singleton_expression< container_sort::list, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortList();
          }
        };
        // Type for set_ variant
        struct set_ : public detail::singleton_expression< container_sort::set_, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortSet();
          }
        };
        // Type for set_ variant
        struct fset : public detail::singleton_expression< container_sort::fset, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortFSet();
          }
        };
        // Type for set_ variant
        struct bag : public detail::singleton_expression< container_sort::bag, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortBag();
          }
        };
        // Type for set_ variant
        struct fbag : public detail::singleton_expression< container_sort::fbag, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortFBag();
          }
        };

      public:

        /// \brief Constructor
        ///
        container_sort()
          : sort_expression(core::detail::constructSortCons())
        {}

        /// \brief Constructor
        ///
        /// \param[in] s A sort expression.
        /// \pre s has the internal structure of a container sort.
        container_sort(const sort_expression& s)
          : sort_expression(s)
        {
          assert(s.is_container_sort());
        }

        /// \brief Constructor
        ///
        /// \param[in] container_name Name of the container, should be one of
        ///            "List", "Set" or "Bag".
        /// \param[in] element_sort The sort of elements in the container.
        container_sort(const container_sort::variant& container_name,
                       const sort_expression& element_sort)
          : sort_expression(core::detail::gsMakeSortCons(container_name, element_sort))
        {}

        /// \overload
        ///
        inline
        bool is_container_sort() const
        {
          return true;
        }

        /// \brief Returns the container name.
        ///
        inline
        container_sort::variant container_type() const
        {
          return atermpp::arg1(*this);
        }

        /// \brief Returns the element sort.
        ///
        inline
        sort_expression element_sort() const
        {
          return atermpp::arg2(*this);
        }

        /// \brief Returns true iff container name is List.
        ///
        inline
        bool is_list_sort() const
        {
          return container_type() == container_sort::list::instance();
        }

        /// \brief Returns true iff container name is Set.
        ///
        inline
        bool is_set_sort() const
        {
          return container_type() == container_sort::set_::instance();
        }

        /// \brief Returns true iff container name is Bag.
        ///
        inline
        bool is_bag_sort() const
        {
          return container_type() == container_sort::bag::instance();
        }

    }; // class container_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::term_list<container_sort> container_sort_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

