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
    /// An example of a container sort is List(S), where List is the name of
    /// the container, and S is the element sort.
    /// Currently only the containers List, Set, FSet, Bag and FBag are
    /// supported.
    class container_sort: public sort_expression
    {
      protected:

        // base class for list types
        struct variant : public atermpp::aterm_appl {
          variant(atermpp::aterm_appl const& e) : atermpp::aterm_appl(e)
          {}
        };

      public:

        /// \brief Type for list variant
        struct list : public detail::singleton_expression< container_sort::list, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortList();
          }
        };

        /// \brief Type for set_ variant
        struct set_ : public detail::singleton_expression< container_sort::set_, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortSet();
          }
        };

        /// \brief Type for fset variant
        struct fset : public detail::singleton_expression< container_sort::fset, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortFSet();
          }
        };

        /// \brief Type for bag variant
        struct bag : public detail::singleton_expression< container_sort::bag, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortBag();
          }
        };

        /// \brief Type for fbag variant
        struct fbag : public detail::singleton_expression< container_sort::fbag, container_sort::variant > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeSortFBag();
          }
        };

      public:

        /// \brief Default constructor
        ///
        /// Note that this does not entail a valid sort expression.
        ///
        container_sort()
          : sort_expression(core::detail::constructSortCons())
        {}

        /// \brief Construct a container sort from a sort expression.
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
        /// \param[in] container_name A container variant.
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

        /// \brief Returns the container variant.
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
          return container_type() == container_sort::list();
        }

        /// \brief Returns true iff container name is Set.
        ///
        inline
        bool is_set_sort() const
        {
          return container_type() == container_sort::set_();
        }

        /// \brief Returns true iff container name is FSet.
        ///
        inline
        bool is_fset_sort() const
        {
          return container_type() == container_sort::fset();
        }

        /// \brief Returns true iff container name is Bag.
        ///
        inline
        bool is_bag_sort() const
        {
          return container_type() == container_sort::bag();
        }

        /// \brief Returns true iff container name is FBag.
        ///
        inline
        bool is_fbag_sort() const
        {
          return container_type() == container_sort::fbag();
        }

    }; // class container_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::term_list<container_sort> container_sort_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

