#include <iterator>
#include <sstream>
#include <boost/bind.hpp>
#include "parser.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/identifier.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace data {

struct default_actions
{
  const dparser::parser_table& table;

  default_actions(const dparser::parser_table& table_)
    : table(table_)
  {}

  // starts a traversal in node, and calls the function f to each subnode of the given type
  template <typename Function>
  void traverse(const dparser::parse_node& node, Function f)
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
    const dparser::parser_table& table;
    const std::string& type;
    Container& container;
    Function f;

    collector(const dparser::parser_table& table_, const std::string& type_, Container& container_, Function f_)
      : table(table_),
        type(type_),
        container(container_),
        f(f_)
    {}

    bool operator()(const dparser::parse_node& node) const
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
  collector<Container, Function> make_collector(const dparser::parser_table& table, const std::string& type, Container& container, Function f)
  {
    return collector<Container, Function>(table, type, container, f);
  }

  std::string symbol_name(const dparser::parse_node& node) const
  {
    return table.symbol_name(node.symbol());
  }

  std::string print_node(const dparser::parse_node& node)
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

  void report_unexpected_node(const dparser::parse_node& node)
  {
    std::cout << "--- unexpected node ---\n" << print_node(node);
    throw mcrl2::runtime_error("unexpected node detected!");
  }

  template <typename T, typename Function>
  atermpp::term_list<T> parse_list(const dparser::parse_node& node, const std::string& type, Function f)
  {
    atermpp::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return atermpp::term_list<T>(result.begin(), result.end());
  }

  template <typename T, typename Function>
  atermpp::vector<T> parse_vector(const dparser::parse_node& node, const std::string& type, Function f)
  {
    atermpp::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return result;
  }

  core::identifier_string parse_Id(const dparser::parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string parse_Number(const dparser::parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string_list parse_IdList(const dparser::parse_node& node)
  {
    return parse_list<core::identifier_string>(node, "Id", boost::bind(&default_actions::parse_Id, this, _1));
  }
};

struct sort_expression_actions: public default_actions
{
  sort_expression_actions(const dparser::parser_table& table_)
    : default_actions(table_)
  {}

  data::sort_expression parse_SortExpr(const dparser::parse_node& node)
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

  data::sort_expression_list parse_SortExprList(const dparser::parse_node& node)
  {
    return parse_list<data::sort_expression>(node, "SortExpr", boost::bind(&sort_expression_actions::parse_SortExpr, this, _1));
  }

  data::structured_sort_constructor parse_ConstrDecl(const dparser::parse_node& node)
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
      dparser::parse_node u = node.child(2);
      if (u.child(0))
      {
        recogniser = parse_Id(node.child(2).child(0).child(1));
      }
    }
    return structured_sort_constructor(name, arguments, recogniser);
  }

  data::structured_sort_constructor_list parse_ConstrDeclList(const dparser::parse_node& node)
  {
    return parse_list<data::structured_sort_constructor>(node, "ConstrDecl", boost::bind(&sort_expression_actions::parse_ConstrDecl, this, _1));
  }

  data::structured_sort_constructor_argument parse_ProjDecl(const dparser::parse_node& node)
  {
    core::identifier_string name = no_identifier();
    sort_expression sort = parse_SortExpr(node.child(1));
    if (node.child(0))
    {
      name = parse_Id(node.child(0));
    }
    return structured_sort_constructor_argument(name, sort);
  }

  data::structured_sort_constructor_argument_list parse_ProjDeclList(const dparser::parse_node& node)
  {
    return parse_list<data::structured_sort_constructor_argument>(node, "ProjDecl", boost::bind(&sort_expression_actions::parse_ProjDecl, this, _1));
  }
};

struct data_expression_actions: public sort_expression_actions
{
  data_expression_actions(const dparser::parser_table& table_)
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

  data::variable parse_VarDecl(const dparser::parse_node& node)
  {
    return variable(parse_Id(node.child(0)), parse_SortExpr(node.child(2)));
  }

  bool callback_VarsDecl(const dparser::parse_node& node, variable_vector& result)
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

