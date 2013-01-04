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

namespace mcrl2
{

namespace data
{

//--- start generated class container_sort ---//
/// \brief A container sort
class container_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    container_sort()
      : sort_expression(core::detail::constructSortCons())
    {}

    /// \brief Constructor.
    /// \param term A term
    container_sort(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortCons(m_term));
    }

    /// \brief Constructor.
    container_sort(const container_type& container_name, const sort_expression& element_sort)
      : sort_expression(core::detail::gsMakeSortCons(container_name, element_sort))
    {}

    const container_type& container_name() const
    {
      return atermpp::aterm_cast<const container_type>(atermpp::arg1(*this));
    }

    const sort_expression& element_sort() const
    {
      return atermpp::aterm_cast<const sort_expression>(atermpp::arg2(*this));
    }
};
//--- end generated class container_sort ---//

/// \brief list of function sorts
///
typedef atermpp::term_list<container_sort> container_sort_list;

/// \brief list of function sorts
///
typedef std::vector<container_sort> container_sort_vector;

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_SORT_H

