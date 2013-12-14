// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/untyped_sort.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNTYPED_SORT_H
#define MCRL2_DATA_UNTYPED_SORT_H

#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

namespace data {

//--- start generated class untyped_sort ---//
/// \brief Unknown sort expression
class untyped_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    untyped_sort()
      : sort_expression(core::detail::default_values::UntypedSortUnknown)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_sort(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_UntypedSortUnknown(*this));
    }
};

// prototype declaration
std::string pp(const untyped_sort& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_sort& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(untyped_sort& t1, untyped_sort& t2)
{
  t1.swap(t2);
}
//--- end generated class untyped_sort ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UNTYPED_SORT_H