  data::variable_list parse_VarsDeclList(const dparser::parse_node& node)
  {
    variable_vector result;
    traverse(node, boost::bind(&data_expression_actions::callback_VarsDecl, this, _1, boost::ref(result)));
    return data::variable_list(result.begin(), result.end());
  }

  data::data_expression parse_DataExpr(const dparser::parse_node& node)
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

  data::data_expression parse_DataExprUnit(const dparser::parse_node& node)
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

  data::data_expression parse_DataValExpr(const dparser::parse_node& node)
  {
    return parse_DataExpr(node.child(2));
  }

  data::identifier_assignment parse_WhrExpr(const dparser::parse_node& node)
  {
    return identifier_assignment(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)));
  }

  data::identifier_assignment_list parse_WhrExprList(const dparser::parse_node& node)
  {
    return parse_list<data::identifier_assignment>(node, "WhrExpr", boost::bind(&data_expression_actions::parse_WhrExpr, this, _1));
  }

  data::data_expression_list parse_DataExprList(const dparser::parse_node& node)
  {
    return parse_list<data::data_expression>(node, "DataExpr", boost::bind(&data_expression_actions::parse_DataExpr, this, _1));
  }

  data::data_expression_list parse_BagEnumEltList(const dparser::parse_node& node)
  {
    return parse_DataExprList(node);
  }
};

struct data_specification_actions: public data_expression_actions
{
  data_specification_actions(const dparser::parser_table& table_)
    : data_expression_actions(table_)
  {}

  bool callback_SortDecl(const dparser::parse_node& node, atermpp::vector<atermpp::aterm_appl>& result)
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

  atermpp::vector<atermpp::aterm_appl> parse_SortDeclList(const dparser::parse_node& node)
  {
    atermpp::vector<atermpp::aterm_appl> result;
    traverse(node, boost::bind(&data_specification_actions::callback_SortDecl, this, _1, boost::ref(result)));
    return result;
  }

  atermpp::vector<atermpp::aterm_appl> parse_SortSpec(const dparser::parse_node& node)
  {
    return parse_SortDeclList(node.child(1));
  }

  bool callback_IdsDecl(const dparser::parse_node& node, function_symbol_vector& result)
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

  data::function_symbol_vector parse_IdsDeclList(const dparser::parse_node& node)
  {
    function_symbol_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_IdsDecl, this, _1, boost::ref(result)));
    return result;
  }

  data::function_symbol_vector parse_ConsSpec(const dparser::parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::function_symbol_vector parse_MapSpec(const dparser::parse_node& node)
  {
    return parse_IdsDeclList(node);
  }

  data::variable_list parse_GlobVarSpec(const dparser::parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  data::variable_list parse_VarSpec(const dparser::parse_node& node)
  {
    return parse_VarsDeclList(node);
  }

  bool callback_EqnDecl(const dparser::parse_node& node, const variable_list& variables, data_equation_vector& result)
  {
    if (symbol_name(node) == "EqnDecl")
    {
      result.push_back(data_equation(variables, parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4))));
      return true;
    }
    return false;
  };

  data::data_equation_vector parse_EqnDeclList(const dparser::parse_node& node, const variable_list& variables)
  {
    data_equation_vector result;
    traverse(node, boost::bind(&data_specification_actions::callback_EqnDecl, this, _1, boost::ref(variables), boost::ref(result)));
    return result;
  }

  data::data_equation_vector parse_EqnSpec(const dparser::parse_node& node)
  {
    variable_list variables = parse_VarSpec(node.child(0));
    return parse_EqnDeclList(node.child(2), variables);
  }

  bool callback_DataSpecElement(const dparser::parse_node& node, data_specification& result)
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

  data::data_specification parse_DataSpec(const dparser::parse_node& node)
  {
    data_specification result;
    traverse(node, boost::bind(&data_specification_actions::callback_DataSpecElement, this, _1, boost::ref(result)));
    return result;
  }
};

} // namespace data

namespace lps {

struct action_actions: public data::data_specification_actions
{
  action_actions(const dparser::parser_table& table_)
    : data::data_specification_actions(table_)
  {}

