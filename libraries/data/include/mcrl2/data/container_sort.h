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
      protected:

        /// \brief Converts a string to an internally used type.
        ///
        /// \param[in] s The string to be converted. May only be any of "List",
        ///            "Set" or "Bag".
        /// \return The internally used type corresponding to s.
        inline
        atermpp::aterm_appl string_to_sort_cons_type(const std::string& s) const
        {
          atermpp::aterm_appl result;

          if (s == "list")
          {
            result = core::detail::gsMakeSortList();
          }
          else if (s == "set_")
          {
            result = core::detail::gsMakeSortSet();
          }
          else if (s == "fset")
          {
            result = core::detail::gsMakeSortFSet();
          }
          else if (s == "bag")
          {
            result = core::detail::gsMakeSortBag();
          }
          else if (s == "fbag")
          {
            result = core::detail::gsMakeSortFBag();
          }
          else
          {
            std::cerr << "Incorrect sort_cons_type: " << s << std::endl;
            assert(false);
          }

          return result;
        }

        /// \brief Converts an internally used type to a string.
        ///
        /// \param[in] s The internally used type to be converted.
        /// \return The string corresponding to s.
        inline
        std::string sort_cons_type_to_string(const atermpp::aterm_appl& s) const
        {
          std::string result;

          if (core::detail::gsIsSortList(s))
          {
            result = "list";
          }
          else if (core::detail::gsIsSortSet(s))
          {
            result = "set_";
          }
          else if (core::detail::gsIsSortFSet(s))
          {
            result = "fset";
          }
          else if (core::detail::gsIsSortBag(s))
          {
            result = "bag";
          }
          else if (core::detail::gsIsSortFBag(s))
          {
            result = "fbag";
          }
          else
          {
            assert(false);
          }

          return result;
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
        container_sort(const std::string& container_name,
                       const sort_expression& element_sort)
          : sort_expression(core::detail::gsMakeSortCons(string_to_sort_cons_type(container_name), element_sort))
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
        std::string container_name() const
        {
          return sort_cons_type_to_string(atermpp::arg1(*this));
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
          return (container_name() == "List");
        }

        /// \brief Returns true iff container name is Set.
        ///
        inline
        bool is_set_sort() const
        {
          return (container_name() == "Set");
        }

        /// \brief Returns true iff container name is Bag.
        ///
        inline
        bool is_bag_sort() const
        {
          return (container_name() == "Bag");
        }

    }; // class container_sort

    /// \brief list of function sorts
    ///
    typedef atermpp::term_list<container_sort> container_sort_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

