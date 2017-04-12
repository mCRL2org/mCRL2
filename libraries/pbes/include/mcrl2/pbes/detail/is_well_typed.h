// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/is_well_typed.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_IS_WELL_TYPED_H
#define MCRL2_PBES_DETAIL_IS_WELL_TYPED_H

#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/data/detail/equal_sorts.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor for collecting the quantifier variables that occur in a pbes expression.
struct find_quantifier_variables_traverser: public pbes_expression_traverser<find_quantifier_variables_traverser>
{
  typedef pbes_expression_traverser<find_quantifier_variables_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<data::variable> result;

  void enter(const forall& x)
  {
    auto const& v = x.variables();
    result.insert(v.begin(), v.end());
  }

  void enter(const exists& x)
  {
    auto const& v = x.variables();
    result.insert(v.begin(), v.end());
  }
};

inline
std::set<data::variable> find_quantifier_variables(const pbes_expression& x)
{
  find_quantifier_variables_traverser f;
  f.apply(x);
  return f.result;
}

/// \brief Visitor for determining if within the scope of a quantifier there are quantifier
/// variables of free variables with the same name.
struct has_quantifier_name_clashes_traverser: public pbes_expression_traverser<has_quantifier_name_clashes_traverser>
{
  typedef pbes_expression_traverser<has_quantifier_name_clashes_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::vector<data::variable_list> quantifier_stack;
  bool result;
  data::variable name_clash; // if result is true, then this attribute contains the conflicting variable

  has_quantifier_name_clashes_traverser()
    : result(false)
  {}

  /// \brief Returns true if the quantifier_stack contains a data variable with the given name
  /// \param name A
  /// \return True if the quantifier_stack contains a data variable with the given name
  bool is_in_quantifier_stack(const core::identifier_string& name) const
  {
    for (const data::variable_list& vars: quantifier_stack)
    {
      if (std::find(boost::make_transform_iterator(vars.begin(), data::detail::variable_name()),
                    boost::make_transform_iterator(vars.end()  , data::detail::variable_name()),
                    name
                   ) != boost::make_transform_iterator(vars.end()  , data::detail::variable_name())
         )
      {
        return true;
      }
    }
    return false;
  }

  /// \brief Adds variables to the quantifier stack, and adds replacements for the name clashes to replacements.
  /// \param variables A sequence of data variables
  /// \return The number of replacements that were added.
  void push(const data::variable_list& variables)
  {
    if (result)
    {
      return;
    }
    for (const data::variable& v: variables)
    {
      if (is_in_quantifier_stack(v.name()))
      {
        result = true;
        name_clash = v;
        return;
      }
    }
    quantifier_stack.push_back(variables);
  }

  /// \brief Pops the quantifier stack
  void pop()
  {
    if (result)
    {
      return;
    }
    quantifier_stack.pop_back();
  }

  void enter(const forall& x)
  {
    push(x.variables());
  }

  void leave(const forall&)
  {
    pop();
  }

  void enter(const exists& x)
  {
    push(x.variables());
  }

  void leave(const exists&)
  {
    pop();
  }
};

inline
bool has_quantifier_name_clashes(const pbes_expression& x)
{
  has_quantifier_name_clashes_traverser f;
  f.apply(x);
  return f.result;
}

/// \brief Checks if the equation is well typed
/// \return True if
/// <ul>
/// <li>the binding variable parameters have unique names</li>
/// <li>the names of the quantifier variables in the equation are disjoint with the binding variable parameter names</li>
/// <li>within the scope of a quantifier variable in the formula, no other quantifier variables with the same name may occur</li>
/// </ul>
inline
bool is_well_typed(const pbes_equation& eqn)
{
  // check 1)
  if (data::detail::sequence_contains_duplicates(
        boost::make_transform_iterator(eqn.variable().parameters().begin(), data::detail::variable_name()),
        boost::make_transform_iterator(eqn.variable().parameters().end()  , data::detail::variable_name())
      )
     )
  {
    mCRL2log(log::error) << "pbes_equation::is_well_typed() failed: the names of the binding variable parameters are not unique" << std::endl;
    return false;
  }

  // check 2)
  std::set<data::variable> qvariables = detail::find_quantifier_variables(eqn.formula());
  if (data::detail::sequences_do_overlap(
        boost::make_transform_iterator(eqn.variable().parameters().begin(), data::detail::variable_name()),
        boost::make_transform_iterator(eqn.variable().parameters().end()  , data::detail::variable_name()),
        boost::make_transform_iterator(qvariables.begin()     , data::detail::variable_name()),
        boost::make_transform_iterator(qvariables.end()       , data::detail::variable_name())
      )
     )
  {
    mCRL2log(log::error) << "pbes_equation::is_well_typed() failed: the names of the quantifier variables and the names of the binding variable parameters are not disjoint in expression " << pbes_system::pp(eqn.formula()) << std::endl;
    return false;
  }

  // check 3)
  has_quantifier_name_clashes_traverser nvisitor;
  nvisitor.apply(eqn.formula());
  if (nvisitor.result)
  {
    mCRL2log(log::error) << "pbes_equation::is_well_typed() failed: the quantifier variable " << nvisitor.name_clash << " occurs within the scope of a quantifier variable with the same name." << std::endl;
    return false;
  }

  return true;
}

