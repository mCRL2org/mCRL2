// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/type_check_tree.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TYPE_CHECK_TREE_H
#define MCRL2_DATA_TYPE_CHECK_TREE_H

#include <iostream>
#include <memory>
#include <sstream>
#include "mcrl2/data/data_type_checker.h"
#include "mcrl2/data/parse.h"

namespace mcrl2 {

namespace data {

struct type_check_node;

typedef std::shared_ptr<type_check_node> type_check_node_ptr;

struct type_check_context
{
  std::map<core::identifier_string, sort_expression_list> system_constants;
  std::map<core::identifier_string, function_sort_list> system_functions;
  std::map<core::identifier_string, sort_expression> user_constants;
  std::map<core::identifier_string, function_sort_list> user_functions;
  std::map<core::identifier_string, sort_expression> declared_variables;
  // const std::map<core::identifier_string, sort_expression>& allowed_variables;

  type_check_context(
                     const std::map<core::identifier_string, sort_expression_list>& system_constants_,
                     const std::map<core::identifier_string, function_sort_list>& system_functions_,
                     const std::map<core::identifier_string, sort_expression>& user_constants_,
                     const std::map<core::identifier_string, function_sort_list>& user_functions_,
                     const std::map<core::identifier_string, sort_expression>& declared_variables_
                    )
    : system_constants(system_constants_),
      system_functions(system_functions_),
      user_constants(user_constants_),
      user_functions(user_functions_),
      declared_variables(declared_variables_)
  { }

  type_check_context(const data_type_checker_base& checker, const std::map<core::identifier_string, sort_expression>& declared_variables_)
    : system_constants(checker.system_constants()),
      system_functions(checker.system_functions()),
      user_constants(checker.user_constants()),
      user_functions(checker.user_functions()),
      declared_variables(declared_variables_)
  {}

  // Returns the system defined constants and the user defined constants matching with name
  std::pair<sort_expression_list, sort_expression_list> find_matching_constants(const std::string& name) const;

  // Returns the system defined functions and the user defined functions matching with (name, arity)
  std::pair<function_sort_list, function_sort_list> find_matching_functions(const std::string& name, std::size_t arity) const;

  // Returns the variables matching with name
  std::vector<sort_expression> find_matching_variable(const std::string& name) const;
};

struct type_check_constraint
{
  virtual std::string print() const = 0;
};

typedef std::shared_ptr<type_check_constraint> constraint_ptr;

// The sort of the corresponding data expression should be a subsort of 'sort'.
struct subsort_constraint: public type_check_constraint
{
  sort_expression sort;

  subsort_constraint(const sort_expression& sort_)
    : sort(sort_)
  {}

  std::string print() const
  {
    return "subsort(" + data::pp(sort) + ")";
  }
};

// The sort of the corresponding data expression should be equal to 'sort'.
struct is_sort_constraint: public type_check_constraint
{
  sort_expression sort;

  is_sort_constraint(const sort_expression& sort_)
    : sort(sort_)
  {}

  std::string print() const
  {
    return "is_sort(" + data::pp(sort) + ")";
  }
};

struct constant_constraint: public type_check_constraint
{
  sort_expression_list possible_sorts;

  constant_constraint(const sort_expression_list& possible_sorts_)
    : possible_sorts(possible_sorts_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "constant_constraint(";
    for (auto i = possible_sorts.begin(); i != possible_sorts.end(); ++i)
    {
      if (i != possible_sorts.begin())
      {
        out << ", ";
      }
      out << *i;
    }
    out << ")";
    return out.str();
  }
};

struct function_constraint: public type_check_constraint
{
  function_sort_list possible_sorts;

  function_constraint(const function_sort_list& possible_sorts_)
    : possible_sorts(possible_sorts_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "function_constraint(";
    for (auto i = possible_sorts.begin(); i != possible_sorts.end(); ++i)
    {
      if (i != possible_sorts.begin())
      {
        out << ", ";
      }
      out << *i;
    }
    out << ")";
    return out.str();
  }
};

// the sort of 'head' should be a function_sort, and the arguments should match with it
struct application_constraint: public type_check_constraint
{
  type_check_node_ptr head;
  std::vector<type_check_node_ptr> arguments;

