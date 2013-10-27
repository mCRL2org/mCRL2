// Author(s): Wieger Wesselink, Jeroen Keiren, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/parse.h
/// \brief Parser for data specifications.

#ifndef MCRL2_DATA_PARSE_H
#define MCRL2_DATA_PARSE_H

#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "boost/algorithm/string.hpp"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/standard_container_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"

#include <boost/algorithm/string/trim.hpp>

namespace mcrl2
{

namespace data
{

struct sort_expression_actions: public core::default_parser_actions
{
  sort_expression_actions(const core::parser_table& table_)
    : core::default_parser_actions(table_)
  {}

  data::sort_expression parse_SortExpr(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "SimpleSortExpr")) { return parse_SimpleSortExpr(node.child(0)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "HashArgs") && (symbol_name(node.child(2)) == "SortExpr")) { return function_sort(parse_SimpleSortExprList(node.child(0)), parse_SortExpr(node.child(2))); }
    report_unexpected_node(node);
    return data::sort_expression();
  }

  data::sort_expression parse_SimpleSortExpr(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Bool")) { return sort_bool::bool_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Pos")) { return sort_pos::pos(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Nat")) { return sort_nat::nat(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Int")) { return sort_int::int_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Real")) { return sort_real::real_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "List") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_list::list(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Set") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_set::set_(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "FSet") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_fset::fset(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Bag") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_bag::bag(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "FBag") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_fbag::fbag(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return basic_sort(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "SortExpr") && (symbol_name(node.child(2)) == ")")) { return parse_SortExpr(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "struct") && (symbol_name(node.child(1)) == "ConstrDeclList")) { return structured_sort(parse_ConstrDeclList(node.child(1))); }
    report_unexpected_node(node);
    return data::sort_expression();
  }

  data::sort_expression_list parse_SortExprList(const core::parse_node& node)
  {
    return parse_list<data::sort_expression>(node, "SortExpr", boost::bind(&sort_expression_actions::parse_SortExpr, this, _1));
  }

  data::sort_expression_list parse_SimpleSortExprList(const core::parse_node& node)
  {
    return parse_list<data::sort_expression>(node, "SimpleSortExpr", boost::bind(&sort_expression_actions::parse_SimpleSortExpr, this, _1));
  }

  data::structured_sort_constructor parse_ConstrDecl(const core::parse_node& node)
  {
    core::identifier_string name = parse_Id(node.child(0));
    data::structured_sort_constructor_argument_list arguments;
    core::identifier_string recogniser = atermpp::empty_string();
    if (node.child(1))
    {
      arguments = parse_ProjDeclList(node.child(1));
    }
    if (node.child(2))
    {
      core::parse_node u = node.child(2);
      if (u.child(0))
      {
        recogniser = parse_Id(node.child(2).child(0).child(1));
      }
    }
    return structured_sort_constructor(name, arguments, recogniser);
  }

  data::structured_sort_constructor_list parse_ConstrDeclList(const core::parse_node& node)
  {
    return parse_list<data::structured_sort_constructor>(node, "ConstrDecl", boost::bind(&sort_expression_actions::parse_ConstrDecl, this, _1));
  }

  data::structured_sort_constructor_argument parse_ProjDecl(const core::parse_node& node)
  {
    core::identifier_string name = atermpp::empty_string();
    sort_expression sort = parse_SortExpr(node.child(1));
    if (node.child(0).child(0))
    {
      // TODO: check if this nesting depth is correct
      name = parse_Id(node.child(0).child(0).child(0));
    }
    return structured_sort_constructor_argument(name, sort);
  }

  data::structured_sort_constructor_argument_list parse_ProjDeclList(const core::parse_node& node)
  {
    return parse_list<data::structured_sort_constructor_argument>(node, "ProjDecl", boost::bind(&sort_expression_actions::parse_ProjDecl, this, _1));
  }
};

struct data_expression_actions: public sort_expression_actions
{
  data_expression_actions(const core::parser_table& table_)
    : sort_expression_actions(table_)
  {}

  data_expression make_untyped_set_or_bag_comprehension(const variable& v, const data_expression& x)
  {
    return abstraction(untyped_set_or_bag_comprehension_binder(), atermpp::make_list(v), x);
  }

