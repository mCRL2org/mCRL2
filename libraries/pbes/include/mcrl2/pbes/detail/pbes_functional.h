// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

/// \brief Function object that determines if a term is equal to a given propositional variable instantiation.
struct compare_propositional_variable_instantiation: public std::unary_function<atermpp::aterm, bool>
{
  const propositional_variable_instantiation& v_;

  compare_propositional_variable_instantiation(const propositional_variable_instantiation& v)
   : v_(v)
  {}

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  template <typename Term>
  bool operator()(Term t) const
  {
    return v_ == t;
  }
};

/// \brief Function object that returns the name of the binding variable of a pbes equation
struct pbes_equation_variable_name: public std::unary_function<data::variable, core::identifier_string>
{
  /// \brief Function call operator
  /// \param e A PBES equation
  /// \return The function result
  core::identifier_string operator()(const pbes_equation& e) const
  {
    return e.variable().name();
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_FUNCTIONAL_H
