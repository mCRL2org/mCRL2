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

#define MCRL2_WITH_GARBAGE_COLLECTION

#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/print.h"

using namespace mcrl2;

data::data_expression parse_data_expression(const std::string& de_in)
{
  std::istringstream de_in_stream(de_in);
  ATermAppl de_aterm = core::parse_data_expr(de_in_stream);
  BOOST_REQUIRE(de_aterm != NULL);

  std::string de_out = core::PrintPart_CXX((ATerm) de_aterm);
  //std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
  BOOST_CHECK_EQUAL(de_in, de_out);

  return data::data_expression(de_aterm);
}

data::data_specification parse_data_specification(const std::string& ds_in)
{
  std::istringstream ds_in_stream(ds_in);
  ATermAppl ds_aterm = core::parse_data_spec(ds_in_stream);
  BOOST_REQUIRE(ds_aterm != NULL);

  std::string ds_out = core::PrintPart_CXX((ATerm) ds_aterm);
  //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in << std::endl << "and" << std::endl << ds_out << std::endl;
  BOOST_CHECK_EQUAL(ds_in, ds_out);

  return data::data_specification(ds_aterm);
}

template <typename VariableIterator>
void test_data_expression(const std::string &de_in,
                          const VariableIterator begin,
                          const VariableIterator end,
                          bool expect_success = true,
                          bool test_type_checker = true)
{
  data::data_expression x(parse_data_expression(de_in));

  if (test_type_checker) {
    if(expect_success)
    {
      data::type_check(x, begin, end);
      atermpp::aterm de_aterm = x;

      std::string de_out = core::PrintPart_CXX((ATerm) de_aterm);
      //std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
      BOOST_CHECK_EQUAL(de_in, de_out);
    }
    else
    {
      BOOST_CHECK_THROW(data::type_check(x), mcrl2::runtime_error);
    }
  }
  core::garbage_collect();
}

void test_data_expression(const std::string &de_in,
                          bool expect_success = true,
                          bool test_type_checker = true)
{
  data::variable_vector v;
  test_data_expression(de_in, v.begin(), v.end(), expect_success, test_type_checker);
}

BOOST_AUTO_TEST_CASE(test_booleans)
{
  //test boolean data expressions
   test_data_expression("true");
   test_data_expression("if(true, true, false)");
   test_data_expression("!true");
   test_data_expression("true && false");
}

BOOST_AUTO_TEST_CASE(test_numbers)
{
  //test numbers
  test_data_expression("0");
  test_data_expression("-1");
  test_data_expression("0 + 1");
  test_data_expression("1 * 2 + 3");
}

BOOST_AUTO_TEST_CASE(test_enumerations)
{
  //test enumerations
  test_data_expression("[true, false]");
  test_data_expression("{true, false}");
  test_data_expression("{true: 1, false: 2}");
}

BOOST_AUTO_TEST_CASE(test_function_updates)
{
  //test function updates
  /*
  test_data_expression("(lambda x: Bool. x)[true -> false]", false);
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]", false);
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]", false);
  */
}

BOOST_AUTO_TEST_CASE(test_inline_struct_recogniser)
{
  test_data_expression("lambda x: struct t?is_t. x == t", false);
}

BOOST_AUTO_TEST_CASE(test_inline_struct)
{
  test_data_expression("lambda x: struct t. x == t", false);
}

BOOST_AUTO_TEST_CASE(test_inline_structs_compare)
{
  test_data_expression("lambda x,y: struct t. x == y");
}

BOOST_AUTO_TEST_CASE(test_inline_structs_compare_recogniser)
{
  test_data_expression("lambda x: struct t?is_t, y: struct t. x == y", false);
}

BOOST_AUTO_TEST_CASE(test_lambda_aliasing)
{
  test_data_expression("lambda f: Nat. lambda f: Nat -> Bool. f(f)", false);
}