  application_constraint(const std::vector<type_check_node_ptr>& nodes)
  {
    assert(!nodes.empty());
    head = nodes[0];
    arguments.insert(arguments.begin(), nodes.begin() + 1, nodes.end());
  }

  std::string print() const;
};

struct list_enumeration_constraint: public type_check_constraint
{
  std::vector<type_check_node_ptr> arguments;

  list_enumeration_constraint(const std::vector<type_check_node_ptr>& arguments_)
    : arguments(arguments_)
  {}

  std::string print() const;
};

struct set_enumeration_constraint: public type_check_constraint
{
  std::vector<type_check_node_ptr> arguments;

  set_enumeration_constraint(const std::vector<type_check_node_ptr>& arguments_)
    : arguments(arguments_)
  {}

  std::string print() const;
};

struct bag_enumeration_constraint: public type_check_constraint
{
  std::vector<type_check_node_ptr> arguments;

  bag_enumeration_constraint(const std::vector<type_check_node_ptr>& arguments_)
    : arguments(arguments_)
  {}

  std::string print() const;
};

struct bag_or_set_enumeration_constraint: public type_check_constraint
{
  std::vector<type_check_node_ptr> arguments;

  bag_or_set_enumeration_constraint(const std::vector<type_check_node_ptr>& arguments_)
    : arguments(arguments_)
  {}

  std::string print() const;
};

struct type_check_node
{
  std::vector<type_check_node_ptr> children;
  std::vector<constraint_ptr> constraints;

  type_check_node()
  {}

  type_check_node(const std::vector<type_check_node_ptr>& children_)
    : children(children_)
  {}

  // Adds constraints that apply to this node to 'constraints'
  virtual
  void set_constraints(const type_check_context& context)
  {}

  void set_children_constraints(const type_check_context& context)
  {
    for (type_check_node_ptr child: children)
    {
      child->set_constraints(context);
    }
  }

  // Throws an exception if the node violates a well typedness rule
  virtual
  void check_well_typedness(const type_check_context& context)
  {}

  virtual std::string print() const = 0;
};

struct id_node: public type_check_node
{
  std::string value;

  id_node(const std::string& value_)
    : value(value_)
  {}

  std::string print() const
  {
    return "id(" + value + ")";
  }
};

struct number_node: public type_check_node
{
  std::string value;

  number_node(const std::string& value_)
    : value(value_)
  {}

  void set_constraints(const type_check_context& context)
  {
    sort_expression sort;
    if (detail::is_pos(value))
    {
      sort = sort_pos::pos();
    }
    else if (detail::is_nat(value))
    {
      sort = sort_nat::nat();
    }
    else
    {
      throw mcrl2::runtime_error("unknown numeric string " + value);
    }
    constraints.push_back(constraint_ptr(new subsort_constraint(sort)));
  }

  std::string print() const
  {
    return "number(" + value + ")";
  }
};

struct constant_node: public type_check_node
{
  std::string name;

  constant_node(const std::string& name_)
    : name(name_)
  {}

  void set_constraints(const type_check_context& context)
  {
    auto p = context.find_matching_constants(name);
    constraints.push_back(constraint_ptr(new constant_constraint(p.first + p.second)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    return "constant(" + name + ")";
  }
};

struct true_node: public constant_node
{
  true_node()
    : constant_node("true")
  { }
};

struct false_node: public constant_node
{
  false_node()
    : constant_node("false")
  { }
};

struct empty_list_node: public constant_node
{
  empty_list_node()
    : constant_node("[]")
  { }

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new subsort_constraint(sort_list::list(untyped_sort()))));
  }
};

struct empty_set_node: public constant_node
{
  empty_set_node()
    : constant_node("{}")
  { }

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new subsort_constraint(sort_set::set_(untyped_sort()))));
  }
};

