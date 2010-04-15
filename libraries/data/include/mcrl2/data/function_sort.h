// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_sort.h
/// \brief The class function_sort.

#ifndef MCRL2_DATA_FUNCTION_SORT_H
#define MCRL2_DATA_FUNCTION_SORT_H

#include <iostream>
#include <boost/range/iterator_range.hpp>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2 {

  namespace data {

//--- start generated class function_sort ---//
/// \brief A function sort
class function_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    function_sort()
      : sort_expression(core::detail::constructSortArrow())
    {}

    /// \brief Constructor.
    /// \param term A term
    function_sort(atermpp::aterm_appl term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// \brief Constructor.
    function_sort(const sort_expression_list& domain, const sort_expression& codomain)
      : sort_expression(core::detail::gsMakeSortArrow(domain, codomain))
    {}

    /// \brief Constructor.
    template <typename Container>
    function_sort(const Container& domain, const sort_expression& codomain, typename atermpp::detail::enable_if_container<Container, sort_expression>::type* = 0)
      : sort_expression(core::detail::gsMakeSortArrow(atermpp::convert<sort_expression_list>(domain), codomain))
    {}

    sort_expression_list domain() const
    {
      return atermpp::list_arg1(*this);
    }

    sort_expression codomain() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated class function_sort ---//

    /// \brief list of function sorts
    typedef atermpp::term_list<function_sort> function_sort_list;
    /// \brief vector of function sorts
    typedef atermpp::vector<function_sort> function_sort_vector;

    /// \brief Convenience constructor for function sort with domain size 1
    ///
    /// \param[in] dom1 The first sort of the domain.
    /// \param[in] codomain The codomain of the sort.
    /// \post *this represents dom1 -> codomain
    inline function_sort make_function_sort(const sort_expression& dom1,
                  const sort_expression& codomain)
    {
      return function_sort(atermpp::make_list(dom1), codomain);
    }

    /// \brief Convenience constructor for function sort with domain size 2
    ///
    /// \param[in] dom1 The first sort of the domain.
    /// \param[in] dom2 The second sort of the domain.
    /// \param[in] codomain The codomain of the sort.
    /// \post *this represents dom1 # dom2 -> codomain
    inline function_sort make_function_sort(const sort_expression& dom1,
                  const sort_expression& dom2,
                  const sort_expression& codomain)
    {
      return function_sort(atermpp::make_list(dom1, dom2), codomain);
    }

    /// \brief Convenience constructor for function sort with domain size 3
    ///
    /// \param[in] dom1 The first sort of the domain.
    /// \param[in] dom2 The second sort of the domain.
    /// \param[in] dom3 The third sort of the domain.
    /// \param[in] codomain The codomain of the sort.
    /// \post *this represents dom1 # dom2 # dom3 -> codomain
    inline function_sort make_function_sort(const sort_expression& dom1,
                  const sort_expression& dom2,
                  const sort_expression& dom3,
                  const sort_expression& codomain)
    {
      return function_sort(atermpp::make_list(dom1, dom2, dom3), codomain);
    }

    /// \brief Convenience constructor for function sort with domain size 3
    ///
    /// \param[in] dom1 The first sort of the domain.
    /// \param[in] dom2 The second sort of the domain.
    /// \param[in] dom3 The third sort of the domain.
    /// \param[in] dom4 The fourth sort of the domain.
    /// \param[in] codomain The codomain of the sort.
    /// \post *this represents dom1 # dom2 # dom3 # dom4 -> codomain
    inline function_sort make_function_sort(const sort_expression& dom1,
                  const sort_expression& dom2,
                  const sort_expression& dom3,
                  const sort_expression& dom4,
                  const sort_expression& codomain)
    {
      return function_sort(atermpp::make_list(dom1, dom2, dom3, dom4), codomain);
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FUNCTION_SORT_H