/// \brief Checks if the propositional variable instantiation v has a conflict with the
/// sequence of propositional variable declarations [first, last).
/// \param first Start of a sequence of propositional variable declarations
/// \param last End of a sequence of propositional variable declarations
/// \return True if a conflict has been detected
/// \param v A propositional variable instantiation
template <typename Iter>
bool has_conflicting_type(Iter first, Iter last, const propositional_variable_instantiation& v, const data::data_specification& data_spec)
{
  for (Iter i = first; i != last; ++i)
  {
    if (i->name() == v.name() && !data::detail::equal_sorts(i->parameters(), v.parameters(), data_spec))
    {
      return true;
    }
  }
  return false;
}

inline
bool is_well_typed_equation(const pbes_equation& eqn,
                            const std::set<data::sort_expression>& declared_sorts,
                            const std::set<data::variable>& declared_global_variables,
                            const data::data_specification& data_spec
                           )
{
  // check 2)
  const data::variable_list& variables = eqn.variable().parameters();
  if (!data::detail::check_sorts(
        boost::make_transform_iterator(variables.begin(), data::detail::sort_of_variable()),
        boost::make_transform_iterator(variables.end()  , data::detail::sort_of_variable()),
        declared_sorts
      )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: some of the sorts of the binding variable "
              << eqn.variable()
              << " are not declared in the data specification "
              << data_spec
              << std::endl;
    return false;
  }

  // check 3)
  std::set<data::variable> quantifier_variables = detail::find_quantifier_variables(eqn.formula());
  if (!data::detail::check_sorts(
        boost::make_transform_iterator(quantifier_variables.begin(), data::detail::sort_of_variable()),
        boost::make_transform_iterator(quantifier_variables.end()  , data::detail::sort_of_variable()),
        declared_sorts
      )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: some of the sorts of the quantifier variables "
              << data::pp(quantifier_variables)
              << " are not declared in the data specification "
              << data_spec
              << std::endl;
    return false;
  }

  // check 7)
  if (!utilities::detail::set_intersection(declared_global_variables, quantifier_variables).empty())
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: the declared free variables and the quantifier variables have collisions" << std::endl;
    return false;
  }
  return true;
}

inline
bool is_well_typed_pbes(const std::set<data::sort_expression>& declared_sorts,
                        const std::set<data::variable>& declared_global_variables,
                        const std::set<data::variable>& occurring_global_variables,
                        const std::set<propositional_variable>& declared_variables,
                        const std::set<propositional_variable_instantiation>& occ,
                        const propositional_variable_instantiation& init,
                        const data::data_specification& data_spec
                       )
{
  // check 1)
  if (!data::detail::check_sorts(
        boost::make_transform_iterator(declared_global_variables.begin(), data::detail::sort_of_variable()),
        boost::make_transform_iterator(declared_global_variables.end()  , data::detail::sort_of_variable()),
        declared_sorts
      )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: some of the sorts of the free variables "
              << data::pp(declared_global_variables)
              << " are not declared in the data specification "
              << data_spec
              << std::endl;
    return false;
  }

  // check 4)
  auto propvar_name = [](const propositional_variable& x) { return x.name(); };
  if (data::detail::sequence_contains_duplicates(
        boost::make_transform_iterator(declared_variables.begin(), propvar_name),
        boost::make_transform_iterator(declared_variables.end()  , propvar_name)
      )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: the names of the binding variables are not unique" << std::endl;
    return false;
  }

  // check 5)
  if (!std::includes(declared_global_variables.begin(),
                     declared_global_variables.end(),
                     occurring_global_variables.begin(),
                     occurring_global_variables.end()
                    )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: not all of the free variables are declared\n"
              << "free variables: " << data::pp(occurring_global_variables) << "\n"
              << "declared free variables: " << data::pp(declared_global_variables)
              << std::endl;
    return false;
  }

  // check 6)
  if (data::detail::sequence_contains_duplicates(
        boost::make_transform_iterator(occurring_global_variables.begin(), data::detail::variable_name()),
        boost::make_transform_iterator(occurring_global_variables.end()  , data::detail::variable_name())
      )
     )
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: the free variables have no unique names" << std::endl;
    return false;
  }

  // check 8)
  for (const propositional_variable_instantiation& v: occ)
  {
    if (has_conflicting_type(declared_variables.begin(), declared_variables.end(), v, data_spec))
    {
      mCRL2log(log::error) << "pbes::is_well_typed() failed: the occurring variable " << pbes_system::pp(v) << " conflicts with its declaration!" << std::endl;
      return false;
    }
  }

  // check 9)
  if (has_conflicting_type(declared_variables.begin(), declared_variables.end(), init, data_spec))
  {
    mCRL2log(log::error) << "pbes::is_well_typed() failed: the initial state " << pbes_system::pp(init) << " conflicts with its declaration!" << std::endl;
    return false;
  }
  return true;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_IS_WELL_TYPED_H
