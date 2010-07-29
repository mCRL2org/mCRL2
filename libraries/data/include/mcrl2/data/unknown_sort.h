// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/unknown_sort.h
/// \brief The class unknown_sort.

#ifndef MCRL2_DATA_UNKNOWN_SORT_H
#define MCRL2_DATA_UNKNOWN_SORT_H

#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

  namespace data {

//--- start generated class unknown_sort ---//
/// \brief Unknown sort expression
class unknown_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    unknown_sort()
      : sort_expression(core::detail::constructSortUnknown())
    {}

    /// \brief Constructor.
    /// \param term A term
    unknown_sort(atermpp::aterm_appl term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortUnknown(m_term));
    }
};
//--- end generated class unknown_sort ---//

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UNKNOWN_SORT_H

