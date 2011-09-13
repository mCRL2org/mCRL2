#include <iterator>
#include <boost/bind.hpp>
#include "parser.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/identifier.h"

namespace mcrl2 {

namespace data {

using namespace dparser;

struct default_actions
{
  const parser_table& table;

  default_actions(const parser_table& table_)
    : table(table_)
  {}

  std::string symbol_name(const parse_node& node) const
  {
    return table.symbol_name(node.symbol());
  }

  void report_unknown_node(const parse_node& node, const std::string& msg = "")
  {
    std::cout << "<node>" << symbol_name(node) << std::endl;
    for (int i = 0; i < node.child_count(); i++)
    {
      std::cout << "<child>" << symbol_name(node.child(i)) << " " << node.child(i).string() << std::endl;
    }
    throw mcrl2::runtime_error("unknown node detected: " + msg + " " + node.string());
  }

  template <typename OutputIterator, typename ParseFunction>
  void parse_list(const parse_node& node, const std::string& type, ParseFunction f, OutputIterator out)
  {
    if (!node)
    {
      return;
    }
    std::cout << "<symbol>" << symbol_name(node) << std::endl;
    if (symbol_name(node) == type)
    {
      *out = f(node);
    }
    else
    {
      for (int i = 0; i < node.child_count(); i++)
      {
        parse_list(node.child(i), type, f, out);
      }
    }
  }

  template <typename T, typename ParseFunction>
  atermpp::term_list<T> parse_list(const parse_node& node, const std::string& type, ParseFunction f)
  {
    atermpp::vector<T> result;
    parse_list(node, type, f, std::back_inserter(result));
    return atermpp::term_list<T>(result.begin(), result.end());
  }

  core::identifier_string parse_Id(const parse_node& node)
  {
    return core::identifier_string(node.string());
  }
};

struct sort_actions: public default_actions
{
  sort_actions(const parser_table& table_)
    : default_actions(table_)
  {}

  data::sort_expression parse_SortExpr(const parse_node& node)
  {
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Bool")) { sort_bool::bool_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Pos")) { return sort_pos::pos(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Nat")) { return sort_nat::nat(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Int")) { return sort_int::int_(); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Real")) { return sort_real::real_(); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "List") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_list::list(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Set") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_set::set_(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Bag") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ")")) { return sort_bag::bag(parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return parse_Id(node.child(0)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "SortExpr") && (symbol_name(node.child(2)) == ")")) { return parse_SortExpr(node.child(1)); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "SortExprList") && (symbol_name(node.child(1)) == "->") && (symbol_name(node.child(2)) == "SortExpr")) { return function_sort(parse_SortExprList(node.child(0)), parse_SortExpr(node.child(2))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "struct") && (symbol_name(node.child(1)) == "ConstrDeclList")) { return structured_sort(parse_ConstrDeclList(node.child(1))); }
    report_unknown_node(node);
    return data::sort_expression();
  }

  data::sort_expression_list parse_SortExprList(const parse_node& node)
  {
    return parse_list<data::sort_expression>(node, "SortExpr", boost::bind(&sort_actions::parse_SortExpr, this, _1));
  }

  data::structured_sort_constructor parse_ConstrDecl(const parse_node& node)
  {
    core::identifier_string name = parse_Id(node.child(0));
    data::structured_sort_constructor_argument_list arguments;
    identifier recogniser = no_identifier();
    if (node.child(1))
    {
      arguments = parse_ProjDeclList(node.child(1).child(1));
    }
    if (node.child(2))
    {
      recogniser = parse_Id(node.child(2).child(1));
    }
    return structured_sort_constructor(name, arguments, recogniser);
  }

  data::structured_sort_constructor_list parse_ConstrDeclList(const parse_node& node)
  {
    return parse_list<data::structured_sort_constructor>(node, "ConstrDecl", boost::bind(&sort_actions::parse_SortExpr, this, _1));
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
    return parse_list<data::structured_sort_constructor_argument>(node, "ProjDecl", boost::bind(&sort_actions::parse_ProjDecl, this, _1));
  }

//  UNKNOWN parse_SortSpec(const parse_node& node)
//  {
//    return UNKNOWN_ALTERNATIVE(parse_SortDeclList(node.child(1)));
//  }
//
//  UNKNOWN parse_SortDecl(const parse_node& node)
//  {
//    if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "IdList") && (symbol_name(node.child(1)) == ";")) { return UNKNOWN_ALTERNATIVE(parse_IdList(node.child(0))); }
//    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "Id") && (symbol_name(node.child(1)) == "=") && (symbol_name(node.child(2)) == "SortExpr") && (symbol_name(node.child(3)) == ";")) { return UNKNOWN_ALTERNATIVE(parse_Id(node.child(0)), parse_SortExpr(node.child(2))); }
//    report_unknown_node(node);
//    return UNKNOWN();
//  }
//  UNKNOWN parse_SortDeclList(const parse_node& node)
//  {
//    return parse_list<UNKNOWN>("SortDecl", node);
//  }
};

//struct data_actions: public sort_actions
//{
//  data_actions(const parser_table& table_)
//    : sort_actions(table_)
//  {}
//};

} // namespace data

} // namespace mcrl2
