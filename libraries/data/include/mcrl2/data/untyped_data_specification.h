// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/untyped_data_specification.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNTYPED_DATA_SPECIFICATION_H
#define MCRL2_DATA_UNTYPED_DATA_SPECIFICATION_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2::data {

struct untyped_data_specification
{
  std::vector<basic_sort> basic_sorts;
  std::vector<alias> aliases;
  std::vector<function_symbol> constructors;
  std::vector<function_symbol> mappings;
  std::vector<data_equation> equations;

  void add_sort(const basic_sort& x) { basic_sorts.push_back(x); }
  void add_alias(const alias& x) { aliases.push_back(x); }
  void add_constructor(const function_symbol& x) { constructors.push_back(x); }
  void add_mapping(const function_symbol& x) { mappings.push_back(x); }
  void add_equation(const data_equation& x) { equations.push_back(x); }

  data_specification construct_data_specification() const
  {
    data_specification dataspec;
    for (const basic_sort& x: basic_sorts)
    {
      dataspec.add_sort(x);
    }
    for (const alias& x: aliases)
    {
      dataspec.add_alias(x);
    }
    for (const function_symbol& x: constructors)
    {
      dataspec.add_constructor(x);
    }
    for (const function_symbol& x: mappings)
    {
      dataspec.add_mapping(x);
    }
    for (const data_equation& x: equations)
    {
      dataspec.add_equation(x);
    }
    return dataspec;
  }
};

} // namespace mcrl2::data



#endif // MCRL2_DATA_UNTYPED_DATA_SPECIFICATION_H
