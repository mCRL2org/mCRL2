// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/basic_sort.h
/// \brief The class basic_sort.

#ifndef MCRL2_DATA_BASIC_SORT_H
#define MCRL2_DATA_BASIC_SORT_H

#include <string>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

  namespace data {

    /// \brief basic sort.
    ///
    /// A basic sort is sort with only a name.
    /// An example is the sort S, specified in mCRL2 with sort S;
    class basic_sort: public sort_expression
    {
      public:

        /// \brief Default constructor. Note that this does not entail a
        ///        valid sort expression.
        ///
        basic_sort()
          : sort_expression()
        {}

        /// \brief Construct a basic sort from a sort expression.
        ///
        /// \param[in] s A sort expression.
        /// \pre s has the internal structure of a basic sort.
        basic_sort(const sort_expression& s)
          : sort_expression(s)
        {
          assert(s.is_basic_sort());
        }

        /// \brief Construct a basic sort from a string.
        ///
        /// \param[in] s The name of the sort that is created.
        /// \post This is a sort with name s.
        basic_sort(const std::string& s)
          : sort_expression(mcrl2::core::detail::gsMakeSortId(core::identifier_string(s)))
        {}

        /// \brief Construct a basic sort from an identifier string.
        ///
        /// \param[in] s The name of the sort that is created.
        /// \post This is a sort with name s.
        basic_sort(const core::identifier_string& s)
          : sort_expression(mcrl2::core::detail::gsMakeSortId(s))
        {}

        /// \brief Returns the name of this sort.
        core::identifier_string name() const
        {
          return atermpp::aterm_string(atermpp::arg1(*this));
        }

    }; // class sort_expression

    /// \brief list of basic sorts
    ///
    typedef atermpp::vector<basic_sort> basic_sort_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