  data_expression make_list_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return sort_list::list_enumeration(untyped_sort(),x);
  }

  data_expression make_set_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return sort_set::set_enumeration(untyped_sort(),x);
  }

  data_expression make_bag_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return sort_bag::bag_enumeration(untyped_sort(),x);
  }

  data_expression make_function_update(const data_expression& x, const data_expression& y, const data_expression& z)
  {
    return make_application(function_symbol(mcrl2::data::function_update_name(),untyped_sort()), x, y, z);
  }

  template <typename ExpressionContainer>
  data::sort_expression_list get_sorts(const ExpressionContainer& x) const
  {
    data::sort_expression_vector result;
    for (typename ExpressionContainer::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      result.push_back(i->sort());
    }
    return data::sort_expression_list(result.begin(), result.end());
  }

  data::variable parse_VarDecl(const core::parse_node& node)
  {
    return variable(parse_Id(node.child(0)), parse_SortExpr(node.child(2)));
  }

  bool callback_VarsDecl(const core::parse_node& node, variable_vector& result)
  {
    if (symbol_name(node) == "VarsDecl")
    {
      core::identifier_string_list names = parse_IdList(node.child(0));
      data::sort_expression sort = parse_SortExpr(node.child(2));
      for (core::identifier_string_list::iterator i = names.begin(); i != names.end(); ++i)
      {
        result.push_back(variable(*i, sort));
      }
      return true;
    }
    return false;
  };

  data::variable_list parse_VarsDeclList(const core::parse_node& node)
  {
    variable_vector result;
    traverse(node, boost::bind(&data_expression_actions::callback_VarsDecl, this, _1, boost::ref(result)));
    return data::variable_list(result.begin(), result.end());
  }

  data::data_expression parse_DataExpr(const core::parse_node& node)
  {
    assert(symbol_name(node) == "DataExpr");
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return untyped_identifier(parse_Number(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "]")) { return untyped_identifier("[]"); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "}")) { return untyped_identifier("{}"); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == ":") && (symbol_name(node.child(2)) == "}")) { return untyped_identifier("{:}"); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "]")) { return make_list_enumeration(parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "BagEnumEltList") && (symbol_name(node.child(2)) == "}")) { return make_bag_enumeration(parse_BagEnumEltList(node.child(1))); }
    else if ((node.child_count() == 5) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "VarDecl") && (symbol_name(node.child(2)) == "|") && (symbol_name(node.child(3)) == "DataExpr") && (symbol_name(node.child(4)) == "}")) { return make_untyped_set_or_bag_comprehension(parse_VarDecl(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "}")) { return make_set_enumeration(parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "[") && (symbol_name(node.child(2)) == "DataExpr") && (symbol_name(node.child(3)) == "->") && (symbol_name(node.child(4)) == "DataExpr") && (symbol_name(node.child(5)) == "]")) { return make_function_update(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return application(parse_DataExpr(node.child(0)), parse_DataExprList(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return forall(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return exists(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "lambda") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return lambda(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "==") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "!=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "in") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "|>") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<|") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "++") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "-") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "/") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "div") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "mod") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "whr") && (symbol_name(node.child(2)) == "AssignmentList") && (symbol_name(node.child(3)) == "end")) { return where_clause(parse_DataExpr(node.child(0)), parse_AssignmentList(node.child(2))); }
    report_unexpected_node(node);
    return data::data_expression();
  }

  data::data_expression parse_DataExprUnit(const core::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return untyped_identifier(parse_Number(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return application(parse_DataExprUnit(node.child(0)), parse_DataExprList(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    report_unexpected_node(node);
    return data::data_expression();
  }

  data::data_expression parse_DataValExpr(const core::parse_node& node)
  {
    return parse_DataExpr(node.child(2));
  }

  data::untyped_identifier_assignment parse_Assignment(const core::parse_node& node)
  {
    return untyped_identifier_assignment(parse_Id(node.child(0)), parse_DataExpr(node.child(2)));
  }

  data::untyped_identifier_assignment_list parse_AssignmentList(const core::parse_node& node)
  {
    return parse_list<data::untyped_identifier_assignment>(node, "Assignment", boost::bind(&data_expression_actions::parse_Assignment, this, _1));
  }

  data::data_expression_list parse_DataExprList(const core::parse_node& node)
  {
    return parse_list<data::data_expression>(node, "DataExpr", boost::bind(&data_expression_actions::parse_DataExpr, this, _1));
  }

  data::data_expression_list parse_BagEnumEltList(const core::parse_node& node)
  {
    return parse_DataExprList(node);
  }
};

struct data_specification_actions: public data_expression_actions
{
  data_specification_actions(const core::parser_table& table_)
    : data_expression_actions(table_)
  {}

  bool callback_SortDecl(const core::parse_node& node, std::vector<atermpp::aterm_appl>& result)
  {
    if (symbol_name(node) == "SortDecl")
    {
      if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "IdList") && (symbol_name(node.child(1)) == ";"))
      {
        core::identifier_string_list ids = parse_IdList(node.child(0));
        for (core::identifier_string_list::iterator i = ids.begin(); i != ids.end(); ++i)
        {
          result.push_back(basic_sort(*i));
        }
      }
      else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "=") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ";"))
      {
        result.push_back(alias(basic_sort(parse_Id(node.child(0))), parse_SortExpr(node.child(2))));
      }
      else
      {
        report_unexpected_node(node);
      }
      return true;
    }
    return false;
  };

  std::vector<atermpp::aterm_appl> parse_SortDeclList(const core::parse_node& node)
  {
    std::vector<atermpp::aterm_appl> result;
    traverse(node, boost::bind(&data_specification_actions::callback_SortDecl, this, _1, boost::ref(result)));
    return result;
  }

  std::vector<atermpp::aterm_appl> parse_SortSpec(const core::parse_node& node)
  {
    return parse_SortDeclList(node.child(1));
  }

  bool callback_IdsDecl(const core::parse_node& node, function_symbol_vector& result)
  {
    if (symbol_name(node) == "IdsDecl")
    {
      core::identifier_string_list names = parse_IdList(node.child(0));
      data::sort_expression sort = parse_SortExpr(node.child(2));
      for (core::identifier_string_list::iterator i = names.begin(); i != names.end(); ++i)
      {
        result.push_back(function_symbol(*i, sort));
      }
      return true;
    }
    return false;
  };

  data::function_symbol_vector parse_IdsDeclList(const core::parse_node& node)
  {
    function_symbol_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_IdsDecl, this, _1, boost::ref(result)));
    return result;
  }

  data::function_symbol_vector parse_ConsSpec(const core::parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::function_symbol_vector parse_MapSpec(const core::parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::variable_list parse_GlobVarSpec(const core::parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  data::variable_list parse_VarSpec(const core::parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  bool callback_EqnDecl(const core::parse_node& node, const variable_list& variables, data_equation_vector& result)
  {
    if (symbol_name(node) == "EqnDecl")
    {
      data_expression condition = sort_bool::true_();
      // TODO: check if this is the correct nesting depth
      if (node.child(0).child(0))
      {
        condition = parse_DataExpr(node.child(0).child(0).child(0));
      }
      result.push_back(data_equation(variables, condition, parse_DataExpr(node.child(1)), parse_DataExpr(node.child(3))));
      return true;
    }
    return false;
  };

  data::data_equation_vector parse_EqnDeclList(const core::parse_node& node, const variable_list& variables)
  {
    data_equation_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_EqnDecl, this, _1, boost::ref(variables), boost::ref(result)));
    return result;
  }

  data::data_equation_vector parse_EqnSpec(const core::parse_node& node)
  {
    assert(symbol_name(node) == "EqnSpec");
    variable_list variables = parse_VarSpec(node.child(0));
    return parse_EqnDeclList(node.child(2), variables);
  }

  bool callback_DataSpecElement(const core::parse_node& node, data_specification& result)
  {
    if (symbol_name(node) == "SortSpec")
    {
      std::vector<atermpp::aterm_appl> v = parse_SortSpec(node);
      for (std::vector<atermpp::aterm_appl>::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (is_alias(*i))
        {
          result.add_alias(alias(*i));
        }
        else
        {
          result.add_sort(basic_sort(*i));
        }
      }
      return true;
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      function_symbol_vector v = parse_ConsSpec(node);
      for (function_symbol_vector::iterator i = v.begin(); i != v.end(); ++i)
      {
        result.add_constructor(*i);
      }
      return true;
    }
    else if (symbol_name(node) == "MapSpec")
    {
      function_symbol_vector v = parse_MapSpec(node);
      for (function_symbol_vector::iterator i = v.begin(); i != v.end(); ++i)
      {
        result.add_mapping(*i);
      }
      return true;
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      data_equation_vector v = parse_EqnSpec(node);
      for (data_equation_vector::iterator i = v.begin(); i != v.end(); ++i)
      {
        result.add_equation(*i);
      }
      return true;
    }
    return false;
  }

  data::data_specification parse_DataSpec(const core::parse_node& node)
  {
    data_specification result;
    traverse(node, boost::bind(&data_specification_actions::callback_DataSpecElement, this, _1, boost::ref(result)));
    return result;
  }
};

inline
sort_expression parse_sort_expression_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("SortExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  sort_expression result = data_expression_actions(parser_tables_mcrl2).parse_SortExpr(node);
  p.destroy_parse_node(node);
  return result;
}

inline
variable_list parse_variables_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("VarSpec");
  bool partial_parses = false;
  std::string var_text("var " + text);
  core::parse_node node = p.parse(var_text, start_symbol_index, partial_parses);
  variable_list result = data_specification_actions(parser_tables_mcrl2).parse_VarSpec(node);
  p.destroy_parse_node(node);
  return result;
}