BOOST_AUTO_TEST_CASE(test_where)
{
  test_data_expression("x + y whr x = 3, y = 10 end");
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two)
{
  test_data_expression("x + y whr x = 3, y = x + 10 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_one_and_two_occur_in_two)
{
  test_data_expression("x + y whr x = 3, y = x + y + 10 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_two_occurs_in_one)
{
  test_data_expression("x + y whr x = y + 10, y = 3 end", false);
}

BOOST_AUTO_TEST_CASE(test_where_var_one_occurs_in_two_and_vice_versa)
{
  test_data_expression("x + y whr x = y + 10, y = x + 3 end", false);
}

BOOST_AUTO_TEST_CASE(test_upcast_pos2nat)
{
  data::variable_vector v;
  v.push_back(data::variable("x", data::sort_pos::pos()));
  v.push_back(data::variable("y", data::sort_nat::nat()));
  test_data_expression("x + y", v.begin(), v.end());
  test_data_expression("x == y", v.begin(), v.end());
}

BOOST_AUTO_TEST_CASE(test_upcast_list_pos2nat)
{
  data::variable_vector v;
  v.push_back(data::variable("x", data::sort_list::list(data::sort_pos::pos())));
  v.push_back(data::variable("y", data::sort_list::list(data::sort_nat::nat())));
  test_data_expression("x ++ y", v.begin(), v.end(), false);
  test_data_expression("x == y", v.begin(), v.end(), false);
}

void test_data_specification(const std::string &ds_in,
                             bool expect_success = true,
                             bool test_type_checker = true)
{
  data::data_specification ds(parse_data_specification(ds_in));

  if (test_type_checker) {

    if (expect_success) {
      data::type_check(ds);

      //ds_out = core::PrintPart_CXX((ATerm) ds_aterm);
      std::string ds_out = data::pp(ds);
      //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in  << std::endl << "and" << std::endl << ds_out << std::endl;
      BOOST_CHECK_EQUAL(ds_in, ds_out);
    }
    else
    {
      BOOST_CHECK_THROW(data::type_check(ds), mcrl2::runtime_error);
    }
  }
  core::garbage_collect();
}

BOOST_AUTO_TEST_CASE(test_data_specification_struct_with_projection)
{
  //test data specification involving structured sorts
  //in which projection functions are reused
  test_data_specification(
    "sort S = struct c(p: Bool) | d(p: Bool, q: S);\n"
  );
}

// What is the desired result here? I would expect an exception (JK)
BOOST_AUTO_TEST_CASE(test_duplicate_sort)
{
  test_data_specification(
    "sort S = struct c;\n"
    "     S = Nat;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_same_signature)
{
  //test data specification involving multiple constructors/functions with
  //different signatures
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: T;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_map_same_signature)
{
  //test data specification involving multiple constructors/functions with
  //different signatures
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "\n"
    "map f: T;\n",
    false
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_constructor_different_signature)
{
  test_data_specification(
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: S -> T;\n"
  );
}

BOOST_AUTO_TEST_CASE(test_data_specification_nested_struct)
{
  test_data_specification(
    "sort S = struct t(struct e(Nat));\n"
  );
}

template <typename VariableIterator>
void test_data_expression_in_specification_context(const std::string &de_in,
                          const std::string& ds_in,
                          const VariableIterator begin,
                          const VariableIterator end,
                          bool expect_success = true,
                          bool test_type_checker = true)
{
  data::data_specification ds(parse_data_specification(ds_in));

  if(test_type_checker)
  {
    data::type_check(ds);

    std::string ds_out = data::pp(ds);
    BOOST_CHECK_EQUAL(ds_in, ds_out);
  }

  data::data_expression de(parse_data_expression(de_in));

  if(test_type_checker)
  {
    if(expect_success)
    {
      data::type_check(de, begin, end, ds);
      atermpp::aterm de_aterm = de;

      std::string de_out = core::PrintPart_CXX((ATerm) de_aterm);

      BOOST_CHECK_EQUAL(de_in, de_out);
    }
    else
    {
      BOOST_CHECK_THROW(data::type_check(de, begin, end, ds), mcrl2::runtime_error);
    }
  }
}

void test_data_expression_in_specification_context(const std::string &de_in,
                          const std::string& ds_in,
                          bool expect_success = true,
                          bool test_type_checker = true)
{
  data::variable_vector v;
  test_data_expression_in_specification_context(de_in, ds_in, v.begin(), v.end(),expect_success,test_type_checker);
}

BOOST_AUTO_TEST_CASE(test_data_expressions_different_signature)
{
  test_data_expression_in_specification_context(
    "f(f)",
    "sort S;\n"
    "     T;\n"
    "\n"
    "cons f: S;\n"
    "     f: S -> T;\n"
  );
}

BOOST_AUTO_TEST_CASE(test_data_expressions_struct)
{
  data::variable_vector v;
  data::basic_sort s("S");
  v.push_back(data::variable("x", s));

  test_data_expression_in_specification_context(
    "x == t(e(3))",
    "sort S = struct t(struct e(Nat));\n",
    v.begin(), v.end()
  );
}

BOOST_AUTO_TEST_CASE(test_lambda_variable_aliasing)
{
  data::variable_vector v;
  data::function_sort s(data::basic_sort("S"), data::basic_sort("T"));
  v.push_back(data::variable("x", s));
  test_data_expression_in_specification_context(
    "lambda x: S. x(x)",
    "sort S;\n",
    v.begin(), v.end(),
    false);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
