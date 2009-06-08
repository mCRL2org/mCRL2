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

// Example that didn't compile, submitted by Jan Friso.
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

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_data_expression("true");
  mcrl2::core::garbage_collect();
  test_data_expression("if(true, true, false)");
  mcrl2::core::garbage_collect();
  test_data_expression("!true");
  mcrl2::core::garbage_collect();
  test_data_expression("true && false");
  mcrl2::core::garbage_collect();

  return 0;
}
