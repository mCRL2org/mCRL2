#include <iterator>
#include <sstream>
#include <boost/bind.hpp>
#include "parser.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/identifier.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/unknown_sort.h"

namespace mcrl2 {

namespace data {

using namespace dparser;

struct default_actions
{
  const parser_table& table;

  default_actions(const parser_table& table_)
    : table(table_)
  {}

  // starts a traversal in node, and calls the function f to each subnode of the given type
  template <typename Function>
  void traverse(const parse_node& node, Function f)
  {
    if (!node)
    {
      return;
    }
    if (!f(node))
    {
      for (int i = 0; i < node.child_count(); i++)
      {
        traverse(node.child(i), f);
      }
    }
  }

  // callback function that applies a function to a node, and adds the result to a container
  template <typename Container, typename Function>
  struct collector
  {
    const parser_table& table;
    const std::string& type;
    Container& container;
    Function f;

    collector(const parser_table& table_, const std::string& type_, Container& container_, Function f_)
      : table(table_),
        type(type_),
        container(container_),
        f(f_)
    {}

    bool operator()(const parse_node& node) const
    {
      if (table.symbol_name(node) == type)
      {
        container.push_back(f(node));
        return true;
      }
      return false;
    }
  };

  template <typename Container, typename Function>
  collector<Container, Function> make_collector(const parser_table& table, const std::string& type, Container& container, Function f)
  {
    return collector<Container, Function>(table, type, container, f);
  }

  std::string symbol_name(const parse_node& node) const
  {
    return table.symbol_name(node.symbol());
  }

  std::string print_node(const parse_node& node)
  {
    std::ostringstream out;
    out << "symbol      = " << symbol_name(node) << std::endl;
    out << "string      = " << node.string() << std::endl;
    out << "child_count = " << node.child_count() << std::endl;
    for (int i = 0; i < node.child_count(); i++)
    {
      out << "child " << i << " = " << symbol_name(node.child(i)) << " " << node.child(i).string() << std::endl;
    }
    return out.str();
  }

  void report_unexpected_node(const parse_node& node)
  {
    std::cout << "--- unexpected node ---\n" << print_node(node);
    throw mcrl2::runtime_error("unexpected node detected!");
  }

  template <typename T, typename Function>
  atermpp::term_list<T> parse_list(const parse_node& node, const std::string& type, Function f)
  {
    atermpp::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return atermpp::term_list<T>(result.begin(), result.end());
  }

  core::identifier_string parse_Id(const parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string parse_Number(const parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string_list parse_IdList(const parse_node& node)
  {
    return parse_list<core::identifier_string>(node, "Id", boost::bind(&default_actions::parse_Id, this, _1));
  }
};

struct sort_expression_actions: public default_actions
{
  sort_expression_actions(const parser_table& table_)
    : default_actions(table_)
  {}

  data::sort_expression parse_SortExpr(const parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Bool")) { return sort_bool::bool_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Pos")) { return sort_pos::pos(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Nat")) { return sort_nat::nat(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Int")) { return sort_int::int_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Real")) { return sort_real::real_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "List") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_list::list(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Set") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_set::set_(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Bag") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_bag::bag(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return basic_sort(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "SortExpr") && (symbol_name(node.child(2)) == ")")) { return parse_SortExpr(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "SortExprList") && (symbol_name(node.child(1)) == "->") && (symbol_name(node.child(2)) == "SortExpr")) { return function_sort(parse_SortExprList(node.child(0)), parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "struct") && (symbol_name(node.child(1)) == "ConstrDeclList")) { return structured_sort(parse_ConstrDeclList(node.child(1))); }
    report_unexpected_node(node);
    return data::sort_expression();
  }

  data::sort_expression_list parse_SortExprList(const parse_node& node)
  {
    return parse_list<data::sort_expression>(node, "SortExpr", boost::bind(&sort_expression_actions::parse_SortExpr, this, _1));
  }

