// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file test_utility.h
/// \brief Utility functions for testing.

#ifndef MCRL2_PBES_DETAIL_TEST_UTILITY_H
#define MCRL2_PBES_DETAIL_TEST_UTILITY_H

#include <string>
#include "mcrl2/old_data/data.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/propositional_variable.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  old_data::data_variable nat(std::string name)
  {
    return old_data::data_variable(core::identifier_string(name), old_data::sort_expr::nat());
  }

  old_data::data_variable pos(std::string name)
  {
    return old_data::data_variable(core::identifier_string(name), old_data::sort_expr::pos());
  } 
  
  old_data::data_variable bool_(std::string name)
  {
    return old_data::data_variable(core::identifier_string(name), old_data::sort_expr::bool_());
  }
  
  propositional_variable propvar(std::string name, old_data::data_variable_list parameters)
  {
    return propositional_variable(core::identifier_string(name), parameters);
  }

  propositional_variable_instantiation propvarinst(std::string name, old_data::data_expression_list parameters)
  {
    return propositional_variable_instantiation(core::identifier_string(name), parameters);
  }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_TEST_UTILITY_H
