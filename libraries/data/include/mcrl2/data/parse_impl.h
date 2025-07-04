// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/parse_impl.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_PARSE_IMPL_H
#define MCRL2_DATA_PARSE_IMPL_H

#include <climits>
#include <fstream>
#include "mcrl2/core/parser_utility.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/data/standard_container_utility.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/untyped_data_specification.h"

namespace mcrl2::data::detail
{

struct sort_expression_actions: public core::default_parser_actions
{
  explicit sort_expression_actions(const core::parser& parser_)
    : core::default_parser_actions(parser_)
  {}

  data::sort_expression parse_SortExpr(const core::parse_node& node, data::sort_expression_list* product=nullptr) const
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
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "SortExpr") && (symbol_name(node.child(2)) == ")")) { return parse_SortExpr(node.child(1), product); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "struct") && (symbol_name(node.child(1)) == "ConstrDeclList")) { return structured_sort(parse_ConstrDeclList(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "SortExpr") && (node.child(1).string() == "->") && (symbol_name(node.child(2)) == "SortExpr")) { return function_sort(parse_SortExpr_as_SortProduct(node.child(0)), parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "SortExpr") && (node.child(1).string() == "#") && (symbol_name(node.child(2)) == "SortExpr"))
    {
      if (product != nullptr)
      {
        data::sort_expression new_element = parse_SortExpr(node.child(2), product);
        if (new_element != data::sort_expression())
        {
          product->push_front(new_element);
        }
        new_element = parse_SortExpr(node.child(0), product);
        if (new_element != data::sort_expression())
        {
          product->push_front(new_element);
        }
        return data::sort_expression();
      }
      else
      {
        throw core::parse_node_exception(node.child(1), "Sort product is only allowed on the left "
                                                        "hand side of ->, and when declaring actions.");
      }
    }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  data::sort_expression_list parse_SortExpr_as_SortProduct(const core::parse_node& node) const
  {
    data::sort_expression_list result;
    data::sort_expression new_element = parse_SortExpr(node, &result);
    if (new_element != data::sort_expression())
    {
      result.push_front(new_element);
    }
    return result;
  }

  data::sort_expression_list parse_SortProduct(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "SortExpr"))
    {
      return parse_SortExpr_as_SortProduct(node.child(0));
    }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  data::structured_sort_constructor parse_ConstrDecl(const core::parse_node& node) const
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

  data::structured_sort_constructor_list parse_ConstrDeclList(const core::parse_node& node) const
  {
    return parse_list<data::structured_sort_constructor>(node, "ConstrDecl", [&](const core::parse_node& node) { return parse_ConstrDecl(node); });
  }

  data::structured_sort_constructor_argument parse_ProjDecl(const core::parse_node& node) const
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

  data::structured_sort_constructor_argument_list parse_ProjDeclList(const core::parse_node& node) const
  {
    return parse_list<data::structured_sort_constructor_argument>(node, "ProjDecl", [&](const core::parse_node& node) { return parse_ProjDecl(node); });
  }
};

struct data_expression_actions: public sort_expression_actions
{
  data_expression_actions(const core::parser& parser_)
    : sort_expression_actions(parser_)
  {}

  data_expression make_untyped_set_or_bag_comprehension(const variable& v, const data_expression& x) const
  {
    return abstraction(untyped_set_or_bag_comprehension_binder(), { v }, x);
  }

  data_expression make_list_enumeration(const data_expression_list& x) const
  {
    assert(!x.empty());
    return sort_list::list_enumeration(untyped_sort(),x);
  }

  data_expression make_set_enumeration(const data_expression_list& x) const
  {
    assert(!x.empty());
    return sort_set::set_enumeration(untyped_sort(),x);
  }

  data_expression make_bag_enumeration(const data_expression_list& x) const
  {
    assert(!x.empty());
    return sort_bag::bag_enumeration(untyped_sort(),x);
  }

  data_expression make_function_update(const data_expression& x, const data_expression& y, const data_expression& z) const
  {
    return application(function_symbol(mcrl2::data::function_update_name(),untyped_sort()), x, y, z);
  }