inline
data_expression parse_data_expression_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("DataExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  data_expression result = data_expression_actions(parser_tables_mcrl2).parse_DataExpr(node);
  p.destroy_parse_node(node);
  return result;
}

inline
data_specification parse_data_specification_new(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("DataSpec");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  data_specification result = data_specification_actions(parser_tables_mcrl2).parse_DataSpec(node);
  p.destroy_parse_node(node);
  return result;
}

/// \cond INTERNAL_DOCS
namespace detail
{
inline static data_specification const& default_specification()
{
  static data_specification specification;

  return specification;
}
} // namespace detail
/// \endcond

inline
void complete_sort_expression(sort_expression& x, const data_specification& data_spec = detail::default_specification())
{
  type_check(x, data_spec);
  x = normalize_sorts(x, data_spec);
}

template <typename VariableIterator>
void complete_data_expression(data_expression& x, VariableIterator first, VariableIterator last, const data_specification& data_spec = detail::default_specification())
{
  type_check(x, first, last, data_spec);
  x = data::translate_user_notation(x);
  x = data::normalize_sorts(x, data_spec);
}

inline
void complete_data_specification(data_specification& x)
{
  type_check(x);
}

/// \brief Parses a and type checks a data specification.
/// \details This function reads a data specification in
///    input string text. It is assumed that the string contains
///    a single data specification, and nothing else.
///    If a parse or type check error is detected
///    an mcrl2::runtime_error exception is raised with a string that
///    indicates the problem. <br>
///    A typical example of a specification is:
///    \code
///     sort D=struct d1 | d2;
///          F=D->Set(D);
///          Natural;
///     cons zero:Natural; plus:Natural->Natural;
///     map  plus:Natural#Natural->Natural;
///     var  x,y:Natural;
///     eqn  y==zero -> plus(x,y)=x;
///          plus(x,succ(y))=succ(plus(x,y));
///    \endcode
///  \param[in] in A input stream containing the data specification.
///  \return the data specification corresponding to text.
inline
data_specification parse_data_specification(std::istream& in)
{
  std::string text = utilities::read_text(in);
  data_specification result = parse_data_specification_new(text);
  complete_data_specification(result);
  return result;
}

