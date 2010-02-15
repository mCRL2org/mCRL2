// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck2_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/process/typecheck.h"

using namespace mcrl2;

void test_process_specification(const std::string &ps_in, bool test_type_checker = true)
{
  std::istringstream ps_in_stream(ps_in);
  ATermAppl ps_aterm = core::parse_proc_spec(ps_in_stream);
  BOOST_REQUIRE(ps_aterm != NULL);

  std::string ps_out = core::PrintPart_CXX((ATerm) ps_aterm);
  //std::cerr << "The following process specifications should be the same:" << std::endl << ps_in << std::endl << "and" << std::endl << ps_out << std::endl;
  BOOST_CHECK(ps_in == ps_out);

  if (test_type_checker) {
    process::process_specification ps(ps_aterm);
    process::type_check(ps);
 
    //ps_out = core::PrintPart_CXX((ATerm) ps_aterm);
    ps_out = process::pp(ps);
    //std::cerr << "The following process specifications should be the same:" << std::endl << ps_in  << std::endl << "and" << std::endl << ps_out << std::endl;
    BOOST_CHECK(ps_in == ps_out);
  }
  core::garbage_collect();
}

void test_process_specifications()
{
  //test process specification involving process reference assignments
  test_process_specification(
    "proc P(b: Bool) = tau . P() + tau . P(b = false);\n"
    "\n"
    "init P(b = true);\n"
  );

  //test process specification involving global variables
  test_process_specification(
    "glob dc: Bool;\n"
    "\n"
    "proc P(b: Bool) = tau . P(dc);\n"
    "\n"
    "init P(dc);\n"
  );
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_process_specifications();

  return 0;
}
