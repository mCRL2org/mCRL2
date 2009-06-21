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

namespace mcrl2 {

  namespace data {

    /// \brief container sort.
    ///
    /// Container sorts are sorts with a name and an element sort.
    class container_sort: public sort_expression
    {
      public:

        enum type {
          list = 0,
          set_ = 1,
          fset = 2,
          bag  = 3,
          fbag = 4
        };

      private:

        inline
        static atermpp::vector< atermpp::aterm_appl > const& initialise_types()
        {
          using namespace core::detail;

          static atermpp::vector< atermpp::aterm_appl > types(5);

          types[container_sort::list] = gsMakeSortList();
          types[container_sort::set_] = gsMakeSortSet();
          types[container_sort::fset] = gsMakeSortFSet();
          types[container_sort::bag]  = gsMakeSortBag();
          types[container_sort::fbag] = gsMakeSortFBag();

          return types;
        }

      protected:

        /// \brief Converts a string to an internally used type.
        ///
        /// \param[in] s The string to be converted. May only be any of "List",
        ///            "Set" or "Bag".
        /// \return The internally used type corresponding to s.
        inline
        container_sort::type container_type(const atermpp::aterm_appl& s) const
        {
          static atermpp::vector< atermpp::aterm_appl > const& types = initialise_types();

          if (s == types[container_sort::list])
          {
            return container_sort::list;
          }
          else if (s == types[container_sort::set_])
          {
            return container_sort::set_;
          }
          else if (s == types[container_sort::fset])
          {
            return container_sort::fset;
          }
          else if (s == types[container_sort::bag])
          {
            return container_sort::bag;
          }
          else if (s == types[container_sort::fbag])
          {
            return container_sort::fbag;
          }

          assert(false);

          return container_sort::list;
        }

        /// \brief Transforms a container type operation to the term representation
        inline
        atermpp::aterm_appl const& container_type_as_term(const container_sort::type& s) const
        {
          static atermpp::vector< atermpp::aterm_appl > const& types = initialise_types();

          return types[s];
        }

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
        container_sort(const container_sort::type& container_name,
                       const sort_expression& element_sort)
          : sort_expression(core::detail::gsMakeSortCons(container_type_as_term(container_name), element_sort))
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
        container_sort::type container_type() const
        {
          return container_type(atermpp::arg1(*this));
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
          return (container_type() == container_sort::list);
        }

        /// \brief Returns true iff container name is Set.
        ///
        inline
        bool is_set_sort() const
        {
          return (container_type() == container_sort::set_);
        }

        /// \brief Returns true iff container name is Bag.
        ///
        inline
        bool is_bag_sort() const
        {
          return (container_type() == container_sort::bag);
        }

    }; // class container_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::term_list<container_sort> container_sort_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

