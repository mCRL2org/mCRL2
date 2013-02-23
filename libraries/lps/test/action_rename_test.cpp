// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_rename_test.cpp
/// \brief Action rename test.

#include <boost/test/minimal.hpp>
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/remove.h"

using namespace mcrl2;
using lps::specification;
using lps::action_rename_specification;
// using lps::action_rename;

static
void test1()
{
  // Check a renaming when more than one renaming rule
  // for one action is present.
  const std::string SPEC =
    "act a:Nat;                               \n"
    "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
    "init P(0);                               \n";

  const std::string AR_SPEC =
    "act b,c:Nat;\n"
    "var n:Nat;\n"
    "rename \n"
    "  (n>4)  -> a(n) => b(n); \n"
    "  (n<22) -> a(n) => c(n); \n";

  specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  specification new_spec = action_rename(ar_spec,spec);
  BOOST_CHECK(new_spec.process().summand_count()==3);
}

static
void test2()
{
  // Check whether new declarations in the rename file
  // are read properly. Check for renamings of more than one action.
  const std::string SPEC =
    "act a,b:Nat;                             \n"
    "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
    "init P(0);                               \n";

  const std::string AR_SPEC =
    "map f:Nat->Nat; \n"
    "var n':Nat; \n"
    "eqn f(n')=3; \n"
    "act c:Nat; \n"
    "var n:Nat; \n"
    "rename \n"
    "  (f(n)>23) -> a(n) => b(n); \n"
    "  b(n) => c(n); \n";

  specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  specification new_spec = action_rename(ar_spec,spec);
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

static
void test3()
{
  // Check whether constants in an action_rename file are properly translated.
  const std::string SPEC =
    "act a,b:Nat;                             \n"
    "proc P(n:Nat) = a(1). P(n)+b(1).P(1);  \n"
    "init P(0);                               \n";

  const std::string AR_SPEC =
    "rename \n"
    "  a(1) => delta; \n";

  specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  specification new_spec = action_rename(ar_spec,spec);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

static
void test4()
{
  const std::string SPEC =
    "sort D = struct d1 | d2;\n"
    "     E = D;\n"
    "\n"
    "act a : E;\n"
    "\n"
    "proc P(d:D) = a(d) . P(d1);\n"
    "\n"
    "init P(d2);\n";

  const std::string AR_SPEC =
    "var e : E;\n"
    "rename\n"
    "(e==d1) -> a(e) => tau;\n";

  specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  specification new_spec = action_rename(ar_spec,spec);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

static
void test5() // Test whether partial renaming to delta is going well. See bug report #1009.
{
  const std::string SPEC =
    "sort Command = struct com1 | com2;\n"
    "sort State = struct st1 | st2;\n"
    "\n"
    "proc Parent(id: Nat, children: List(Nat)) =\n"
    "  sum child: Nat, command: Command . (child in children) -> sc(id, child, command) . Parent()\n"
    "+ sum child: Nat, state: State . (child in children) -> rs(child, id, state) . Parent();\n"
    "\n"
    "proc Child(id: Nat, parent: Nat) =\n"
    "  sum command: Command . rc(parent, id, command) . Child()\n"
    "+ sum state: State . ss(id, parent, state) . Child();\n"
    "\n"
    "act sc, rc, cc: Nat # Nat # Command;\n"
    "    rs, ss, cs: Nat # Nat # State;\n"
    "\n"
    "act none;\n"
    "\n"
    "init\n"
    "  allow({cc, cs, sc, rs},\n"
    "  comm({sc|rc -> cc, rs|ss->cs},\n"
    "    Parent(0, [1, 2]) ||\n"
    "    Child(1, 0) ||\n"
    "    Child(2, 0)\n"
    "  ));\n";


  const std::string AR_SPEC =
    "var c: Command;\n"
    "    s: State;\n"
    "rename\n"
    "  sc(0, 1, c) => delta;\n"
    "  rs(1, 0, s) => delta;\n";


  specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  specification new_spec = action_rename(ar_spec,spec);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(new_spec.process().summand_count()==8);
}


int test_main(int argc, char** argv)
{
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}
