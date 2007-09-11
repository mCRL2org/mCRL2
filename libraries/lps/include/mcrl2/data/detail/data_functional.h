// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
#define MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "boost/format.hpp"
#include "mcrl2/data/data.h"
#include "mcrl2/lps/detail/data_utility.h"
#include "atermpp/algorithm.h"
#include "atermpp/aterm.h"
#include "atermpp/set.h"
#include "atermpp/utility.h"

namespace lps {

namespace detail {

using atermpp::aterm;
using atermpp::aterm_traits;

/// Function object that returns true if the expressions x and y have the same sort.
struct equal_data_expression_sort: public std::binary_function<data_expression, data_expression, bool>
{
  bool operator()(const data_expression& x, const data_expression& y) const
  {
    return x.sort() == y.sort();
  }
};

/// \brief Function object that returns the name of a data variable
struct data_variable_name: public std::unary_function<data_variable, identifier_string>
{
  identifier_string operator()(const data_variable& v) const
  {
    return v.name();
  }
};

/// \brief Function object that returns the sort of a data variable
struct data_variable_sort: public std::unary_function<data_variable, lps::sort>
{
  lps::sort operator()(const data_variable& v) const
  {
    return v.sort();
  }
};

} // namespace detail

} // namespace lps

#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