/// \brief Parses a and type checks a data specification.
/// \details This function reads a data specification in
///    input string text.
///    See for an example the function parse_data_expression
///    on a string.
///  \param[in] text A textual description of the data specification.
///  \return the data specification corresponding to the input istream.
inline
data_specification parse_data_specification(const std::string& text)
{
  // handle empty data specification
  if (boost::trim_copy(text).empty())
  {
    return data_specification();
  }
  std::istringstream spec_stream(text);
  return parse_data_specification(spec_stream);
}

/// \brief Parses and type checks a data variable declaration list checking for double occurrences
///        of variables in an existing variable range.
/// \details The shape of the variables are x_11,...,x_1n:S_1; ... x_m1,...,x_mk:S_m where
///      x_ij are variable strings and S_i are sort  expressions. It is checked that the
///      sort expressions are properly typed regarding the data specification and that
///      the variable names do not clash with the names of mappings and constructors. It
///      is also not allowed to use a variable name twice. If an optional range of variables
///      is given, then it is also checked that there are no conflicts with
///      variable names in this range. An mcrl2::runtime_error exception is raised when an
///      error occurs. In this case no names added using the input iterator. The
///      default data specification contains all standard data types.<br>
///      The output iterator can be used as follows, on standard variable lists.
///      \code
///         variable_list l;
///         parse_variables("x:Nat; y:Pos", std::front_inserter(l));
///      \endcode
/// \param[in] in An input stream containing the variable declarations to be parsed.
/// \param[out] o An output interator indicating where the parsed variables must be inserted.
/// \param[in]  begin The start of a variable range against which the variables are checked
///             for double occurrences.
/// \param[in]  end   The end of the variable range against which the parsed variables are checked.
/// \param[in] data_spec The data specification that is used for type checking.

