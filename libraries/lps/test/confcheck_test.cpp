// Author(s): Unknown
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/data/bool.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

//static bool check_for_ctau(lps::specification const& s) .
static size_t count_ctau(specification const& s)
{
  size_t result = 0;
  auto const& v_summands = s.process().action_summands();

  for (auto i=v_summands.begin(); i!=v_summands.end(); ++i)
  {
    const process::action_list al=i->multi_action().actions();
    if (al.size()==1)
    {
      const process::action_label lab=al.front().label();
      if (to_string(lab.name())=="ctau")
      {
        ++result;
      }
    }
  }
  return result;
}

static
void run_confluence_test_case(const std::string& s, const size_t ctau_count)
{
  using namespace mcrl2::lps;

  specification s0 = parse_linear_process_specification(s);
  Confluence_Checker<specification> checker1(s0);
  // Check confluence for all summands and
  // replace confluents tau's by ctau's.
  checker1.check_confluence_and_mark(data::sort_bool::true_(),0);

  BOOST_CHECK_EQUAL(count_ctau(s0), ctau_count);
}

BOOST_AUTO_TEST_CASE(case_1)
{
  /*
   act a, b;
   init tau.a + tau.b;
   */
  const std::string s(
      "act  Terminate,a,b;\n"
      "proc P(s3: Pos) =\n"
      "       (s3 == 5) ->\n"
      "         b .\n"
      "         P(s3 = 3)\n"
      "     + (s3 == 3) ->\n"
      "         Terminate .\n"
      "         P(s3 = 4)\n"
      "     + (s3 == 2) ->\n"
      "         a .\n"
      "         P(s3 = 3)\n"
      "     + sum e: Bool.\n"
      "         (if(e, true, true) && s3 == 1) ->\n"
      "         tau .\n"
      "         P(s3 = if(e, 5, 2))\n"
      "     + delta;\n"
      "init P(1);\n"
  );

  run_confluence_test_case(s,0);
}


BOOST_AUTO_TEST_CASE(case_2)
{
  /*
   act a, b;
   init a.tau.b.delta;
   */
  const std::string s(
    "act  a,b;\n"
    "proc P(s3: Pos) =\n"
    "       (s3 == 3) ->\n"
    "         b .\n"
    "         P(s3 = 4)\n"
    "     + (s3 == 2) ->\n"
    "         tau .\n"
    "         P(s3 = 3)\n"
    "     + (s3 == 1) ->\n"
    "         a .\n"
    "         P(s3 = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
  );

  run_confluence_test_case(s,1);
}

BOOST_AUTO_TEST_CASE(case_3)
{
  /*
   act a, b;
   init a.b.delta;
   */
  const std::string s(
    "act  a,b;\n"
    "proc P(s3: Pos) =\n"
    "       (s3 == 2) ->\n"
    "         b .\n"
    "         P(s3 = 3)\n"
    "     + (s3 == 1) ->\n"
    "         a .\n"
    "         P(s3 = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
    );
  run_confluence_test_case(s,0);
}

// The following test case unearthed an assertion failure when applying
// substitutions.
BOOST_AUTO_TEST_CASE(case_4)
{
  const std::string s(
    "proc P(m: Int) =\n"
    "       (m == 1) ->\n"
    "         tau .\n"
    "         P(m = m - 1)\n"
    "     + (m == 1) ->\n"
    "         tau .\n"
    "         P()\n"
    "     + delta;\n"
    "\n"
    "init P(0);\n"
    );
  run_confluence_test_case(s,0);
}

// Test case provided by Jaco van der Pol
// in some versions the algorithm hangs on this example.
BOOST_AUTO_TEST_CASE(case_5)
{
  const std::string s(
      "sort Byte = struct ESC | END | OTHER;\n"
      "     Enum3 = struct e2_3 | e1_3 | e0_3;\n"
      "     Enum4 = struct e3_4 | e2_4 | e1_4 | e0_4;\n"
      "map  special: Byte -> Bool;\n"
      "var  b: Byte;\n"
      "eqn  special(b)  =  b == ESC || b == END;\n"
      "act  r,s,r1,s1,r2,s2,c1,c2: Byte;\n"
      "proc P(s3_S: Enum3, b_S: Byte, s30_C: Bool, b_C: Byte, s31_R: Enum4, b_R,b0_R: Byte) =\n"
      "       sum b1_S: Byte.\n"
      "         (s3_S == e2_3 && !(b1_S == ESC || b1_S == END)) ->\n"
      "         r(b1_S) .\n"
      "         P(s3_S = e0_3, b_S = b1_S)\n"
      "     + sum b2_S: Byte.\n"
      "         (s3_S == e2_3 && (b2_S == ESC || b2_S == END)) ->\n"
      "         r(b2_S) .\n"
      "         P(s3_S = e1_3, b_S = b2_S)\n"
      "     + ((s30_C && s31_R == e3_4) && ESC == b_C) ->\n"
      "         tau .\n"
      "         P(s30_C = false, b_C = ESC, s31_R = e2_4, b_R = ESC, b0_R = ESC)\n"
      "     + ((s30_C && s31_R == e3_4) && !(b_C == ESC)) ->\n"
      "         tau .\n"
      "         P(s30_C = false, b_C = ESC, s31_R = e0_4, b_R = b_C, b0_R = ESC)\n"
      "     + (s30_C && s31_R == e2_4) ->\n"
      "         tau .\n"
      "         P(s30_C = false, b_C = ESC, s31_R = e1_4, b_R = ESC, b0_R = b_C)\n"
      "     + (s31_R == e1_4) ->\n"
      "         s(b0_R) .\n"
      "         P(s31_R = e3_4, b_R = ESC, b0_R = ESC)\n"
      "     + (s31_R == e0_4) ->\n"
      "         s(b_R) .\n"
      "         P(s31_R = e3_4, b_R = ESC, b0_R = ESC)\n"
      "     + (s3_S == e0_3 && !s30_C) ->\n"
      "         tau .\n"
      "         P(s3_S = e2_3, b_S = ESC, s30_C = true, b_C = b_S)\n"
      "     + (s3_S == e1_3 && !s30_C) ->\n"
      "         tau .\n"
      "         P(s3_S = e0_3, s30_C = true, b_C = ESC)\n"
      "     + delta;\n"
      "init P(e2_3, ESC, false, ESC, e3_4, ESC, ESC);\n"
  );
  run_confluence_test_case(s,5);
}


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
