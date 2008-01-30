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
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/sort_identifier.h"
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

struct sort_has_name
{
  std::string m_name;
    
  sort_has_name(std::string name)
    : m_name(name)
  {}
  
  bool operator()(sort_expression s) const
  {
    return is_sort_identifier(s) && std::string(sort_identifier(s).name()) == m_name;
  }
};

struct data_operation_has_name
{
  std::string m_name;
    
  data_operation_has_name(std::string name)
    : m_name(name)
  {}
  
  bool operator()(data_operation c) const
  {
    return std::string(c.name()) == m_name;
  }
};

/// Find the mapping named s.
inline
data_operation find_mapping(data_specification data, std::string s)
{
  data_operation_list::iterator i = std::find_if(data.mappings().begin(), data.mappings().end(), data_operation_has_name(s));
  if (i == data.mappings().end())
  {
    return data_operation();
  }
  return *i;
}

/// Find the constructor named s.
inline
data_operation find_constructor(data_specification data, std::string s)
{
  data_operation_list::iterator i = std::find_if(data.constructors().begin(), data.constructors().end(), data_operation_has_name(s));
  if (i == data.constructors().end())
  {
    return data_operation();
  }
  return *i;
}

/// Find the sort named s.
inline
sort_expression find_sort(data_specification data, std::string s)
{
  sort_expression_list::iterator i = std::find_if(data.sorts().begin(), data.sorts().end(), sort_has_name(s));
  if (i == data.sorts().end())
  {
    return sort_expression();
  }
  return *i;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_FUNCTIONAL_H
