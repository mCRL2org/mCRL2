// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/multiple_possible_sorts.h
/// \brief The class multiple_possible_sorts.

#ifndef MCRL2_DATA_MULTIPLE_POSSIBLE_SORTS_H
#define MCRL2_DATA_MULTIPLE_POSSIBLE_SORTS_H

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2
{

namespace data
{

//--- start generated class multiple_possible_sorts ---//
/// \brief Multiple possible sorts
class multiple_possible_sorts: public sort_expression
{
  public:
    /// \brief Default constructor.
    multiple_possible_sorts()
      : sort_expression(core::detail::constructSortsPossible())
    {}

    /// \brief Constructor.
    /// \param term A term
    multiple_possible_sorts(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortsPossible(*this));
    }

    /// \brief Constructor.
    multiple_possible_sorts(const sort_expression_list& sorts)
      : sort_expression(core::detail::gsMakeSortsPossible(sorts))
    {}

    /// \brief Constructor.
    template <typename Container>
    multiple_possible_sorts(const Container& sorts, typename atermpp::detail::enable_if_container<Container, sort_expression>::type* = 0)
      : sort_expression(core::detail::gsMakeSortsPossible(sort_expression_list(sorts.begin(), sorts.end())))
    {}

    const sort_expression_list& sorts() const
    {
      return atermpp::aterm_cast<const sort_expression_list>(atermpp::list_arg1(*this));
    }
};
//--- end generated class multiple_possible_sorts ---//

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::multiple_possible_sorts& t1, mcrl2::data::multiple_possible_sorts& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_MULTIPLE_POSSIBLE_SORTS_H
