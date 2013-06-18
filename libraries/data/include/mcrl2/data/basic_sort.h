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
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2
{

namespace data
{

//--- start generated class basic_sort ---//
/// \brief A basic sort
class basic_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    basic_sort()
      : sort_expression(core::detail::constructSortId())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit basic_sort(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortId(*this));
    }

    /// \brief Constructor.
    basic_sort(const core::identifier_string& name)
      : sort_expression(core::detail::gsMakeSortId(name))
    {}

    /// \brief Constructor.
    basic_sort(const std::string& name)
      : sort_expression(core::detail::gsMakeSortId(core::identifier_string(name)))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }
};
//--- end generated class basic_sort ---//

/// \brief list of basic sorts
typedef atermpp::term_list<basic_sort> basic_sort_list;
/// \brief vector of basic sorts
typedef std::vector<basic_sort> basic_sort_vector;

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::basic_sort& t1, mcrl2::data::basic_sort& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_SORT_EXPRESSION_H