  lps::action parse_Action(const dparser::parse_node& node)
  {
    return action(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  lps::action_list parse_ActionList(const dparser::parse_node& node)
  {
    return parse_list<lps::action_label>(node, "Action", boost::bind(&action_actions::parse_Action, this, _1));
  }

  bool callback_ActDecl(const dparser::parse_node& node, action_label_vector& result)
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

  lps::action_label_list parse_ActDeclList(const dparser::parse_node& node)
  {
    action_label_vector result;
    traverse(node, boost::bind(&action_actions::callback_ActDecl, this, _1, boost::ref(result)));
    return lps::action_label_list(result.begin(), result.end());
  }

  lps::action_label_list parse_ActSpec(const dparser::parse_node& node)
  {
    return parse_ActDeclList(node.child(1));
  }

  lps::multi_action parse_MultAct(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return lps::multi_action(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActionList")) { return lps::multi_action(parse_ActionList(node.child(0))); }
    report_unexpected_node(node);
    return lps::action_list();
  }
};

} // namespace lps

namespace process {

struct process_actions: public lps::action_actions
{
  process_actions(const dparser::parser_table& table_)
    : lps::action_actions(table_)
  {}

  core::identifier_string_list parse_ActIdSet(const dparser::parse_node& node)
  {
    return parse_IdList(node.child(1));
  }

  process::action_name_multiset parse_MultActId(const dparser::parse_node& node)
  {
    return action_name_multiset(parse_IdList(node));
  }

  process::action_name_multiset_list parse_MultActIdList(const dparser::parse_node& node)
  {
    return parse_list<process::action_name_multiset>(node, "MultActId", boost::bind(&process_actions::parse_MultActId, this, _1));
  }

  process::action_name_multiset_list parse_MultActIdSet(const dparser::parse_node& node)
  {
    return parse_MultActIdList(node.child(1));
  }

  core::identifier_string parse_CommExprRhs(const dparser::parse_node& node)
  {
    // TODO: get rid of this 'nil'
    core::identifier_string result = core::detail::gsMakeNil();
    if (node.child(1))
    {
      result = parse_Id(node.child(1));
    }
    return result;
  }

  process::communication_expression parse_CommExpr(const dparser::parse_node& node)
  {
    core::identifier_string id = parse_Id(node.child(0));
    core::identifier_string_list ids = parse_IdList(node.child(2));
    action_name_multiset lhs(atermpp::push_front(ids, id));
    core::identifier_string rhs = parse_CommExprRhs(node.child(3));
    return process::communication_expression(lhs, rhs);
  }

  process::communication_expression_list parse_CommExprList(const dparser::parse_node& node)
  {
    return parse_list<process::communication_expression>(node, "CommExpr", boost::bind(&process_actions::parse_CommExpr, this, _1));
  }

  process::communication_expression_list parse_CommExprSet(const dparser::parse_node& node)
  {
    return parse_CommExprList(node.child(1));
  }

  process::rename_expression parse_RenExpr(const dparser::parse_node& node)
  {
    return process::rename_expression(parse_Id(node.child(0)), parse_Id(node.child(2)));
  }

  process::rename_expression_list parse_RenExprList(const dparser::parse_node& node)
  {
    return parse_list<process::rename_expression>(node, "RenExpr", boost::bind(&process_actions::parse_RenExpr, this, _1));
  }

  process::rename_expression_list parse_RenExprSet(const dparser::parse_node& node)
  {
    return parse_RenExprList(node.child(1));
  }

  process::process_expression parse_ProcExpr(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Action")) { return parse_Action(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "delta")) { return delta(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "tau")) { return tau(); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "block") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return block(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "allow") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "MultActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return allow(parse_MultActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "hide") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "ActIdSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return hide(parse_ActIdSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "rename") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "RenExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return rename(parse_RenExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "comm") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "CommExprSet") && (symbol_name(node.child(3)) == ",") && (symbol_name(node.child(4)) == "ProcExpr") && (symbol_name(node.child(5)) == ")")) { return comm(parse_CommExprSet(node.child(2)), parse_ProcExpr(node.child(4))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ProcExpr") && (symbol_name(node.child(2)) == ")")) { return parse_ProcExpr(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "+") && (symbol_name(node.child(2)) == "ProcExpr")) { return choice(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "ProcExpr")) { return merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "||_") && (symbol_name(node.child(2)) == "ProcExpr")) { return left_merge(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == ".") && (symbol_name(node.child(2)) == "ProcExpr")) { return seq(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "<<") && (symbol_name(node.child(2)) == "ProcExpr")) { return bounded_init(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "@") && (symbol_name(node.child(2)) == "DataExprUnit")) { return at(parse_ProcExpr(node.child(0)), parse_DataExprUnit(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ProcExpr") && (symbol_name(node.child(1)) == "|") && (symbol_name(node.child(2)) == "ProcExpr")) { return sync(parse_ProcExpr(node.child(0)), parse_ProcExpr(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "DataExprUnit") && (symbol_name(node.child(1)) == "ProcExprThenElse"))
    {
      data::data_expression condition = parse_DataExprUnit(node.child(0));
      dparser::parse_node u = node.child(1);
      process_expression x1 = parse_ProcExpr(u.child(1));
      if (u.child(2) && u.child(2).child(1))
      {
        process_expression x2 = parse_ProcExpr(u.child(2).child(1));
        return if_then_else(condition, x1, x2);
      }
      return if_then(condition, x1);
    }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "sum") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ProcExpr")) { return sum(parse_VarsDeclList(node.child(1)), parse_ProcExpr(node.child(3))); }
    report_unexpected_node(node);
    return process::process_expression();
  }

