// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/type_check_tree.h"
#include "mcrl2/data/type_checker.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;

void test_data_expression(const std::string& text, const data::data_specification& dataspec = data::data_specification())
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("DataExpr");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  core::warn_and_or(node);
  data::type_check_node_ptr tnode = data::type_check_tree_generator(p).parse_DataExpr(node);
  data::type_checker checker(dataspec);
  std::map<core::identifier_string, data::sort_expression_vector> declared_variables;
  data::type_check_context context(checker, declared_variables);
  tnode->set_constraint(context);
  data::print_node(tnode);
  p.destroy_parse_node(node);
}

inline
data::sort_expression pos()
{
  return data::sort_pos::pos();
}

inline
data::sort_expression nat()
{
  return data::sort_nat::nat();
}

inline
data::sort_expression list(const data::sort_expression& x)
{
  return data::sort_list::list(x);
}

inline
data::variable var(const std::string& name, const data::sort_expression& sort)
{
  return data::variable(name, sort);
}

// Parse functions that do not change any context (i.e. do not typecheck and
// normalise sorts).
data::sort_expression parse_sort_expression(const std::string& de_in)
{
  data::sort_expression result;
  try {
    result = data::detail::parse_sort_expression_new(de_in);
    std::string de_out = data::pp(result);
    if (de_in != de_out)
    {
      std::clog << "aterm : " << result << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following sort expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(de_in, de_out);
    }
  }
  catch (...)
  {
    BOOST_CHECK(false);
  }
  return result;
}

data::data_expression parse_data_expression(const std::string& de_in)
{
  test_data_expression(de_in);
  data::data_expression result;
  try {
    result = data::detail::parse_data_expression_new(de_in);
#ifdef MCRL2_ENABLE_TYPECHECK_PP_TESTS
    std::string de_out = data::pp(result);
    if (de_in != de_out)
    {
      std::clog << "aterm : " << result << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(de_in, de_out);
    }
#endif
  }
  catch (...)
  {
    BOOST_CHECK(false);
  }
  return result;
}

data::data_specification parse_data_specification(const std::string& de_in, bool expect_success = true)
{
  data::data_specification result;
  try {
    result = data::detail::parse_data_specification_new(de_in);
    std::string de_out = data::pp(result);

    std::string input = utilities::trim_copy(de_in);
    std::string output = utilities::trim_copy(de_out);
    if (input != output)
    {
      std::clog << "aterm : " << data::detail::data_specification_to_aterm_data_spec(result) << std::endl;
      std::clog << "de_in : " << de_in << std::endl;
      std::clog << "de_out: " << de_out << std::endl;
      std::clog << "The following data specifications should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(input, output);
    }
  }
  catch (...)
  {
    BOOST_CHECK(!expect_success);
  }
  return result;
}

void test_data_expression(const std::string& de_in,
                          const data::variable_vector& variable_context,
                          bool expect_success = true,
                          const std::string& expected_sort = "",
                          bool test_type_checker = true)
{
  data::data_expression x(parse_data_expression(de_in));
  std::clog << std::endl
            << "==========================================" << std::endl
            << "Testing type checking of data expression: " << std::endl
            << "  de_in:  " << de_in << std::endl
            << "  de_out: " << pp(x) << std::endl
            << "  expect success: " << (expect_success?("yes"):("no")) << std::endl
            << "  expected type: " << expected_sort << std::endl
            << "  detected type: " << pp(x.sort()) << " (before typecheckeing) " << std::endl;


  if (test_type_checker)
  {
    if (expect_success)
    {
      BOOST_CHECK_NO_THROW(data::type_check(x, variable_context.begin(), variable_context.end()));
      BOOST_CHECK_NE(x, data::data_expression());

      std::string de_out = data::pp(x);
      BOOST_CHECK_EQUAL(de_in, de_out);
      // TODO: this check should be uncommented
      //BOOST_CHECK(!search_sort_expression(x.sort(), data::untyped_sort()));
      if (expected_sort != "")
      {
        BOOST_CHECK_EQUAL(x.sort(), parse_sort_expression(expected_sort));
        std::clog << "    expression x in internal format: " << x << std::endl;
      }
      else
      {
        std::clog << "  failed to typecheck" << std::endl;
      }
    }
    else
    {
      BOOST_CHECK_THROW(data::type_check(x), mcrl2::runtime_error);
    }
  }
}

void test_data_expression(const std::string& de_in,
                          bool expect_success = true,
                          const std::string& expected_sort = "",
                          bool test_type_checker = true)
{
  data::variable_vector v;
  test_data_expression(de_in, v, expect_success, expected_sort, test_type_checker);
}

