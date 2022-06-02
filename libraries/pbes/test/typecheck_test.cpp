// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE typecheck_test
#include <boost/test/included/unit_test.hpp>
#include "mcrl2/core/parse.h"
#include "mcrl2/pbes/parse.h"

using namespace mcrl2;

void test_pbes_specification(const std::string& pbes_in, bool test_type_checker = true)
{
  pbes_system::pbes p = pbes_system::detail::parse_pbes_new(pbes_in).construct_pbes();
  if (test_type_checker)
  {
    pbes_system::typecheck_pbes(p);
    std::string pbes_out = pbes_system::pp(p);

    if (pbes_in!=pbes_out)
    {
      std::cerr << "PBES IN AND PBES OUT ARE DIFFERENT (with typechecking).\n";
      std::cerr << "PBES IN: " << pbes_in << "\n";
      std::cerr << "PBES OUT: " << pbes_out << "\n";
    }
    BOOST_CHECK(pbes_in == pbes_out);
  }
}

BOOST_AUTO_TEST_CASE(test_pbes_specification1)
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

BOOST_AUTO_TEST_CASE(test_pbes_specification2)
{
  //test PBES specification where the type of [10,m] should become List(Nat), not List(Pos).
  //This failed in revision 10180 and before.
  test_pbes_specification(
   "pbes nu X0(m: Nat) =\n"
   "       forall i: Nat. val(!(i < 2)) || X0([10, m] . i);\n"
   "\n"
   "init X0(0);\n"
  );
}