  process::process_equation parse_ProcDecl(const dparser::parse_node& node)
  {
    return process::process_equation(parse_Id(node.child(0)), parse_VarsDeclList(node.child(1)), parse_ProcExpr(node.child(3)));
  }

  process::process_equation_list parse_ProcDeclList(const dparser::parse_node& node)
  {
    return parse_list<process::process_equation>(node, "ProcDecl", boost::bind(&process_actions::parse_ProcDecl, this, _1));
  }

  process::process_equation_list parse_ProcSpec(const dparser::parse_node& node)
  {
    return parse_ProcDeclList(node.child(1));
  }

  process::process_expression parse_Init(const dparser::parse_node& node)
  {
    return parse_ProcExpr(node.child(1));
  }

  bool callback_mCRL2Spec(const dparser::parse_node& node, process::process_specification& result)
  {
    if (symbol_name(node) == "SortSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "ConsSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "MapSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "EqnSpec")
    {
      return callback_DataSpecElement(node, result.data());
    }
    else if (symbol_name(node) == "GlobVarSpec")
    {
      data::variable_list variables = parse_GlobVarSpec(node);
      result.global_variables() = atermpp::set<data::variable>(variables.begin(), variables.end());
      return true;
    }
    else if (symbol_name(node) == "ActSpec")
    {
      result.action_labels() = parse_ActSpec(node);
      return true;
    }
    else if (symbol_name(node) == "ProcSpec")
    {
      process::process_equation_list eqn = parse_ProcSpec(node);
      result.equations() = atermpp::vector<process_equation>(eqn.begin(), eqn.end());
      return true;
    }
    else if (symbol_name(node) == "Init")
    {
      result.init() = parse_Init(node);
    }
    return false;
  }

  process::process_specification parse_mCRL2Spec(const dparser::parse_node& node)
  {
    process::process_specification result;
    traverse(node, boost::bind(&process_actions::callback_mCRL2Spec, this, _1, boost::ref(result)));
    return result;
  }
};

} // namespace process

namespace bes {

struct bes_actions: public data::default_actions
{
  bes_actions(const dparser::parser_table& table_)
    : data::default_actions(table_)
  {}