BOOST_AUTO_TEST_CASE(data_expressions_test)
{
  test_data_expression("true", true, "Bool");
  test_data_expression("if(true, true, false)", true, "Bool");
  test_data_expression("!true", true, "Bool");
  test_data_expression("true && false", true, "Bool");
  test_data_expression("0", true, "Nat");
  test_data_expression("-1", true, "Int");
  test_data_expression("0 + 1", true, "Pos");
  test_data_expression("1 + 0", true, "Pos");
  test_data_expression("0 + 0", true, "Nat");
  test_data_expression("1 + 1", true, "Pos");
  test_data_expression("1 * 2 + 3", true, "Pos");
  test_data_expression("[]", true); // List unknown
  test_data_expression("[] ++ []", true); // List unknown
  test_data_expression("#[]", true, "Nat");
  test_data_expression("true in []", true, "Bool");
  test_data_expression("[true, false]", true, "List(Bool)");
  test_data_expression("[0]", true, "List(Nat)");
  test_data_expression("[1, 2]", true, "List(Pos)");
  test_data_expression("[0] ++ [1, 2]", true, "List(Nat)");
  test_data_expression("[0, 1, 2]", true, "List(Nat)");
  test_data_expression("[1, 0, 2]", true, "List(Nat)");
  test_data_expression("l ++ [1, 2]", { var("l", list(nat())) }, true, "List(Nat)");
  test_data_expression("l ++ [1, 2]", { var("l", list(pos())) }, true, "List(Pos)");
  test_data_expression("[0] ++ l", { var("l", list(pos())) }, false);
  test_data_expression("[0] ++ l", { var("l", list(nat())) }, true, "List(Nat)");
  test_data_expression("x ++ y", { var("x", list(pos())), var("y", list(nat())) }, false);
  test_data_expression("x == y", { var("x", list(pos())),  var("y", list(nat())) }, false);
  test_data_expression("{}", true);
  test_data_expression("!{}", true);
  test_data_expression("!{} <= {}", true);
  test_data_expression("{} <= !{}", true);
  test_data_expression("{ true, false }", true, "FSet(Bool)");
  test_data_expression("{ 1, 2, (-7) }", true, "FSet(Int)");
  test_data_expression("{ x: Nat | x mod 2 == 0 }", true, "Set(Nat)");
  test_data_expression("{:}", true);
  test_data_expression("!{:}", false);
  test_data_expression("{ true: 1, false: 2 }", true, "FBag(Bool)");
  test_data_expression("{ 1: 1, 2: 2, -8: 8 }", true, "FBag(Int)");
  test_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }", true, "Bag(Nat)");
  test_data_expression("(lambda x: Bool. x)[true -> false]", true);
  test_data_expression("(lambda x: Bool. x)[0 -> false]", false);
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]", true);
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]", true);
  test_data_expression("lambda x: struct t?is_t. x == t", false);
  test_data_expression("lambda x: struct t. x == t", false);
  test_data_expression("lambda x,y: struct t. x == y", true, "struct t # struct t -> Bool");
  test_data_expression("lambda x: struct t?is_t, y: struct t. x == y", false);
  test_data_expression("lambda f: Nat. lambda f: Nat -> Bool. f(f)", false);
  test_data_expression("forall x,y: struct t. x == y", true, "Bool");
  test_data_expression("forall n: Nat. n >= 0", true, "Bool");
  test_data_expression("forall n: Nat. n > -1", true, "Bool");
  test_data_expression("exists x,y: struct t. x == y", true, "Bool");
  test_data_expression("exists n: Nat. n > 481", true, "Bool");
  test_data_expression("x == y", { var("x", parse_sort_expression("struct t?is_t")), var("y", parse_sort_expression("struct t?is_t")) }, true, "Bool");
  test_data_expression("x == y", { var("x", parse_sort_expression("struct t")), var("y", parse_sort_expression("struct t?is_t")) }, false);
  test_data_expression("x + y whr x = 3, y = 10 end", true, "Pos");
  test_data_expression("x + y whr x = 3, y = x + 10 end", false);
  test_data_expression("x + y whr x = 3, y = x + y + 10 end", false);
  test_data_expression("x + y whr x = y + 10, y = 3 end", false);
  test_data_expression("x + y whr x = y + 10, y = x + 3 end", false);
  test_data_expression("x + y whr x = 3, y = 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x + y whr x = 3, y = x + 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x + y whr x = 3, y = x + y + 10 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x + y whr x = y + 10, y = 3 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x + y whr x = y + 10, y = x + 3 end", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x ++ y whr x = [0, y], y = [x] end", { var("x", pos()), var("y", nat()) }, false);
  test_data_expression("x ++ y whr x = [0, y], y = [x] end", { var("x", nat()), var("y", nat()) }, true, "List(Nat)");
  test_data_expression("x + y", { var("x", pos()), var("y", nat()) }, true, "Pos");
  test_data_expression("x == y", { var("x", pos()), var("y", nat()) }, true, "Bool");
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
