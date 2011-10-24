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
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/typecheck.h"

namespace mcrl2 {

namespace lps {

struct action_actions: public data::data_specification_actions
{
  action_actions(const core::parser_table& table_)
    : data::data_specification_actions(table_)
  {}

  atermpp::aterm_appl parse_Action(const core::parse_node& node)
  {
    return core::detail::gsMakeParamId(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  atermpp::aterm_list parse_ActionList(const core::parse_node& node)
  {
    return parse_list<atermpp::aterm_appl>(node, "Action", boost::bind(&action_actions::parse_Action, this, _1));
  }

  bool callback_ActDecl(const core::parse_node& node, action_label_vector& result)
  {
    if (symbol_name(node) == "ActDecl")
    {
      core::identifier_string_list ids = parse_IdList(node.child(0));
      data::sort_expression_list sorts = parse_SortExprList(node.child(1));
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

inline
multi_action parse_multi_action_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2);
  unsigned int start_symbol_index = p.start_symbol_index("MultAct");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  return action_actions(parser_tables_mcrl2).parse_MultAct(node);
}

inline
multi_action parse_multi_action_old(std::istream& in)
{
  atermpp::aterm_appl x = core::parse_mult_act(in);
  if (!x)
  {
    throw mcrl2::runtime_error("Error while parsing multi action");
  }
  return multi_action(x);
}

inline
multi_action parse_multi_action_old(const std::string& text)
{
  std::istringstream in(text);
  return parse_multi_action_old(in);
}

inline
void complete_multi_action(multi_action& x, const lps::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  lps::type_check(x, data_spec, action_decls);
}

template <typename T>
void compare_parse_results(const std::string& text, const T& x1, const T& x2)
{
  if (!(x1 == x2))
  {
    std::clog << "--- WARNING: difference detected between old and new parser ---\n";
    std::clog << "string: " << text << std::endl;
    std::clog << "old:    " << lps::pp(x1) << std::endl;
    core::print_aterm(x1);
    std::clog << "new:    " << lps::pp(x2) << std::endl;
    core::print_aterm(x2);
#ifdef MCRL2_THROW_ON_PARSE_DIFFERENCES
    throw mcrl2::runtime_error("difference detected between old and new parser");
#endif
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ACTION_PARSE_H