  template <typename ExpressionContainer>
  data::sort_expression_list get_sorts(const ExpressionContainer& x) const
  {
    data::sort_expression_vector result;
    for (auto i = x.begin(); i != x.end(); ++i)
    {
      result.push_back(i->sort());
    }
    return data::sort_expression_list(result.begin(), result.end());
  }

  data::variable parse_VarDecl(const core::parse_node& node) const
  {
    return variable(parse_Id(node.child(0)), parse_SortExpr(node.child(2)));
  }

  bool callback_VarsDecl(const core::parse_node& node, variable_vector& result) const
  {
    if (symbol_name(node) == "VarsDecl")
    {
      core::identifier_string_list names = parse_IdList(node.child(0));
      data::sort_expression sort = parse_SortExpr(node.child(2));
      for (const core::identifier_string& name: names)
      {
        result.emplace_back(name, sort);
      }
      return true;
    }
    return false;
  };

  data::variable_list parse_VarsDeclList(const core::parse_node& node) const
  {
    variable_vector result;
    traverse(node, [&](const core::parse_node& node) { return callback_VarsDecl(node, result); });
    return data::variable_list(result.begin(), result.end());
  }

  data::data_expression parse_DataExpr(const core::parse_node& node) const
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
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return forall(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return exists(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "lambda") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return lambda(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "=>") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "&&") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "||") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "==") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "!=") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<=") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">=") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "in") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "|>") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<|") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "++") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "+") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "-") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "/") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "div") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "mod") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "*") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ".") && (symbol_name(node.child(2)) == "DataExpr")) { return application(untyped_identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "whr") && (symbol_name(node.child(2)) == "AssignmentList") && (symbol_name(node.child(3)) == "end")) { return where_clause(parse_DataExpr(node.child(0)), parse_AssignmentList(node.child(2))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  data::data_expression parse_DataExprUnit(const core::parse_node& node) const
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return untyped_identifier(parse_Number(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return untyped_identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return application(parse_DataExprUnit(node.child(0)), parse_DataExprList(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExprUnit")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExprUnit")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExprUnit")) { return application(untyped_identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }

  data::data_expression parse_DataValExpr(const core::parse_node& node) const
  {
    return parse_DataExpr(node.child(2));
  }

  data::untyped_identifier_assignment parse_Assignment(const core::parse_node& node) const
  {
    return untyped_identifier_assignment(parse_Id(node.child(0)), parse_DataExpr(node.child(2)));
  }

  data::untyped_identifier_assignment_list parse_AssignmentList(const core::parse_node& node) const
  {
    return parse_list<data::untyped_identifier_assignment>(node, "Assignment", [&](const core::parse_node& node) { return parse_Assignment(node); });
  }

  data::data_expression_list parse_DataExprList(const core::parse_node& node) const
  {
    return parse_list<data::data_expression>(node, "DataExpr", [&](const core::parse_node& node) { return parse_DataExpr(node); });
  }

  data::data_expression_list parse_BagEnumEltList(const core::parse_node& node) const
  {
    return parse_DataExprList(node);
  }
};

struct data_specification_actions: public data_expression_actions
{
  explicit data_specification_actions(const core::parser& parser_)
    : data_expression_actions(parser_)
  {}

  bool callback_SortDecl(const core::parse_node& node, std::vector<atermpp::aterm>& result) const
  {
    if (symbol_name(node) == "SortDecl")
    {
      if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "IdList") && (symbol_name(node.child(1)) == ";"))
      {
        core::identifier_string_list ids = parse_IdList(node.child(0));
        for (const core::identifier_string& id: ids)
        {
          result.push_back(basic_sort(id));
        }
      }
      else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "=") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ";"))
      {
        result.push_back(alias(basic_sort(parse_Id(node.child(0))), parse_SortExpr(node.child(2))));
      }
      else
      {
        throw core::parse_node_unexpected_exception(m_parser, node);
      }
      return true;
    }
    return false;
  }

  std::vector<atermpp::aterm> parse_SortDeclList(const core::parse_node& node) const
  {
    std::vector<atermpp::aterm> result;
    traverse(node, [&](const core::parse_node& node) { return callback_SortDecl(node, result); });
    return result;
  }

  std::vector<atermpp::aterm> parse_SortSpec(const core::parse_node& node) const
  {
    return parse_SortDeclList(node.child(1));
  }

  bool callback_IdsDecl(const core::parse_node& node, function_symbol_vector& result) const
  {
    if (symbol_name(node) == "IdsDecl")
    {
      core::identifier_string_list names = parse_IdList(node.child(0));
      data::sort_expression sort = parse_SortExpr(node.child(2));
      for (const core::identifier_string& name: names)
      {
        result.emplace_back(name, sort);
      }
      return true;
    }
    return false;
  }

  data::function_symbol_vector parse_IdsDeclList(const core::parse_node& node) const
  {
    function_symbol_vector result;
    traverse(node, [&](const core::parse_node& node) { return callback_IdsDecl(node, result); });
    return result;
  }

  data::function_symbol_vector parse_ConsSpec(const core::parse_node& node) const
  {
    return parse_IdsDeclList(node);
  }

  data::function_symbol_vector parse_MapSpec(const core::parse_node& node) const
  {
    return parse_IdsDeclList(node);
  }

  data::variable_list parse_GlobVarSpec(const core::parse_node& node) const
  {
    return parse_VarsDeclList(node);
  }

  data::variable_list parse_VarSpec(const core::parse_node& node) const
  {
    return parse_VarsDeclList(node);
  }

  bool callback_EqnDecl(const core::parse_node& node, const variable_list& variables, data_equation_vector& result) const
  {
    if (symbol_name(node) == "EqnDecl")
    {
      data_expression condition = sort_bool::true_();
      // TODO: check if this is the correct nesting depth
      if (node.child(0).child(0))
      {
        condition = parse_DataExpr(node.child(0).child(0).child(0));
      }
      result.emplace_back(variables, condition, parse_DataExpr(node.child(1)), parse_DataExpr(node.child(3)));
      return true;
    }
    return false;
  }

  data::data_equation_vector parse_EqnDeclList(const core::parse_node& node, const variable_list& variables) const
  {
    data_equation_vector result;
    traverse(node, [&](const core::parse_node& node) { return callback_EqnDecl(node, variables, result); });
    return result;
  }

  data::data_equation_vector parse_EqnSpec(const core::parse_node& node) const
  {
    assert(symbol_name(node) == "EqnSpec");
    variable_list variables = parse_VarSpec(node.child(0));
    return parse_EqnDeclList(node.child(2), variables);
  }

  bool callback_DataSpecElement(const core::parse_node& node, untyped_data_specification& result) const
  {
    if (symbol_name(node) == "SortSpec")
    {
      std::vector<atermpp::aterm> sorts = parse_SortSpec(node);
      for (const atermpp::aterm& t: sorts)
      {
        if (is_alias(t))
        {
          result.add_alias(alias(t));
        }
        else
        {
          result.add_sort(basic_sort(t));
        }
      }
      return true;
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      function_symbol_vector functions = parse_ConsSpec(node);
      for (const function_symbol& f: functions)
      {
        result.add_constructor(f);
      }
      return true;
    }
    else if (symbol_name(node) == "MapSpec")
    {
      function_symbol_vector functions = parse_MapSpec(node);
      for (const function_symbol& f: functions)
      {
        result.add_mapping(f);
      }
      return true;
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      data_equation_vector equations = parse_EqnSpec(node);
      for (const data_equation& eq: equations)
      {
        result.add_equation(eq);
      }
      return true;
    }
    return false;
  }

  untyped_data_specification parse_DataSpec(const core::parse_node& node) const
  {
    untyped_data_specification result;
    traverse(node, [&](const core::parse_node& node) { return callback_DataSpecElement(node, result); });
    return result;
  }
};

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_PARSE_IMPL_H
