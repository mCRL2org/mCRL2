// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_functional.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_FUNCTIONAL_H
#define MCRL2_PBES_DETAIL_PBES_FUNCTIONAL_H

#include <functional>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data.h"
#include "mcrl2/pbes/pbes_equation.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Function object that returns the name of the binding variable of a pbes equation
struct pbes_equation_variable_name: public std::unary_function<data::data_variable, core::identifier_string>
{
  core::identifier_string operator()(const pbes_equation& e) const
  {
    return e.variable().name();
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_FUNCTIONAL_H
