// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_print_test.cpp
/// \brief Tests for the parse and print functionality of the core library

#include <boost/test/minimal.hpp>

#define MCRL2_WITH_GARBAGE_COLLECTION
#include "mcrl2/core/garbage_collection.h"

#include <sstream>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/messaging.h"

void test_data_expression(const std::string &de_in, bool test_type_checker = true)
{
  std::istringstream de_in_stream(de_in);
  ATermAppl de_aterm = mcrl2::core::parse_data_expr(de_in_stream);
  BOOST_REQUIRE(de_aterm != NULL);

  std::string de_out = mcrl2::core::PrintPart_CXX((ATerm) de_aterm);
  //std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
  BOOST_CHECK(de_in == de_out);

  if (test_type_checker) {
    de_aterm = mcrl2::core::type_check_data_expr(de_aterm, NULL, mcrl2::core::detail::gsMakeEmptyDataSpec());
    BOOST_REQUIRE(de_aterm != NULL);
   
    de_out = mcrl2::core::PrintPart_CXX((ATerm) de_aterm);
    //std::cerr << "The following data expressions should be the same:" << std::endl << "  " << de_in  << std::endl << "  " << de_out << std::endl;
    BOOST_CHECK(de_in == de_out);
  }
}

void test_data_specification(const std::string &ds_in, bool test_type_checker = true)
{
  std::istringstream ds_in_stream(ds_in);
  ATermAppl ds_aterm = mcrl2::core::parse_data_spec(ds_in_stream);
  BOOST_REQUIRE(ds_aterm != NULL);

  std::string ds_out = mcrl2::core::PrintPart_CXX((ATerm) ds_aterm);
  //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in << std::endl << "and" << std::endl << ds_out << std::endl;
  BOOST_CHECK(ds_in == ds_out);

  if (test_type_checker) {
    ds_aterm = mcrl2::core::type_check_data_spec(ds_aterm);
    BOOST_REQUIRE(ds_aterm != NULL);
 
    ds_out = mcrl2::core::PrintPart_CXX((ATerm) ds_aterm);
    //std::cerr << "The following data specifications should be the same:" << std::endl << ds_in  << std::endl << "and" << std::endl << ds_out << std::endl;
    BOOST_CHECK(ds_in == ds_out);
  }
}

void test_process_specification(const std::string &ps_in, bool test_type_checker = true)
{
  std::istringstream ps_in_stream(ps_in);
  ATermAppl ps_aterm = mcrl2::core::parse_proc_spec(ps_in_stream);
  BOOST_REQUIRE(ps_aterm != NULL);

  std::string ps_out = mcrl2::core::PrintPart_CXX((ATerm) ps_aterm);
  //std::cerr << "The following process specifications should be the same:" << std::endl << ps_in << std::endl << "and" << std::endl << ps_out << std::endl;
  BOOST_CHECK(ps_in == ps_out);

  if (test_type_checker) {
    ps_aterm = mcrl2::core::type_check_proc_spec(ps_aterm);
    BOOST_REQUIRE(ps_aterm != NULL);
 
    ps_out = mcrl2::core::PrintPart_CXX((ATerm) ps_aterm);
    //std::cerr << "The following process specifications should be the same:" << std::endl << ps_in  << std::endl << "and" << std::endl << ps_out << std::endl;
    BOOST_CHECK(ps_in == ps_out);
  }
}

void test_pbes_specification(const std::string &pbes_in, bool test_type_checker = true)
{
  std::istringstream pbes_in_stream(pbes_in);
  ATermAppl pbes_aterm = mcrl2::core::parse_pbes_spec(pbes_in_stream);
  BOOST_REQUIRE(pbes_aterm != NULL);

  std::string pbes_out = mcrl2::core::PrintPart_CXX((ATerm) pbes_aterm);
  //std::cerr << "The following PBES specifications should be the same:" << std::endl << pbes_in << std::endl << "and" << std::endl << pbes_out << std::endl;
  BOOST_CHECK(pbes_in == pbes_out);

  if (test_type_checker) {
    pbes_aterm = mcrl2::core::type_check_pbes_spec(pbes_aterm);
    BOOST_REQUIRE(pbes_aterm != NULL);
 
    pbes_out = mcrl2::core::PrintPart_CXX((ATerm) pbes_aterm);
    //std::cerr << "The following PBES specifications should be the same:" << std::endl << pbes_in  << std::endl << "and" << std::endl << pbes_out << std::endl;
    BOOST_CHECK(pbes_in == pbes_out);
  }
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  //test boolean data expressions
  test_data_expression("true");
  mcrl2::core::garbage_collect();
  test_data_expression("if(true, true, false)");
  mcrl2::core::garbage_collect();
  test_data_expression("!true");
  mcrl2::core::garbage_collect();
  test_data_expression("true && false");
  mcrl2::core::garbage_collect();

  //test numbers
  test_data_expression("0");
  mcrl2::core::garbage_collect();
  test_data_expression("-1");
  mcrl2::core::garbage_collect();
  test_data_expression("0 + 1");
  mcrl2::core::garbage_collect();
  test_data_expression("1 * 2 + 3");
  mcrl2::core::garbage_collect();

  //test enumerations
  test_data_expression("[true, false]");
  mcrl2::core::garbage_collect();
  test_data_expression("{true, false}");
  mcrl2::core::garbage_collect();
  test_data_expression("{true: 1, false: 2}");
  mcrl2::core::garbage_collect();

  //test function updates
  test_data_expression("(lambda x: Bool. x)[true -> false]", false);
  mcrl2::core::garbage_collect();
  test_data_expression("(lambda x: Bool. x)[true -> false][false -> true]", false);
  mcrl2::core::garbage_collect();
  test_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]", false);
  mcrl2::core::garbage_collect();

  //test data specification involving structured sorts
  //in which projection functions are reused
  test_data_specification(
    "sort S = struct c(p: Bool) | d(p: Bool, q: S);\n"
  );
  mcrl2::core::garbage_collect();

  //test process specification involving process reference assignments
  test_process_specification(
    "proc P(b: Bool) = tau . P() + tau . P(b = false);\n"
    "\n"
    "init P(b = true);\n"
  );
  mcrl2::core::garbage_collect();

  //test process specification involving global variables
  test_process_specification(
    "glob dc: Bool;\n"
    "\n"
    "proc P(b: Bool) = tau . P(dc);\n"
    "\n"
    "init P(dc);\n"
  );
  mcrl2::core::garbage_collect();

  //test PBES specification involving global variables
  test_pbes_specification(
    "glob dc: Bool;\n"
    "\n"
    "pbes nu X(b: Bool) =\n"
    "       val(b) && X(dc);\n"
    "\n"
    "init X(dc);\n"
  );
  mcrl2::core::garbage_collect();

  return 0;
}
