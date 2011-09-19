// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_parse.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_ACTION_PARSE_H
#define MCRL2_LPS_ACTION_PARSE_H

#include "mcrl2/data/parse.h"
#include "mcrl2/lps/multi_action.h"

namespace mcrl2 {

namespace lps {

struct action_actions: public data::data_specification_actions
{
  lps::action parse_Action(const core::parse_node& node)
  {
    return action(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  lps::action_list parse_ActionList(const core::parse_node& node)
  {
    return parse_list<lps::action_label>(node, "Action", boost::bind(&action_actions::parse_Action, this, _1));
  }

  bool callback_ActDecl(const core::parse_node& node, action_label_vector& result)
  {
    if (symbol_name(node) == "ActDecl")
    {
      core::identifier_string_list ids = parse_IdList(node.child(0));
      data::sort_expression_list sorts = parse_SortExprList(node.child(1));
      action_label_vector result;
      for (core::identifier_string_list::iterator i = ids.begin(); i != ids.end(); ++i)
      {
        result.push_back(action_label(*i, sorts));
      }
      return true;
    }
    return false;
  };

  lps::action_label_list parse_ActDeclList(const core::parse_node& node)
  {
    action_label_vector result;
    traverse(node, boost::bind(&action_actions::callback_ActDecl, this, _1, boost::ref(result)));
    return lps::action_label_list(result.begin(), result.end());
  }

  lps::action_label_list parse_ActSpec(const core::parse_node& node)
  {
    return parse_ActDeclList(node.child(1));
  }

  lps::multi_action parse_MultAct(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return lps::multi_action(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActionList")) { return lps::multi_action(parse_ActionList(node.child(0))); }
    report_unexpected_node(node);
    return lps::action_list();
  }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ACTION_PARSE_H