template < typename Output_iterator, typename Variable_iterator >
void parse_variables(std::istream& in,
                     Output_iterator o,
                     const Variable_iterator begin,
                     const Variable_iterator end,
                     const data_specification& data_spec = detail::default_specification())
{
  // Parse the variables list.
  std::string text = utilities::read_text(in);
  variable_list data_vars = parse_variables_new(text);

  // Type check the variable list.
  /* atermpp::aterm_appl d=mcrl2::data::detail::data_specification_to_aterm_data_spec(
                                      mcrl2::data::remove_all_system_defined(data_spec)); */
  // atermpp::aterm_appl d=mcrl2::data::detail::data_specification_to_aterm_data_spec(data_spec);

  atermpp::aterm_list temporary_data_vars = data_vars;
  data_type_checker type_checker(data_spec);
  // temporary_data_vars = core::type_check_data_vars(data_vars, d);
  temporary_data_vars = type_checker(data_vars);

  if (temporary_data_vars == atermpp::aterm_list(atermpp::aterm()))
  {
    throw mcrl2::runtime_error("Error while type checking data variable declarations.");
  }
  data_vars=variable_list(temporary_data_vars);

  // Undo sort renamings for compatibility with type checker
  // data_vars = data::detail::undo_compatibility_renamings(data_spec, data_vars);
  data_vars = atermpp::reverse(data_vars);
  data_vars = normalize_sorts(data_vars,data_spec);

  // Check that variables do not have equal names.
  for (variable_list::const_iterator v=data_vars.begin(); v!=data_vars.end(); ++v)
  {
    for (Variable_iterator i=begin; i!=end; ++i)
    {
      if (v->name()==i->name())
      {
        throw mcrl2::runtime_error("Name conflict of variables " + data::pp(*i) + " and " + data::pp(*v) + ".");
      }
    }
    for (variable_list::const_iterator v1=data_vars.begin(); v1!=data_vars.end(); ++v1)
    {
      if (((*v1)!=(*v)) && (v1->name()==v->name()))
      {
        throw mcrl2::runtime_error("Name conflict of variables " + data::pp(*v1) + " and " + data::pp(*v) + ".");
      }
    }
  }

  // Output the variables read via the Output iterator.
  std::copy(data_vars.begin(), data_vars.end(), o);
}

/// \brief Parses and type checks a data variable declaration list checking for double occurrences
///        of variables in an existing variable range.
/// \details See parse_variables on a string for more explanation.
/// \param[in] text A textual description of the variable declarations to be parsed.
/// \param[out] i An input interator indicating where the parsed variables must be inserted.
/// \param[in]  begin The start of a variable range against which the variables are checked
///             for double occurrences.
/// \param[in]  end   The end of the variable range against which the parsed variables are checked.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename Output_iterator, typename Variable_iterator>
void parse_variables(const std::string& text,
                     Output_iterator i,
                     Variable_iterator begin,
                     Variable_iterator end,
                     const data_specification& data_spec = detail::default_specification())
{
  std::istringstream spec_stream(text);
  parse_variables(spec_stream, i, begin, end, data_spec);
}

