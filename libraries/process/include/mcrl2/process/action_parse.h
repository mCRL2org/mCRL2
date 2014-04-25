// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/action_parse.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ACTION_PARSE_H
#define MCRL2_PROCESS_ACTION_PARSE_H

#include "mcrl2/data/parse.h"
#include "mcrl2/process/untyped_action.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/typecheck.h"

namespace mcrl2 {

namespace process {

struct action_actions: public data::data_specification_actions
{
  action_actions(const core::parser_table& table_)
    : data::data_specification_actions(table_)
  {}

  untyped_action parse_Action(const core::parse_node& node)
  {
    return process::untyped_action(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  untyped_action_list parse_ActionList(const core::parse_node& node)
  {
    return parse_list<process::untyped_action>(node, "Action", boost::bind(&action_actions::parse_Action, this, _1));
  }

  bool callback_ActDecl(const core::parse_node& node, action_label_vector& result)
  {
    if (symbol_name(node) == "ActDecl")
    {
      core::identifier_string_list ids = parse_IdList(node.child(0));
      data::sort_expression_list sorts = parse_SortProduct(node.child(1));
      for (core::identifier_string_list::iterator i = ids.begin(); i != ids.end(); ++i)
      {
        result.push_back(action_label(*i, sorts));
      }
      return true;
    }
    return false;
  };

  action_label_list parse_ActDeclList(const core::parse_node& node)
  {
    action_label_vector result;
    traverse(node, boost::bind(&action_actions::callback_ActDecl, this, _1, boost::ref(result)));
    return process::action_label_list(result.begin(), result.end());
  }

  action_label_list parse_ActSpec(const core::parse_node& node)
  {
    return parse_ActDeclList(node.child(1));
  }
};

/// \brief Parses an action declaration from a string
/// \param text A string containing an action declaration
/// \param[in] data_spec A data specification used for sort normalization
/// \return A list of action labels
/// \exception mcrl2::runtime_error when the input does not match the syntax of an action declaration.
inline
process::action_label_list parse_action_declaration(const std::string& text, const data::data_specification& data_spec = data::detail::default_specification())
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("ActDecl");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  action_label_vector result;
  action_actions(parser_tables_mcrl2).callback_ActDecl(node, result);
  p.destroy_parse_node(node);
  process::action_label_list v(result.begin(), result.end());
  v = process::normalize_sorts(v, data_spec);
  return v;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ACTION_PARSE_H
