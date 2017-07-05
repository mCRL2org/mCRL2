// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/is_sub_sort.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_IS_SUB_SORT_H
#define MCRL2_DATA_IS_SUB_SORT_H

#include <algorithm>

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/standard_container_utility.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/untyped_possible_sorts.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

namespace detail {

inline
bool is_numeric_sort(const sort_expression& x)
{
  return sort_pos::is_pos(x) || sort_nat::is_nat(x) || sort_int::is_int(x) || sort_real::is_real(x);
}

inline
std::size_t numeric_sort_value(const sort_expression& x)
{
  if (sort_pos::is_pos(x)  ) { return 0; }
  if (sort_nat::is_nat(x)  ) { return 1; }
  if (sort_int::is_int(x)  ) { return 2; }
  if (sort_real::is_real(x)) { return 3; }
  throw mcrl2::runtime_error("numeric_sort_value: argument " + data::pp(x) + " is not a numeric sort!");
}

} // namespace detail

inline
bool is_sub_sort(const sort_expression& x1, const sort_expression& x2)
{
  if (x1 == x2)
  {
    return true;
  }
  if (detail::is_numeric_sort(x1) && detail::is_numeric_sort(x2))
  {
    return detail::numeric_sort_value(x1) <= detail::numeric_sort_value(x2);
  }
  if (is_container_sort(x1) && is_container_sort(x2))
  {
    const container_sort& s1 = atermpp::down_cast<container_sort>(x1);
    const container_sort& s2 = atermpp::down_cast<container_sort>(x2);
    if (s1.container_name() == s2.container_name())
    {
      return is_sub_sort(s1.element_sort(), s2.element_sort());
    }
    else
    {
      return false;
    }
  }
  if (is_function_sort(x1) && is_function_sort(x2))
  {
    const function_sort& s1 = atermpp::down_cast<function_sort>(x1);
    const function_sort& s2 = atermpp::down_cast<function_sort>(x2);
    if (s1.domain().size() != s2.domain().size())
    {
      return false;
    }
    if (!is_sub_sort(s2.codomain(), s1.codomain()))
    {
      return false;
    }
    auto const& s1_domain = s1.domain();
    auto const& s2_domain = s2.domain();
    return std::equal(s1_domain.begin(), s1_domain.end(), s2_domain.begin(), [&](const sort_expression& x, const sort_expression& y) { return is_sub_sort(x, y); });
  }
  throw mcrl2::runtime_error("is_sub_sort: not implemented yet!");
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_IS_SUB_SORT_H