/// \brief Parses and type checks a data variable declaration list.
/// \details See parse_variables on a string for more explanation.
/// \param[in] text A textual description of the variable declarations to be parsed.
/// \param[out] i An input interator indicating where the parsed variables must be inserted.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename Output_iterator>
void parse_variables(std::istream& text,
                     Output_iterator i,
                     const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  parse_variables(text,i,v_list.begin(),v_list.end(),data_spec);
}

/// \brief Parses and type checks a data variable declaration list.
/// \details See parse_variables on a string for more explanation.
/// \param[in] text A textual description of the variable declarations to be parsed.
/// \param[out] i An input interator indicating where the parsed variables must be inserted.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename Output_iterator>
void parse_variables(const std::string& text,
                     Output_iterator i,
                     const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  parse_variables(text,i,v_list.begin(),v_list.end(),data_spec);
}

/// \brief Parses and type checks a data variable declaration.
/// \details
///    See the information for reading a variable declaration from a string.
///  \param[in] text A textual description of the variable declaration.
///  \param[in] data_spec The data specification that is used for type checking.
///  \return the variable corresponding to the input istream.
inline
variable parse_variable(const std::string& text,
                        const data_specification& data_spec = detail::default_specification())
{
  std::vector < variable > variable_store;

  parse_variables(text + ";", std::back_inserter(variable_store),data_spec);

  if (variable_store.size()==0)
  {
    throw mcrl2::runtime_error("Input does not contain a variable declaration.");
  }
  if (variable_store.size()>1)
  {
    throw mcrl2::runtime_error("Input contains more than one variable declaration.");
  }

  return variable_store.front();
}

/// \brief Parses and type checks a data variable declaration.
/// \details
///    A variable declaration has the form x:S where x is a string and S is a
///    sort expression. No trailing information after the declaration of the
///    variable is allowed. The declaration is checked using the data specification
///    that is provided. The default data specification contains all standard
///    data types.
///    If a parse or typecheck error occurs an mcrl2::runtime_error exception
///    is raised.
///  \param[in] text A textual description of the variable declaration.
///  \param[in] data_spec The data specification that is used for type checking.
///  \return the variable corresponding to the string text.
inline
variable parse_variable(std::istream& text,
                        const data_specification& data_spec = detail::default_specification())
{
  std::ostringstream input;
  input << text.rdbuf();
  return parse_variable(input.str(),data_spec);
}

/// \brief Parses and type checks a data expression.
/// \details
///     A data expression is read from the input where it is assumed that
///     it can contain variables from the range from begin to end. The data
///     expression is type checked using the given data specification data_spec.
///     The default data specification contains all standard sorts and functions.
///     If a parse or type check error occurs this is reported using a mcrl2::runtime_error
///     exception. It is assumed that the input contains exactly one expression, and nothing
///     else.
/// \param[in] in The input stream containing a data expression.
/// \param[in] first The start of a variables that can occur in the data expression.
/// \param[in] last  The end of the potentially free variables in the expression.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename Variable_iterator>
data_expression parse_data_expression(std::istream& in,
                                      const Variable_iterator first,
                                      const Variable_iterator last,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::string text = utilities::read_text(in);
  data_expression result = parse_data_expression_new(text);
  complete_data_expression(result, first, last, data_spec);
  return result;
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in]  begin The start of a variables that can occur in the data expression.
/// \param[in]  end   The end of the potentially free variables in the expression.
/// \param[in] data_spec The data specification that is used for type checking.

template <typename Variable_iterator>
data_expression parse_data_expression(const std::string& text,
                                      const Variable_iterator begin,
                                      const Variable_iterator end,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::istringstream spec_stream(text);
  return parse_data_expression(spec_stream, begin,end, data_spec);
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] data_spec The data specification that is used for type checking.
inline
data_expression parse_data_expression(std::istream& text,
                                      const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  return parse_data_expression(text,v_list.begin(),v_list.end(),data_spec);
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] data_spec The data specification that is used for type checking.
inline
data_expression parse_data_expression(const std::string& text,
                                      const data_specification& data_spec = detail::default_specification())
{
  variable_list v_list;
  return parse_data_expression(text,v_list.begin(),v_list.end(),data_spec);
}

