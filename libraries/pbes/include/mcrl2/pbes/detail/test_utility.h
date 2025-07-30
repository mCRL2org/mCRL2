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

#include "mcrl2/pbes/pbes.h"





namespace mcrl2::pbes_system::detail
{

/// \brief Returns a data variable of type Nat with a given name
/// \param name A string
/// \return A data variable of type Nat with a given name
inline data::variable nat(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

/// \brief Returns a data variable of type Pos with a given name
/// \param name A string
/// \return A data variable of type Pos with a given name
inline data::variable pos(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

/// \brief Returns a data variable of type Bool with a given name
/// \param name A string
/// \return A data variable of type Bool with a given name
inline data::variable bool_(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

/// \brief Returns a propositional variable declaration with the given name and parameters
/// \param name A string
/// \param parameters A sequence of data variables
/// \return A propositional variable declaration with the given name and parameters
inline propositional_variable propvar(const std::string& name, const data::variable_list& parameters)
{
  return propositional_variable(core::identifier_string(name), parameters);
}

/// \brief Returns a propositional variable instantiation with the given name and parameters
/// \param name A string
/// \param parameters A sequence of data expressions
/// \return A propositional variable instantiation with the given name and parameters
inline propositional_variable_instantiation propvarinst(const std::string& name,
    const data::data_expression_list& parameters)
{
  return propositional_variable_instantiation(core::identifier_string(name), parameters);
}

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_DETAIL_TEST_UTILITY_H