  data::structured_sort_constructor parse_ConstrDecl(const parse_node& node)
  {
    core::identifier_string name = parse_Id(node.child(0));
    data::structured_sort_constructor_argument_list arguments;
    core::identifier_string recogniser = no_identifier();
    if (node.child(1))
    {
      arguments = parse_ProjDeclList(node.child(1).child(1));
    }
    if (node.child(2))
    {
      parse_node u = node.child(2);
      if (u.child(0))
      {
        recogniser = parse_Id(node.child(2).child(0).child(1));
      }
    }
    return structured_sort_constructor(name, arguments, recogniser);
  }

  data::structured_sort_constructor_list parse_ConstrDeclList(const parse_node& node)
  {
    return parse_list<data::structured_sort_constructor>(node, "ConstrDecl", boost::bind(&sort_expression_actions::parse_ConstrDecl, this, _1));
  }

  data::structured_sort_constructor_argument parse_ProjDecl(const parse_node& node)
  {
    core::identifier_string name = no_identifier();
    sort_expression sort = parse_SortExpr(node.child(1));
    if (node.child(0))
    {
      name = parse_Id(node.child(0));
    }
    return structured_sort_constructor_argument(name, sort);
  }

  data::structured_sort_constructor_argument_list parse_ProjDeclList(const parse_node& node)
  {
    return parse_list<data::structured_sort_constructor_argument>(node, "ProjDecl", boost::bind(&sort_expression_actions::parse_ProjDecl, this, _1));
  }
};

struct data_expression_actions: public sort_expression_actions
{
  data_expression_actions(const parser_table& table_)
    : sort_expression_actions(table_)
  {}

  data_expression make_set_or_bag_comprehension(const variable& v, const data_expression& x)
  {
    return abstraction(set_or_bag_comprehension_binder(), atermpp::make_list(v), x);
  }

  data_expression make_list_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return application(identifier(sort_list::list_enumeration_name()), x);
  }

  data_expression make_set_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return application(identifier(sort_set::set_enumeration_name()), x);
  }

  data_expression make_bag_enumeration(const data_expression_list& x)
  {
    assert(!x.empty());
    return application(identifier(sort_bag::bag_enumeration_name()), x);
  }

  data_expression make_function_update(const data_expression& x, const data_expression& y, const data_expression& z)
  {
    return make_application(identifier(mcrl2::data::function_update_name()), x, y, z);
  }

  data::variable parse_VarDecl(const parse_node& node)
  {
    return variable(parse_Id(node.child(0)), parse_SortExpr(node.child(2)));
  }

  bool callback_VarsDecl(const parse_node& node, variable_vector& result)
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

  data::variable_list parse_VarsDeclList(const parse_node& node)
  {
    variable_vector result;
    traverse(node, boost::bind(&data_expression_actions::callback_VarsDecl, this, _1, result));
    return data::variable_list(result.begin(), result.end());
  }