struct empty_bag_node: public constant_node
{
  empty_bag_node()
    : constant_node("{:}")
  { }

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new subsort_constraint(sort_bag::bag(untyped_sort()))));
  }
};

struct list_enumeration_node: public type_check_node
{
  list_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new list_enumeration_constraint(children)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "list_enumeration(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct bag_enumeration_node: public type_check_node
{
  bag_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new bag_enumeration_constraint(children)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "bag_enumeration(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct set_enumeration_node: public type_check_node
{
  set_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new set_enumeration_constraint(children)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "set_enumeration(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct bag_or_set_enumeration_node: public type_check_node
{
  variable v;

  bag_or_set_enumeration_node(const variable& v_, type_check_node_ptr x)
    : v(v_)
  {
    children.push_back(x);
  }

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new bag_or_set_enumeration_constraint(children)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "bag_or_set_enumeration(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct function_update_node: public type_check_node
{
  function_update_node(type_check_node_ptr x1, type_check_node_ptr x2, type_check_node_ptr x3)
  {
    children.push_back(x1);
    children.push_back(x2);
    children.push_back(x3);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "function_update(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct application_node: public type_check_node
{
  application_node(type_check_node_ptr head, const std::vector<type_check_node_ptr>& arguments)
  {
    children.push_back(head);
    for (auto arg: arguments)
    {
      children.push_back(arg);
    }
  }

  void set_constraints(const type_check_context& context)
  {
    constraints.push_back(constraint_ptr(new application_constraint(children)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "application(";
    for (auto i = children.begin(); i != children.end(); ++i)
    {
      if (i != children.begin())
      {
        out << ", ";
      }
      out << (*i)->print();
    }
    out << ")";
    return out.str();
  }
};

struct unary_operator_node: public type_check_node
{
  std::string name;

  unary_operator_node(const std::string& name_, type_check_node_ptr arg)
    : type_check_node({ arg }), name(name_)
  {}

  void set_constraints(const type_check_context& context)
  {
    auto p = context.find_matching_functions(name, 1);
    constraints.push_back(constraint_ptr(new function_constraint(p.first + p.second)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "unary_operator(" << name << ", " << children.front()->print() << ")";
    return out.str();
  }
};

struct forall_node: public unary_operator_node
{
  variable_list v;

  forall_node(const variable_list& v_, type_check_node_ptr arg)
    : unary_operator_node("forall", arg), v(v_)
  {}

  void set_constraints(const type_check_context& context)
  {
    unary_operator_node::set_constraints(context);
    children[0]->constraints.push_back(constraint_ptr(new is_sort_constraint(sort_bool::bool_())));
    // TODO: update context
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "forall(" << data::pp(v) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct exists_node: public unary_operator_node
{
  variable_list v;

  exists_node(const variable_list& v_, type_check_node_ptr arg)
    : unary_operator_node("exists", arg), v(v_)
  {}

  void set_constraints(const type_check_context& context)
  {
    unary_operator_node::set_constraints(context);
    children[0]->constraints.push_back(constraint_ptr(new is_sort_constraint(sort_bool::bool_())));
    // TODO: update context
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "exists(" << data::pp(v) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct lambda_node: public unary_operator_node
{
  variable_list v;

  lambda_node(const variable_list& v_, type_check_node_ptr arg)
    : unary_operator_node("lambda", arg), v(v_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "lambda(" << data::pp(v) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct binary_operator_node: public type_check_node
{
  std::string name;

  binary_operator_node(const std::string& name_, type_check_node_ptr left, type_check_node_ptr right)
    : type_check_node({ left, right }), name(name_)
  {}

  void set_constraints(const type_check_context& context)
  {
    auto p = context.find_matching_functions(name, 2);
    constraints.push_back(constraint_ptr(new function_constraint(p.first + p.second)));
    set_children_constraints(context);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "binary_operator(" << name << ", " << children[0]->print() << ", " << children[1]->print() << ")";
    return out.str();
  }
};

struct where_clause_node: public type_check_node
{
  std::vector<std::string> variables;

  where_clause_node(type_check_node_ptr body, const std::vector<std::pair<std::string, type_check_node_ptr> >& assignments)
  {
    children.push_back(body);
    variable_vector v;
    for (const std::pair<std::string, type_check_node_ptr>& a: assignments)
    {
      variables.push_back(a.first);
      children.push_back(a.second);
    }
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "where_clause()";
    return out.str();
  }
};

struct type_check_tree_generator: public data_expression_actions
{
  type_check_tree_generator(const core::parser& parser_)
    : data_expression_actions(parser_)
  {}

  std::vector<type_check_node_ptr> parse_DataExprList(const core::parse_node& node) const
  {
    return parse_vector<type_check_node_ptr>(node, "DataExpr", [&](const core::parse_node& node) { return parse_DataExpr(node); });
  }

  std::vector<type_check_node_ptr> parse_BagEnumEltList(const core::parse_node& node) const
  {
    return parse_DataExprList(node);
  }

  std::pair<std::string, type_check_node_ptr> parse_Assignment(const core::parse_node& node) const
  {
    return std::make_pair(parse_Id(node.child(0)), parse_DataExpr(node.child(2)));
  }

  std::vector<std::pair<std::string, type_check_node_ptr> > parse_AssignmentList(const core::parse_node& node) const
  {
    return parse_vector<std::pair<std::string, type_check_node_ptr> >(node, "Assignment", [&](const core::parse_node& node) { return parse_Assignment(node); });
  }

  type_check_node_ptr parse_DataExpr(const core::parse_node& node) const
  {
    assert(symbol_name(node) == "DataExpr");
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return type_check_node_ptr(new id_node(parse_Id(node.child(0)))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return type_check_node_ptr(new number_node(parse_Number(node.child(0)))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return type_check_node_ptr(new true_node()); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return type_check_node_ptr(new false_node()); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "]")) { return type_check_node_ptr(new empty_list_node()); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "}")) { return type_check_node_ptr(new empty_set_node()); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == ":") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new empty_bag_node()); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "]")) { return type_check_node_ptr(new list_enumeration_node(parse_DataExprList(node.child(1)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "BagEnumEltList") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new bag_enumeration_node(parse_BagEnumEltList(node.child(1)))); }
    else if ((node.child_count() == 5) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "VarDecl") && (symbol_name(node.child(2)) == "|") && (symbol_name(node.child(3)) == "DataExpr") && (symbol_name(node.child(4)) == "}")) { return type_check_node_ptr(new bag_or_set_enumeration_node(parse_VarDecl(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new set_enumeration_node(parse_DataExprList(node.child(1)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "[") && (symbol_name(node.child(2)) == "DataExpr") && (symbol_name(node.child(3)) == "->") && (symbol_name(node.child(4)) == "DataExpr") && (symbol_name(node.child(5)) == "]")) { return type_check_node_ptr(new function_update_node(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return type_check_node_ptr(new application_node(parse_DataExpr(node.child(0)), parse_DataExprList(node.child(2)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("!", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("-", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("#", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new forall_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new exists_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "lambda") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new lambda_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "=>" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("=>" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "&&" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("&&" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "||" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("||" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "==" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("==" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "!=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("!=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node(">=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node(">"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "in" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("in" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "|>" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("|>" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<|" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<|" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "++" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("++" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "+"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("+"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "-"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("-"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "/"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("/"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "div") && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("div", parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "mod") && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("mod", parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "*"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("*"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "."  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("."  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "whr") && (symbol_name(node.child(2)) == "AssignmentList") && (symbol_name(node.child(3)) == "end")) { return type_check_node_ptr(new where_clause_node({ parse_DataExpr(node.child(0)) }, parse_AssignmentList(node.child(2)))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

inline
std::pair<sort_expression_list, sort_expression_list> type_check_context::find_matching_constants(const std::string& name) const
{
  sort_expression_list system_result;
  sort_expression_list user_result;
  auto i = system_constants.find(core::identifier_string(name));
  if (i != system_constants.end())
  {
    system_result = i->second;
  }
  auto j = user_constants.find(core::identifier_string(name));
  if (j != user_constants.end())
  {
    user_result = atermpp::make_list<sort_expression>(j->second);
  }
  return std::make_pair(system_result, user_result);
}

inline
function_sort_list filter_sorts(const function_sort_list& sorts, std::size_t arity)
{
  std::vector<function_sort> result;
  for (const function_sort& sort: sorts)
  {
    if (sort.domain().size() == arity)
    {
      result.push_back(sort);
    }
  }
  return function_sort_list(result.begin(), result.end());
}

inline
std::pair<function_sort_list, function_sort_list> type_check_context::find_matching_functions(const std::string& name, std::size_t arity) const
{
  function_sort_list system_result;
  function_sort_list user_result;
  auto i = system_functions.find(core::identifier_string(name));
  if (i != system_functions.end())
  {
    system_result = filter_sorts(i->second, arity);
  }
  auto j = user_functions.find(core::identifier_string(name));
  if (j != user_functions.end())
  {
    user_result = filter_sorts(j->second, arity);
  }
  return std::make_pair(system_result, user_result);
}

inline
std::vector<sort_expression> type_check_context::find_matching_variable(const std::string& name) const
{
  std::vector<sort_expression> result;
  auto i = declared_variables.find(core::identifier_string(name));
  if (i != declared_variables.end())
  {
    result.push_back(i->second);
  }
  return result;
}

inline
void print_node(type_check_node_ptr node)
{
  std::cout << "\n-----------------------------" << std::endl;
  std::cout << "node = " << node->print() << std::endl;
  std::cout << "  --- constraints ---" << std::endl;
  for (constraint_ptr constraint: node->constraints)
  {
    std::cout << "  " << constraint->print() << std::endl;
  }
  for (type_check_node_ptr child: node->children)
  {
    print_node(child);
  }
}

inline
std::string application_constraint::print() const
{
  std::ostringstream out;
  out << "application_constraint(";
  out << head->print() << ", ";
  for (auto i = arguments.begin(); i != arguments.end(); ++i)
  {
    if (i != arguments.begin())
    {
      out << ", ";
    }
    out << (*i)->print();
  }
  return out.str();
}

inline
std::string list_enumeration_constraint::print() const
{
  std::ostringstream out;
  out << "list_enumeration_constraint(";
  for (auto i = arguments.begin(); i != arguments.end(); ++i)
  {
    if (i != arguments.begin())
    {
      out << ", ";
    }
    out << (*i)->print();
  }
  return out.str();
}

inline
std::string set_enumeration_constraint::print() const
{
  std::ostringstream out;
  out << "set_enumeration_constraint(";
  for (auto i = arguments.begin(); i != arguments.end(); ++i)
  {
    if (i != arguments.begin())
    {
      out << ", ";
    }
    out << (*i)->print();
  }
  return out.str();
}

inline
std::string bag_enumeration_constraint::print() const
{
  std::ostringstream out;
  out << "bag_enumeration_constraint(";
  for (auto i = arguments.begin(); i != arguments.end(); ++i)
  {
    if (i != arguments.begin())
    {
      out << ", ";
    }
    out << (*i)->print();
  }
  return out.str();
}

inline
std::string bag_or_set_enumeration_constraint::print() const
{
  std::ostringstream out;
  out << "bag_or_set_enumeration_constraint(";
  for (auto i = arguments.begin(); i != arguments.end(); ++i)
  {
    if (i != arguments.begin())
    {
      out << ", ";
    }
    out << (*i)->print();
  }
  return out.str();
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TYPE_CHECK_TREE_H
