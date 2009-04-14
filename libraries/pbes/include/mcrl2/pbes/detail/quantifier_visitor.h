// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file quantifier_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H
#define MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H

#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor for collecting the quantifier variables that occur in a pbes expression.
struct quantifier_visitor: public pbes_expression_visitor<pbes_expression>
{
  std::set<data::data_variable> variables;

  /// \brief Visit forall node
  /// \param e A PBES expression
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_forall(const pbes_expression& e, const data::data_variable_list& v, const pbes_expression&)
  {
    variables.insert(v.begin(), v.end());
    return stop_recursion;
  }

  /// \brief Visit exists node
  /// \param e A PBES expression
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_exists(const pbes_expression& e, const data::data_variable_list& v, const pbes_expression&)
  {
    variables.insert(v.begin(), v.end());
    return stop_recursion;
  }
};

/// \brief Visitor for determining if within the scope of a quantifier there are quantifier
/// variables of free variables with the same name.
struct quantifier_name_clash_visitor: public pbes_expression_visitor<pbes_expression>
{
  std::vector<data::data_variable_list> quantifier_stack;
  bool result;
  data::data_variable name_clash; // if result is true, then this attribute contains the conflicting variable

  quantifier_name_clash_visitor()
    : result(false)
  {}

  /// \brief Returns true if the quantifier_stack contains a data variable with the given name
  /// \param name A
  /// \return True if the quantifier_stack contains a data variable with the given name
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

  /// \brief Adds variables to the quantifier stack, and adds replacements for the name clashes to replacements.
  /// \param variables A sequence of data variables
  /// \return The number of replacements that were added.
  void push(const data::data_variable_list& variables)
  {
    if (result)
    {
      return;
    }
    for (data::data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (is_in_quantifier_stack(i->name()))
      {
        result = true;
        name_clash = *i;
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

  /// \brief Visit forall node
  /// \param e A PBES expression
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_forall(const pbes_expression& e, const data::data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return continue_recursion;
  }

  /// \brief Leave forall node
  void leave_forall()
  {
    pop();
  }

  /// \brief Visit exists node
  /// \param e A PBES expression
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_exists(const pbes_expression& e, const data::data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return continue_recursion;
  }

  /// \brief Leave exists node
  void leave_exists()
  {
    pop();
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H
