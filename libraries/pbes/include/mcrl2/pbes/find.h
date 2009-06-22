// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find.h
/// \brief Search functions of the pbes library.

#ifndef MCRL2_PBES_FIND_H
#define MCRL2_PBES_FIND_H

#include <set>
#include <iterator>
#include <functional>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/pbes_functional.h"
#include "mcrl2/pbes/detail/global_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Returns true if the term has a given variable as subterm.
/// \param t A term
/// \param v A propositional variable instantiation
/// \return True if the term has a given variable as subterm.
template <typename Term>
bool find_propositional_variable(Term t, const propositional_variable_instantiation& v)
{
  return atermpp::partial_find_if(t,
                                  detail::compare_propositional_variable_instantiation(v),
                                  is_propositional_variable_instantiation
                                 ) != atermpp::aterm();
}

/// \brief Returns all propositional variable instantiations that occur in the term t
/// \param t A term
/// \return All propositional variable instantiations that occur in the term t
template <typename Term>
std::set<propositional_variable_instantiation> find_all_propositional_variable_instantiations(Term t)
{
  std::set<propositional_variable_instantiation> variables;
  atermpp::find_all_if(t, is_propositional_variable_instantiation, std::inserter(variables, variables.end()));
  return variables;
/*
  std::set<propositional_variable_instantiation> variables;
  atermpp::partial_find_all_if(t,
                               is_propositional_variable_instantiation,
                               boost::bind(std::logical_or<bool>(), boost::bind(data::is_data_expression, _1), boost::bind(is_propositional_variable_instantiation, _1)),
                               std::inserter(variables, variables.end())
                              );
*/
  return variables;
}

/// \brief Finds free data variables in a pbes expression.
/// \param e A PBES expression
/// \return The free data variables occurring in the expression.
inline
std::set<data::variable> find_free_variables(const pbes_expression& e)
{
  detail::global_variable_visitor<pbes_expression> visitor;
  visitor.visit(e);
  return visitor.result;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_H
