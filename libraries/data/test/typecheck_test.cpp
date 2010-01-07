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
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/print.h"

using namespace mcrl2;

void test_data_expression(const std::string &de_in, bool test_type_checker = true)
{
  std::istringstream de_in_stream(de_in);
  ATermAppl de_aterm = core::parse_data_expr(de_in_stream);
  BOOST_REQUIRE(de_aterm != NULL);

  std::string de_out = core::PrintPart_CXX((ATerm) de_aterm);
  //std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
  BOOST_CHECK(de_in == de_out);

  if (test_type_checker) {
    data::data_expression x(de_aterm);
    data::type_check(x);
    de_aterm = x;
   
    de_out = core::PrintPart_CXX((ATerm) de_aterm);
    std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
    BOOST_CHECK(de_in == de_out);
  }
  core::garbage_collect();
}

void test_data_expressions()
{
  //test boolean data expressions
  test_data_expression("true");
  test_data_expression("if(true, true, false)");
  test_data_expression("!true");
  test_data_expression("true && false");

  //test numbers
  test_data_expression("0");
  test_data_expression("-1");
  test_data_expression("0 + 1");
  test_data_expression("1 * 2 + 3");

  //test enumerations
  test_data_expression("[true, false]");
  test_data_expression("{true, false}");
  test_data_expression("{true: 1, false: 2}");

  //test function updates
  /*
  test_data_expression("(lambda x: Bool. x)[true -> false]", false);
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]", false);
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]", false);
  */
}

void test_data_specification(const std::string &ds_in, bool test_type_checker = true)
{
  std::istringstream ds_in_stream(ds_in);
  ATermAppl ds_aterm = core::parse_data_spec(ds_in_stream);
  BOOST_REQUIRE(ds_aterm != NULL);

  std::string ds_out = core::PrintPart_CXX((ATerm) ds_aterm);
  //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in << std::endl << "and" << std::endl << ds_out << std::endl;
  BOOST_CHECK(ds_in == ds_out);

  if (test_type_checker) {
    //ds_aterm = core::type_check_data_spec(ds_aterm);
    //BOOST_REQUIRE(ds_aterm != NULL);
    data::data_specification ds(ds_aterm);
    data::type_check(ds);
 
    //ds_out = core::PrintPart_CXX((ATerm) ds_aterm);
    ds_out = data::pp(ds);
    //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in  << std::endl << "and" << std::endl << ds_out << std::endl;
    BOOST_CHECK(ds_in == ds_out);
  }
  core::garbage_collect();
}

void test_data_specifications()
{
  //test data specification involving structured sorts
  //in which projection functions are reused
  test_data_specification(
    "sort S = struct c(p: Bool) | d(p: Bool, q: S);\n"
  );
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_data_expressions();
  test_data_specifications();

  return 0;
}
