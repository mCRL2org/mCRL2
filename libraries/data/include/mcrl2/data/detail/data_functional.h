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
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"

namespace mcrl2 {

namespace data {

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
struct data_variable_name: public std::unary_function<data_variable, core::identifier_string>
{
  core::identifier_string operator()(const data_variable& v) const
  {
    return v.name();
  }
};

/// \brief Function object that returns the sort of a data variable
struct data_variable_sort: public std::unary_function<data_variable, sort_expression>
{
  sort_expression operator()(const data_variable& v) const
  {
    return v.sort();
  }
};

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