  bes::boolean_expression parse_BesExpr(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return bes::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return bes::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "BesExpr")) { return bes::not_(parse_BesExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::imp(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::and_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "BesExpr") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "BesExpr")) { return bes::or_(parse_BesExpr(node.child(0)), parse_BesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "BesExpr") && (symbol_name(node.child(2)) == ")")) { return parse_BesExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "BesVar")) { return parse_BesVar(node.child(0)); }
    report_unexpected_node(node);
    return bes::boolean_expression();
  }

  bes::boolean_variable parse_BesVar(const dparser::parse_node& node)
  {
    return bes::boolean_variable(parse_Id(node.child(0)));
  }

  fixpoint_symbol parse_FixedPointOperator(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    report_unexpected_node(node);
    return pbes_system::fixpoint_symbol();
  }

  bes::boolean_equation parse_BesEqnDecl(const dparser::parse_node& node)
  {
    return bes::boolean_equation(parse_FixedPointOperator(node.child(0)), parse_BesVar(node.child(1)), parse_BesExpr(node.child(3)));
  }

  atermpp::vector<boolean_equation> parse_BesEqnSpec(const dparser::parse_node& node)
  {
    return parse_BesEqnDeclList(node.child(1));
  }

  bes::boolean_variable parse_BesInit(const dparser::parse_node& node)
  {
    return parse_BesVar(node.child(1));
  }

  bes::boolean_equation_system<> parse_BesSpec(const dparser::parse_node& node)
  {
    return bes::boolean_equation_system<>(parse_BesEqnSpec(node.child(0)), parse_BesInit(node.child(1)));
  }

  atermpp::vector<boolean_equation> parse_BesEqnDeclList(const dparser::parse_node& node)
  {
    return parse_vector<bes::boolean_equation>(node, "BesEqnDecl", boost::bind(&bes_actions::parse_BesEqnDecl, this, _1));
  }
};

} // namespace bes

namespace pbes_system {

struct pbes_actions: public data::data_specification_actions
{
  pbes_actions(const dparser::parser_table& table_)
    : data::data_specification_actions(table_)
  {}

  pbes_system::pbes_expression parse_PbesExpr(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return pbes_system::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return pbes_system::false_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PbesExpr")) { return pbes_system::forall(parse_VarsDeclList(node.child(1)), parse_PbesExpr(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "PbesExpr")) { return pbes_system::exists(parse_VarsDeclList(node.child(1)), parse_PbesExpr(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "PbesExpr")) { return pbes_system::not_(parse_PbesExpr(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::imp(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::and_(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "PbesExpr") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "PbesExpr")) { return pbes_system::or_(parse_PbesExpr(node.child(0)), parse_PbesExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "PbesExpr") && (symbol_name(node.child(2)) == ")")) { return parse_PbesExpr(node.child(1)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "PropVarInst")) { return parse_PropVarInst(node.child(0)); }
    report_unexpected_node(node);
    return pbes_system::pbes_expression();
  }

  pbes_system::propositional_variable parse_PropVarDecl(const dparser::parse_node& node)
  {
    return pbes_system::propositional_variable(parse_Id(node.child(0)), parse_VarsDeclList(node.child(1)));
  }

  pbes_system::propositional_variable_instantiation parse_PropVarInst(const dparser::parse_node& node)
  {
    return pbes_system::propositional_variable_instantiation(parse_Id(node.child(0)), parse_DataExprList(node.child(1)));
  }

  pbes_system::propositional_variable_instantiation parse_PbesInit(const dparser::parse_node& node)
  {
    return parse_PropVarInst(node.child(1));
  }

  pbes_system::fixpoint_symbol parse_FixedPointOperator(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "mu")) { return fixpoint_symbol::mu(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nu")) { return fixpoint_symbol::nu(); }
    report_unexpected_node(node);
    return pbes_system::fixpoint_symbol();
  }

  pbes_equation parse_PbesEqnDecl(const dparser::parse_node& node)
  {
    return pbes_equation(parse_FixedPointOperator(node.child(0)), parse_PropVarDecl(node.child(1)), parse_PbesExpr(node.child(3)));
  }

  atermpp::vector<pbes_equation> parse_PbesEqnDeclList(const dparser::parse_node& node)
  {
    return parse_vector<pbes_equation>(node, "PbesEqnDecl", boost::bind(&pbes_actions::parse_PbesEqnDecl, this, _1));
  }

  atermpp::vector<pbes_equation> parse_PbesEqnSpec(const dparser::parse_node& node)
  {
    return parse_PbesEqnDeclList(node.child(1));
  }

  pbes_system::pbes<> parse_PbesSpec(const dparser::parse_node& node)
  {
    return pbes<>(parse_DataSpec(node.child(0)), parse_PbesEqnSpec(node.child(2)), atermpp::convert<atermpp::set<data::variable> >(parse_GlobVarSpec(node.child(1))), parse_PbesInit(node.child(3)));
  }
};

} // namespace pbes_system

