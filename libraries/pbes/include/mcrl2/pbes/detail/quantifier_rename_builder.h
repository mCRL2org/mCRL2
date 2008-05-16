// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file quantifier_rename_builder.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_QUANTIFIER_RENAME_BUILDER_H
#define MCRL2_PBES_DETAIL_QUANTIFIER_RENAME_BUILDER_H

#include <algorithm>
#include <deque>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/pbes/pbes_expression_builder.h"
#include "mcrl2/data/detail/sequence_substitution.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor that renames quantifier variables, to make sure that within the scope of a quantifier
/// variable, no other quantifier variables or free variables with the same name occur.
/// The identifier generator that is supplied via the constructor is used to choose new names.
template <typename IdentifierGenerator>
struct quantifier_rename_builder: public pbes_expression_builder
{
  IdentifierGenerator& generator;
  std::vector<data::data_variable_list> quantifier_stack;
  std::deque<std::pair<data::data_variable, data::data_variable> > replacements;

  quantifier_rename_builder(IdentifierGenerator& generator)
    : generator(generator)
  {}

  /// returns true if the quantifier_stack contains a data variable with the given name
  bool is_in_quantifier_stack(core::identifier_string name) const
  {
    for (std::vector<data::data_variable_list>::const_iterator i = quantifier_stack.begin(); i != quantifier_stack.end(); ++i)
    {
      if (std::find(boost::make_transform_iterator(i->begin(), data::detail::data_variable_name()),
                    boost::make_transform_iterator(i->end()  , data::detail::data_variable_name()),
                    name
                   ) != boost::make_transform_iterator(i->end()  , data::detail::data_variable_name())
         )
      {
        return true;
      }
    }
    return false;
  }

  // Add variables to the quantifier stack, and add replacements for the name clashes to replacements.
  // Returns the number of replacements that were added.
  unsigned int push(const data::data_variable_list& variables)
  {
    unsigned int replacement_count = 0;

    // check for new name clashes
    for (data::data_variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (is_in_quantifier_stack(i->name()))
      {
        replacement_count++;
        data::data_variable new_variable(generator(i->name()), i->sort());

        // add the replacement in front, since it needs to be applied first
        replacements.push_front(std::make_pair(*i, new_variable));
      }
    }
    quantifier_stack.push_back(variables);
    generator.add_to_context(variables);

    return replacement_count;
  }

  // Removes the last added variable list from the quantifier stack, and removes
  // replacement_count replacements.
  void pop(unsigned int replacement_count)
  {
    generator.remove_from_context(quantifier_stack.back());
    for (unsigned int i = 0; i < replacement_count; i++)
    {
      generator.remove_identifier(replacements.front().second.name());
      replacements.pop_front();
    }
    quantifier_stack.pop_back();
  }

  pbes_expression visit_data_expression(const pbes_expression& e, const data::data_expression& d)
  {
    return d.substitute(data::detail::make_sequence_substitution(replacements));
  }

  pbes_expression visit_forall(const pbes_expression& e, const data::data_variable_list& variables, const pbes_expression& expression)
  {
    unsigned int replacement_count = push(variables);
    pbes_expression new_expression = visit(expression);
    data::data_variable_list new_variables = replacement_count > 0 ? variables.substitute(data::detail::make_sequence_substitution(replacements)) : variables;
    pop(replacement_count);
    return pbes_expr::forall(new_variables, new_expression);
  }

  pbes_expression visit_exists(const pbes_expression& e, const data::data_variable_list& variables, const pbes_expression& expression)
  {
    unsigned int replacement_count = push(variables);
    pbes_expression new_expression = visit(expression);
    data::data_variable_list new_variables = replacement_count > 0 ? variables.substitute(data::detail::make_sequence_substitution(replacements)) : variables;
    pop(replacement_count);
    return pbes_expr::exists(new_variables, new_expression);
  }
};

template <typename IdentifierGenerator>
quantifier_rename_builder<IdentifierGenerator> make_quantifier_rename_builder(IdentifierGenerator& generator)
{
  return quantifier_rename_builder<IdentifierGenerator>(generator);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_QUANTIFIER_RENAME_BUILDER_H
