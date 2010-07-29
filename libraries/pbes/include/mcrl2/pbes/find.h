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
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/pbes_free_variable_finder.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container, typename OutputIterator >
void find_free_variables(Container const& container, OutputIterator const& o)
{
  pbes_system::detail::pbes_free_variable_finder<std::insert_iterator<std::set<data::variable> > > finder(o);
  finder(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container>
std::set<data::variable> find_free_variables(Container const& container)
{
  std::set<data::variable> result;
  pbes_system::find_free_variables(container, std::inserter(result, result.end()));
  return result;
}

namespace detail {

  template <typename OutputIterator>
  struct find_propositional_variables_visitor: public pbes_expression_visitor<pbes_expression>
  {
    OutputIterator dest;
    
    find_propositional_variables_visitor(OutputIterator d)
      : dest(d)
    {}

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_propositional_variable(const pbes_expression& /* e */, const propositional_variable_instantiation& v)
    {
      *dest++ = v;
      return true;
    } 
  };

  template <typename OutputIterator>
  find_propositional_variables_visitor<OutputIterator> make_find_propositional_variables_visitor(OutputIterator dest)
  {
    return find_propositional_variables_visitor<OutputIterator>(dest);
  }

} // namespace detail


/// \brief Returns all propositional variable instantiations that occur in the pbes expression t
/// \param t A term
/// \return All propositional variable instantiations that occur in the pbes expression t
inline
std::set<propositional_variable_instantiation> find_all_propositional_variable_instantiations(const pbes_expression& t)
{
  std::set<propositional_variable_instantiation> variables;
  detail::make_find_propositional_variables_visitor(std::inserter(variables, variables.end())).visit(t);
  return variables;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_H