namespace action_formulas {

struct action_formula_actions: public lps::action_actions
{
  action_formula_actions(const dparser::parser_table& table_)
    : lps::action_actions(table_)
  {}

  action_formulas::action_formula parse_ActFrm(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "MultAct")) { return parse_MultAct(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return action_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return action_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "ActFrm")) { return action_formulas::not_(parse_ActFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::imp(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::and_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "ActFrm")) { return action_formulas::or_(parse_ActFrm(node.child(0)), parse_ActFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::forall(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "ActFrm")) { return action_formulas::exists(parse_VarsDeclList(node.child(1)), parse_ActFrm(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "ActFrm") && (symbol_name(node.child(1)) == "@") && (symbol_name(node.child(2)) == "DataExpr")) { return action_formulas::at(parse_ActFrm(node.child(0)), parse_DataExpr(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "ActFrm") && (symbol_name(node.child(2)) == ")")) { return parse_ActFrm(node.child(1)); }
    report_unexpected_node(node);
    return action_formulas::action_formula();
  }
};

} // namespace action_formulas

namespace regular_formulas {

struct regular_formula_actions: public action_formulas::action_formula_actions
{
  regular_formula_actions(const dparser::parser_table& table_)
    : action_formulas::action_formula_actions(table_)
  {}

  regular_formulas::regular_formula parse_RegFrm(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "ActFrm")) { return parse_ActFrm(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "nil")) { return regular_formulas::nil(); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ")")) { return parse_RegFrm(node.child(1)); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "*") && (symbol_name(node.child(1)) == "RegFrm")) { return trans_or_nil(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "+") && (symbol_name(node.child(1)) == "RegFrm")) { return trans(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == ".") && (symbol_name(node.child(2)) == "RegFrm")) { return seq(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "RegFrm") && (symbol_name(node.child(1)) == "+") && (symbol_name(node.child(2)) == "RegFrm")) { return alt(parse_RegFrm(node.child(0)), parse_RegFrm(node.child(2))); }
    report_unexpected_node(node);
    return regular_formulas::regular_formula();
  }
};

} // namespace regular_formulas

namespace state_formulas {

struct state_formula_actions: public regular_formulas::regular_formula_actions
{
  state_formula_actions(const dparser::parser_table& table_)
    : regular_formulas::regular_formula_actions(table_)
  {}

  data::data_expression parse_Time(const dparser::parse_node& node)
  {
    if (node.child(1))
    {
      return parse_DataExpr(node.child(1));
    }
    else
    {
      return data::data_expression(core::detail::gsMakeNil());
    }
  }

  state_formulas::state_formula parse_StateFrm(const dparser::parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "DataValExpr")) { return parse_DataValExpr(node.child(0)); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return state_formulas::true_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return state_formulas::false_(); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "StateFrm")) { return state_formulas::not_(parse_StateFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "=>") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::imp(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "&&") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::and_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "StateFrm") && (symbol_name(node.child(1)) == "||") && (symbol_name(node.child(2)) == "StateFrm")) { return state_formulas::or_(parse_StateFrm(node.child(0)), parse_StateFrm(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::forall(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::exists(parse_VarsDeclList(node.child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == "]")) { return state_formulas::may(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "<") && (symbol_name(node.child(1)) == "RegFrm") && (symbol_name(node.child(2)) == ">")) { return state_formulas::must(parse_RegFrm(node.child(1))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "mu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::mu(parse_Id(node.child(1).child(0)), parse_DataExprList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "nu") && (symbol_name(node.child(1)) == "StateVarDecl") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "StateFrm")) { return state_formulas::nu(parse_Id(node.child(1).child(0)), parse_DataExprList(node.child(1).child(1)), parse_StateFrm(node.child(3))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "Id")) { return state_formulas::variable(parse_Id(node.child(0)), parse_DataExprList(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "delay")) { return state_formulas::delay(parse_Time(node.child(1))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "yaled")) { return state_formulas::yaled(parse_Time(node.child(1))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "StateFrm") && (symbol_name(node.child(2)) == ")")) { return parse_StateFrm(node.child(1)); }
    report_unexpected_node(node);
    return state_formulas::state_formula();
  }
};

} // namespace state_formulas

} // namespace mcrl2