  data::data_expression parse_DataExpr(const parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return identifier(parse_Number(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "[]")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "{}")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "]")) { return make_list_enumeration(parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "BagEnumEltList") && (symbol_name(node.child(2)) == "}")) { return make_bag_enumeration(parse_BagEnumEltList(node.child(1))); }
    else if ((node.child_count() == 5) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "VarDecl") && (symbol_name(node.child(2)) == "|") && (symbol_name(node.child(3)) == "DataExpr") && (symbol_name(node.child(4)) == "}")) { return make_set_or_bag_comprehension(parse_VarDecl(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "}")) { return make_set_enumeration(parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "[") && (symbol_name(node.child(2)) == "DataExpr") && (symbol_name(node.child(3)) == "->") && (symbol_name(node.child(4)) == "DataExpr") && (symbol_name(node.child(5)) == "]")) { return make_function_update(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return application(parse_DataExpr(node.child(0)), parse_DataExprList(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExpr(node.child(1))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return forall(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return exists(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "lambda") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return lambda(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "==") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "!=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "<") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "<=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == ">=") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == ">") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "in") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "|>") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "<|") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "++") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "+") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "-") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "/") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "div") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "mod") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "*") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == ".") && (symbol_name(node.child(2)) == "DataExpr")) { return make_application(identifier(parse_Id(node.child(1))), parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "whr") && (symbol_name(node.child(2)) == "WhrExprList") && (symbol_name(node.child(3)) == "end")) { return where_clause(parse_DataExpr(node.child(0)), parse_WhrExprList(node.child(2))); }
    report_unexpected_node(node);
    return data::data_expression();
  }

  data::data_expression parse_DataExprUnit(const parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return identifier(parse_Number(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return identifier(parse_Id(node.child(0))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return application(parse_DataExprUnit(node.child(0)), parse_DataExprList(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExprUnit")) { return make_application(identifier(parse_Id(node.child(0))), parse_DataExprUnit(node.child(1))); }
    report_unexpected_node(node);
    return data::data_expression();
  }

  data::identifier_assignment parse_WhrExpr(const parse_node& node)
  {
    return identifier_assignment(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)));
  }

  data::identifier_assignment_list parse_WhrExprList(const parse_node& node)
  {
    return parse_list<data::identifier_assignment>(node, "WhrExpr", boost::bind(&data_expression_actions::parse_WhrExpr, this, _1));
  }

  data::data_expression_list parse_DataExprList(const parse_node& node)
  {
    return parse_list<data::data_expression>(node, "DataExpr", boost::bind(&data_expression_actions::parse_DataExpr, this, _1));
  }

  data::data_expression_list parse_BagEnumEltList(const parse_node& node)
  {
    return parse_DataExprList(node);
  }
};

struct data_specification_actions: public data_expression_actions
{
  data_specification_actions(const parser_table& table_)
    : data_expression_actions(table_)
  {}

  bool callback_SortDecl(const parse_node& node, atermpp::vector<atermpp::aterm_appl>& result)
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

  atermpp::vector<atermpp::aterm_appl> parse_SortDeclList(const parse_node& node)
  {
    atermpp::vector<atermpp::aterm_appl> result;
    traverse(node, boost::bind(&data_specification_actions::callback_SortDecl, this, _1, result));
    return result;
  }

  atermpp::vector<atermpp::aterm_appl> parse_SortSpec(const parse_node& node)
  {
    return parse_SortDeclList(node.child(1));
  }

  bool callback_IdsDecl(const parse_node& node, function_symbol_vector& result)
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

  data::function_symbol_vector parse_IdsDeclList(const parse_node& node)
  {
    function_symbol_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_IdsDecl, this, _1, result));
    return result;
  }

  data::function_symbol_vector parse_ConsSpec(const parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::function_symbol_vector parse_MapSpec(const parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::variable_list parse_GlobVarSpec(const parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  data::variable_list parse_VarSpec(const parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  bool callback_EqnDecl(const parse_node& node, const variable_list& variables, data_equation_vector& result)
  {
    if (symbol_name(node) == "EqnDecl")
    {
      result.push_back(data_equation(variables, parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4))));
      return true;
    }
    return false;
  };

  data::data_equation_vector parse_EqnDeclList(const parse_node& node, const variable_list& variables)
  {
    data_equation_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_EqnDecl, this, _1, variables, result));
    return result;
  }

  data::data_equation_vector parse_EqnSpec(const parse_node& node)
  {
    variable_list variables = parse_VarSpec(node.child(0));
    return parse_EqnDeclList(node.child(2), variables);
  }

  bool callback_DataSpecElement(const parse_node& node, data_specification& result)
  {
    if (symbol_name(node) == "SortSpec")
    {
      atermpp::vector<atermpp::aterm_appl> v = parse_SortSpec(node);
      for (atermpp::vector<atermpp::aterm_appl>::iterator i = v.begin(); i != v.end(); ++i)
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
      data_equation_vector v = parse_EqnSpec(node.child(0));
      for (data_equation_vector::iterator i = v.begin(); i != v.end(); ++i)
      {
        result.add_equation(*i);
      }
      return true;
    }
    return false;
  }

  data::data_specification parse_DataSpec(const parse_node& node)
  {
    data_specification result;
    traverse(node, boost::bind(&data_specification_actions::callback_DataSpecElement, this, _1, result));
    return result;
  }
};

} // namespace data

} // namespace mcrl2