/// \brief Parses and type checks a data expression.
/// \details
///     See parsing a data expression from a string for details.
/// \param[in] text The input text containing a data expression.
/// \param[in] var_decl a list of variable declarations
/// \param[in] data_spec The data specification that is used for type checking.
inline
data_expression parse_data_expression(const std::string& text,
                                      const std::string& var_decl,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::vector < variable > variable_store;
  if (!var_decl.empty())
  {
    parse_variables(var_decl,std::back_inserter(variable_store),data_spec);
  }
  return parse_data_expression(text,variable_store.begin(),variable_store.end(),data_spec);
}

/// \brief Parses and type checks a sort expression.
/// \details See parsing a sort expression from a string for details.
/// \param[in] in An input stream containing a sort expression.
/// \param[in] data_spec The data specification that is used for type checking.
inline
sort_expression parse_sort_expression(std::istream& in,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::string text = utilities::read_text(in);
  sort_expression result = parse_sort_expression_new(text);
  complete_sort_expression(result, data_spec);
  return result;
}

/// \brief Parses and type checks a sort expression.
/// \details
///     Parses and type checks the sort expression. An error is signalled using
///     the mcrl2::runtime_error exception. This routine expects exactly one sort
///     expression on the input. The default data specification contains all standard
///     sorts.
/// \param[in] text The input text containing a sort expression.
/// \param[in] data_spec The data specification that is used for type checking.
inline
sort_expression parse_sort_expression(const std::string& text,
                                      const data_specification& data_spec = detail::default_specification())
{
  std::istringstream spec_stream(text);
  return parse_sort_expression(spec_stream, data_spec);
}


/// \brief Parses a single data expression.
/// \param text A string
/// \param var_decl A string
/// with their types.<br>
/// An example of this is:
/// \code
///   m, n: Nat;
///   b: Bool;
/// \endcode
/// \param data_spec A string
/// \return The parsed expression
inline
data_expression parse_data_expression(std::string text, std::string var_decl, std::string data_spec)
{
  return parse_data_expression(text,var_decl,data::parse_data_specification(data_spec));
}

// parse a string like 'tail: List(D) -> List(D)'
//
// TODO: replace this by a proper parse function once the current parser and type checker have been replaced
inline
data::function_symbol parse_function_symbol(std::string text, const std::string& dataspec_text = "")
{
  const std::string prefix = "UNIQUE_FUNCTION_SYMBOL_PREFIX";
  boost::algorithm::trim(text);
  std::string::size_type pos = text.find_first_of(':');
  std::string name = boost::algorithm::trim_copy(text.substr(0, pos));
  std::string type = prefix + text.substr(pos);
  std::string spec_text = dataspec_text + "\nmap " + prefix + type + ";\n";
  data::data_specification dataspec = data::parse_data_specification(spec_text);
  data::function_symbol f = dataspec.user_defined_mappings().back();
  data::function_symbol result = data::function_symbol(name, f.sort());
  return result;
}

/// \cond INTERNAL_DOCS
namespace detail
{
/// \brief Parses a data variable that is applied to arguments.
/// This is typically used for parsing pbes variables or variables in the modal formula context.
/// For example: "X(d:D,e:E)".
/// \param s A string
/// \return The parsed data variable
inline
std::pair<std::string, data_expression_list> parse_variable(std::string const& s)
{
  using boost::algorithm::split;
  using boost::algorithm::is_any_of;

  std::string name;
  data_expression_vector variables;

  std::string::size_type idx = s.find('(');
  if (idx == std::string::npos)
  {
    name = s;
  }
  else
  {
    name = s.substr(0, idx);
    assert(*s.rbegin() == ')');
    std::vector<std::string> v;
    std::string w = s.substr(idx + 1, s.size() - idx - 2);
    split(v, w, is_any_of(","));
    // This doesn't compile in combination with 'using namespace std::rel_ops'
    // for Visual C++ 8.0 (looks like a compiler bug)
    // for (std::vector<std::string>::reverse_iterator i = v.rbegin(); i != v.rend(); ++i)
    // {
    //   data_expression d = variable(*i);
    //   variables = push_front(variables, d);
    // }
    for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
    {
      variables.push_back(data::parse_variable(*i));
    }
  }
  return std::make_pair(name, data_expression_list(variables.begin(), variables.end()));
}
} // namespace detail
/// \endcond

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PARSE_H
