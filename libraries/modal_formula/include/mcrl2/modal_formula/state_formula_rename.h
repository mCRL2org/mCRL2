// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_rename.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_STATE_VARIABLE_RENAME_H
#define MCRL2_MODAL_STATE_VARIABLE_RENAME_H

#include <deque>
#include "mcrl2/data/replace.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/utilities/number_postfix_generator.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace state_formulas
{

/// Visitor that renames predicate variables using the specified identifier generator.
/// \post In the generated formula, all predicate variables have different names.
template <typename IdentifierGenerator>
struct state_formula_predicate_variable_rename_builder: public state_formulas::state_formula_builder<state_formula_predicate_variable_rename_builder<IdentifierGenerator> >
{
  typedef state_formulas::state_formula_builder<state_formula_predicate_variable_rename_builder<IdentifierGenerator> > super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  /// \brief An identifier generator
  IdentifierGenerator& generator;

  /// \brief A stack of replacements. It may contain pairs with identical values.
  std::deque<std::pair<core::identifier_string, core::identifier_string> > replacements;

  /// \brief Constructor
  /// \param generator A generator for fresh identifiers
  state_formula_predicate_variable_rename_builder(IdentifierGenerator& generator)
    : generator(generator)
  {}

  /// \brief Generates a new name for n, and adds a replacement to the replacement stack.
  /// \param n A
  /// \return The new name.
  core::identifier_string push(const core::identifier_string& n)
  {
    core::identifier_string new_name = generator(n);
    replacements.push_front(std::make_pair(n, new_name));
    return new_name;
  }

  /// \brief Removes the last added replacement.
  void pop()
  {
    replacements.pop_front();
  }

  /// \brief Visit var node
  /// \param e A modal formula
  /// \param n A
  /// \param l A sequence of data expressions
  /// \return The result of visiting the node
  state_formula apply(const variable& x)
  {
    core::identifier_string new_name = x.name();
    for (std::deque<std::pair<core::identifier_string, core::identifier_string> >::iterator i = replacements.begin(); i != replacements.end(); ++i)
    {
      if (i->first == x.name())
      {
        new_name = i->second;
        break;
      }
    }
    return variable(new_name, x.arguments());
  }

  /// \brief Visit mu node
  /// \param e A modal formula
  /// \param n A
  /// \param a A sequence of assignments to data variables
  /// \param f A modal formula
  /// \return The result of visiting the node
  state_formula apply(const mu& x)
  {
    core::identifier_string new_name = push(x.name());
    state_formula new_formula = apply(x.operand());
    pop();
    return mu(new_name, x.assignments(), new_formula);
  }

  /// \brief Visit nu node
  /// \param e A modal formula
  /// \param n A
  /// \param a A sequence of assignments to data variables
  /// \param f A modal formula
  /// \return The result of visiting the node
  state_formula apply(const nu& x)
  {
    core::identifier_string new_name = push(x.name());
    state_formula new_formula = apply(x.operand());
    pop();
    return nu(new_name, x.assignments(), new_formula);
  }
};

/// \brief Utility function for creating a state_formula_predicate_variable_rename_builder.
/// \param generator A generator for fresh identifiers
/// \return a state_formula_predicate_variable_rename_builder
template <typename IdentifierGenerator>
state_formula_predicate_variable_rename_builder<IdentifierGenerator> make_state_formula_predicate_variable_rename_builder(IdentifierGenerator& generator)
{
  return state_formula_predicate_variable_rename_builder<IdentifierGenerator>(generator);
}

/// \brief Renames predicate variables of the formula f using the specified identifier generator.
/// \post predicate variables within the same scope have different names
/// \param f A modal formula
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
state_formula rename_predicate_variables(const state_formula& f, IdentifierGenerator& generator)
{
  return make_state_formula_predicate_variable_rename_builder(generator).apply(f);
}

/// Visitor that renames variables using the specified identifier generator. Also bound variables are renamed!
struct state_formula_variable_rename_builder: public state_formulas::sort_expression_builder<state_formula_variable_rename_builder>
{
  typedef state_formulas::sort_expression_builder<state_formula_variable_rename_builder> super;

  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  /// \brief The set of identifiers that may not be used as a variable name.
  const std::set<core::identifier_string>& forbidden_identifiers;

  std::map<core::identifier_string, core::identifier_string> generated_identifiers;

  utilities::number_postfix_generator generator;

  core::identifier_string create_name(const core::identifier_string& x)
  {
    std::map<core::identifier_string, core::identifier_string>::iterator i = generated_identifiers.find(x);
    if (i != generated_identifiers.end())
    {
      return i->second;
    }
    std::string name = generator(std::string(x));
    generated_identifiers[x] = core::identifier_string(name);
    return core::identifier_string(name);
  }

  /// \brief Constructor
  state_formula_variable_rename_builder(const std::set<core::identifier_string>& forbidden_identifiers_)
    : forbidden_identifiers(forbidden_identifiers_)
  {
    for (const core::identifier_string& id: forbidden_identifiers)
    {
      generator.add_identifier(std::string(id));
    }
  }

  // do not traverse sorts
  data::sort_expression apply(const data::sort_expression& x)
  {
    return x;
  }

  data::variable apply(const data::variable& x)
  {
    using utilities::detail::contains;
    if (!contains(forbidden_identifiers, x.name()))
    {
      return x;
    }
    return data::variable(create_name(x.name()), x.sort());
  }
};

/// \brief Renames all data variables in the formula f such that the forbidden identifiers are not used
/// \param f A modal formula
/// \return The rename result
inline
state_formula rename_variables(const state_formula& f, const std::set<core::identifier_string>& forbidden_identifiers)
{
  return state_formula_variable_rename_builder(forbidden_identifiers).apply(f);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_VARIABLE_RENAME_H
