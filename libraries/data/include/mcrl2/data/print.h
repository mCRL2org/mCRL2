// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_DATA_PRINT_H
#define MCRL2_DATA_PRINT_H

#include "boost/utility/enable_if.hpp"

#include "mcrl2/core/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \brief Pretty prints a data specification
    /// \param[in] specification a data specification
    inline std::string pp(data_specification const& specification)
    {
      return core::pp(detail::data_specification_to_aterm_data_spec(specification));
    }

    /// \brief Pretty prints the contents of a container
    /// \param[in] c a container with data or sort expressions
    template < typename Container >
    inline std::string pp(Container const& c, typename boost::enable_if< typename detail::is_container< Container >::type >::type* = 0)
    {
      std::string result;

      if (c.begin() != c.end())
      {
        result.append(mcrl2::core::pp(*c.begin()));

        for (typename Container::const_iterator i = ++(c.begin()); i != c.end(); ++i)
        {
          result.append(", ").append(mcrl2::core::pp(*i));
        }
      }

      return result;
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression
    inline std::string pp(atermpp::aterm_appl const& c)
    {
      return core::pp(c);
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression.
    template < typename Expression >
    inline std::string pp(atermpp::term_list< Expression > const& c)
    {
      return core::pp(c);
    }


  } // namespace data

} // namespace mcrl2

#endif

