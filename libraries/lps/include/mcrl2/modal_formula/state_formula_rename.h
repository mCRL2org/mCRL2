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
#include "mcrl2/modal_formula/state_formula_builder.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitution.h"

namespace mcrl2 {

namespace modal_formula {

/// Visitor that renames predicate variables using the specified identifier generator.
/// \post In the generated formula, all predicate variables have different names.
template <typename IdentifierGenerator>
struct state_formula_predicate_variable_rename_builder: public state_formula_builder
{
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
    std::pair<core::identifier_string, core::identifier_string> p = replacements.front();
    replacements.pop_front();
  }

  /// \brief Visit var node
  /// \param e A modal formula
  /// \param n A
  /// \param l A sequence of data expressions
  /// \return The result of visiting the node
  state_formula visit_var(const state_formula& e, const core::identifier_string& n, const data::data_expression_list& l)
  {
    core::identifier_string new_name = n;
    for (std::deque<std::pair<core::identifier_string, core::identifier_string> >::iterator i = replacements.begin(); i != replacements.end(); ++i)
    {
      if (i->first == n)
      {
        new_name = i->second;
        break;
      }
    }
    return state_frm::var(new_name, l);
  }

  /// \brief Visit mu node
  /// \param e A modal formula
  /// \param n A
  /// \param a A sequence of assignments to data variables
  /// \param f A modal formula
  /// \return The result of visiting the node
  state_formula visit_mu(const state_formula& e, const core::identifier_string& n, const data::assignment_list& a, const state_formula& f)
  {
    core::identifier_string new_name = push(n);
    state_formula new_formula = visit(f);
    pop();
    return state_frm::mu(new_name, a, new_formula);
  }

  /// \brief Visit nu node
  /// \param e A modal formula
  /// \param n A
  /// \param a A sequence of assignments to data variables
  /// \param f A modal formula
  /// \return The result of visiting the node
  state_formula visit_nu(const state_formula& e, const core::identifier_string& n, const data::assignment_list& a, const state_formula& f)
  {
    core::identifier_string new_name = push(n);
    state_formula new_formula = visit(f);
    pop();
    return state_frm::nu(new_name, a, new_formula);
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
  return make_state_formula_predicate_variable_rename_builder(generator).visit(f);
}

/// \brief Renames all data variables in the formula f using the supplied identifier generator.
/// \param f A modal formula
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
state_formula rename_variables(const state_formula& f, IdentifierGenerator& generator)
{
  // find all data variables in f
  std::set<data::variable> src = data::find_variables(f);

  // create a mapping of replacements
  data::mutable_map_substitution<> replacements;

  for (std::set<data::variable>::const_iterator i = src.begin(); i != src.end(); ++i)
  {
    replacements[*i] = data::variable(generator(i->name()), i->sort());
  }

  return data::replace_variables(f, replacements);
}

} // namespace modal_formula

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_VARIABLE_RENAME_H
