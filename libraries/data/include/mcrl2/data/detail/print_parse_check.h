// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/print_parse_check.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_PRINT_PARSE_CHECK_H
#define MCRL2_DATA_DETAIL_PRINT_PARSE_CHECK_H

#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

namespace detail {

inline
void print_parse_check(const sort_expression& x, const data_specification& dataspec = data_specification())
{
  std::string xtext = data::pp(x);
  sort_expression y = data::parse_sort_expression(xtext, dataspec);
  std::string ytext = data::pp(y);
  if (xtext != ytext)
  {
    std::cerr << "--- ERROR: print_parse_check failed! ---\n";
    std::cerr << "x = " << x << " " << xtext << "\n";
    std::cerr << "y = " << y << " " << ytext << "\n";
  }
}

inline
void print_parse_check(const data_expression& x, const data_specification& dataspec = data_specification())
{
  std::string xtext = data::pp(x);
  std::set<variable> v = data::find_free_variables(x);
  data_expression y = data::parse_data_expression(xtext, v, dataspec);
  std::string ytext = data::pp(y);
  if (xtext != ytext)
  {
    std::cerr << "--- ERROR: print_parse_check failed! ---\n";
    std::cerr << "x = " << x << " " << xtext << "\n";
    std::cerr << "y = " << y << " " << ytext << "\n";
  }
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_PRINT_PARSE_CHECK_H
