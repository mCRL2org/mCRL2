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

#include <iostream>
#include <sstream>
#include <list>
#include <iterator>

#include "boost/utility/enable_if.hpp"

#include "mcrl2/core/print.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/find.h"

namespace atermpp {
  namespace detail {

    // This is here to make the std::list container work with the pp container overload.
    template < typename T >
    struct is_container_impl< std::list< T > > {
      typedef boost::true_type type;
    };

  } // namespace detail
} // namespace atermpp

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
    inline std::string pp(Container const& c, typename atermpp::detail::enable_if_container< Container >::type* = 0)
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

