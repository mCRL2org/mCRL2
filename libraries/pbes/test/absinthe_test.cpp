// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file absinthe_test.cpp
/// \brief Test program for absinthe algorithm.

#define BOOST_TEST_MODULE absinthe_test
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_separate_keyword_section)
{
  std::string text =
    "\n"
    "\n"
    "absmap\n"
    "h: Nat -> AbsNat\n"
  ;

  std::vector<std::string> all_keywords = { "sort", "absmap" };
  std::pair<std::string, std::string> q = utilities::detail::separate_keyword_section(text, "absmap", all_keywords);
  BOOST_CHECK(q.first.find("absmap") == 0);
  BOOST_CHECK(utilities::trim_copy(q.second).empty());
}

void test_absinthe(const std::string& pbes_text, const std::string& abstraction_text, bool is_over_approximation)
{
  pbes p = txt2pbes(pbes_text);
  absinthe_algorithm algorithm;
  algorithm.run(p, abstraction_text, is_over_approximation);
}

// test with structured sorts
BOOST_AUTO_TEST_CASE(test_structured_sorts)
{
  std::string PBES_TEXT =
    "sort Bit = struct e0 | e1;                     \n"
    "                                               \n"
    "map inv : Bit -> Bit;                          \n"
    "                                               \n"
    "eqn                                            \n"
    "  inv(e0) = e1;                                \n"
    "  inv(e1) = e0;                                \n"
    "                                               \n"
    "pbes                                           \n"
    "                                               \n"
    "nu X(b:Bit) = val (b == e0) && X(inv(inv(b))); \n"
    "                                               \n"
    "init X(e0);                                    \n"
    ;

  std::string ABSTRACTION_TEXT =
    "sort                                                                   \n"
    "  AbsBit  = struct arbitrary;                                          \n"
    "                                                                       \n"
    "var                                                                    \n"
    "  b:Bit;                                                               \n"
    "eqn                                                                    \n"
    "  h(b) = arbitrary;                                                    \n"
    "  abseq(arbitrary,arbitrary) = {true,false};                           \n"
    "  absinv(arbitrary) = {arbitrary};                                     \n"
    "                                                                       \n"
    "absmap                                                                 \n"
    "h: Bit -> AbsBit;                                                      \n"
    "                                                                       \n"
    "absfunc                                                                \n"
    "  ==: Bit # Bit -> Bool        := abseq : AbsBit # AbsBit -> Set(Bool) \n"
    "  inv: Bit -> Bit              := absinv : AbsBit -> Set(AbsBit)       \n"
  ;

  test_absinthe(PBES_TEXT, ABSTRACTION_TEXT, true);
}
