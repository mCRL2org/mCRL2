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
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/pbes/typecheck.h"

using namespace mcrl2;

void test_pbes_specification(const std::string &pbes_in, bool test_type_checker = true)
{
  std::istringstream pbes_in_stream(pbes_in);
  ATermAppl pbes_aterm = core::parse_pbes_spec(pbes_in_stream);
  BOOST_REQUIRE(pbes_aterm != NULL);

  std::string pbes_out = core::PrintPart_CXX((ATerm) pbes_aterm);
  //std::cerr << "The following PBES specifications should be the same:" << std::endl << pbes_in << std::endl << "and" << std::endl << pbes_out << std::endl;
  BOOST_CHECK(pbes_in == pbes_out);

  if (test_type_checker) {
    pbes_system::pbes<> p(pbes_aterm);
    pbes_system::type_check(p);
    //pbes_aterm = core::type_check_pbes_spec(pbes_aterm);
    //BOOST_REQUIRE(pbes_aterm != NULL);
 
    //pbes_out = core::PrintPart_CXX((ATerm) pbes_aterm);
    pbes_out = pbes_system::pp(p);
    //std::cerr << "The following PBES specifications should be the same:" << std::endl << pbes_in  << std::endl << "and" << std::endl << pbes_out << std::endl;
    BOOST_CHECK(pbes_in == pbes_out);
  }
  core::garbage_collect();
}

void test_pbes_specifications()
{
  //test PBES specification involving global variables
  test_pbes_specification(
    "glob dc: Bool;\n"
    "\n"
    "pbes nu X(b: Bool) =\n"
    "       val(b) && X(dc);\n"
    "\n"
    "init X(dc);\n"
  );
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_pbes_specifications();

  return 0;
}
