// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_rename_test.cpp
/// \brief Action rename test.

#define BOOST_TEST_MODULE action_rename_test
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using lps::stochastic_specification;
using lps::action_rename_specification;
using lps::action_summand;
using lps::action_summand_vector;
using lps::deadlock_summand;

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

  stochastic_specification spec=lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);
  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK(new_spec.process().summand_count()==3);
}

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

  stochastic_specification spec=lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);
  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

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

  stochastic_specification spec=lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

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

  stochastic_specification spec=lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK(new_spec.process().summand_count()==2);
}

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


  stochastic_specification spec=lps::linearise(SPEC);
  std::istringstream ar_spec_stream(AR_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  data::rewriter R (spec.data(), mcrl2::data::rewriter::strategy());
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);
  lps::rewrite(new_spec, R);
  lps::remove_trivial_summands(new_spec);
  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK(new_spec.process().summand_count()==8);
}

// Check whether renaming with a regular expression works well
static void test_regex1()
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out and ac_out, leaving the rest
std::cerr << "ACTION LABESL1 " << pp(spec.action_labels()) << "\n";
  stochastic_specification new_spec = action_rename(std::regex("^([^b]*)_out"), "out_$1", spec);

  BOOST_CHECK(check_well_typedness(new_spec));
std::cerr << "ACTION LABESL2 " << pp(new_spec.action_labels()) << "\n";
  BOOST_CHECK(std::string(new_spec.action_labels().front().name()) == "out_a");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().front().name()) == "b_out");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().tail().front().name()) == "cout");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().tail().tail().front().name()) == "ab_out");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().tail().tail().tail().front().name()) == "out_ac");

  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "ab_out|out_a"; }));
  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "b_out"; }));
  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "cout"; }));
}

// Check whether renaming some actions to delta works
void test_regex2()
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out, leaving the rest
  stochastic_specification new_spec = action_rename(std::regex("^a_out"), "delta", spec);

  BOOST_CHECK(std::string(new_spec.action_labels().front().name()) == "b_out");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().front().name()) == "cout");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().tail().front().name()) == "ab_out");

  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "b_out"; }));
  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "cout"; }));

  BOOST_CHECK(new_spec.process().deadlock_summands().size() == 2);
  auto find_result = std::find_if(spec.process().action_summands().begin(), spec.process().action_summands().end(),
    [](const action_summand& as){ return lps::pp(as.multi_action()) == "a_out|ab_out"; });
  BOOST_CHECK(find_result != spec.process().action_summands().end());
  if(find_result != spec.process().action_summands().end())
  {
    BOOST_CHECK(std::any_of(new_spec.process().deadlock_summands().begin(), new_spec.process().deadlock_summands().end(),
                                [&find_result](const deadlock_summand& ds){ return ds.condition() == find_result->condition(); }));
  }
}

// Check whether renaming some actions to tau works
void test_regex3()
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out and cout, leaving the rest
  stochastic_specification new_spec = action_rename(std::regex("^(a_out|cout)$"), "tau", spec);

  BOOST_CHECK(std::string(new_spec.action_labels().front().name()) == "b_out");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().front().name()) == "ab_out");
  BOOST_CHECK(std::string(new_spec.action_labels().tail().tail().front().name()) == "ac_out");

  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "ab_out"; }));
  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "b_out"; }));
  BOOST_CHECK(std::any_of(new_spec.process().action_summands().begin(), new_spec.process().action_summands().end(),
                              [](const action_summand& as){ return lps::pp(as.multi_action()) == "tau"; }));
}

// Check whether the list of actions contains no duplicates after renaming multiple actions
// to one action.
void test_regex4()
{
  const std::string SPEC =
  "act a_out, b_out;\n"
  "init a_out . b_out . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename both actions to 'out'
  stochastic_specification new_spec = action_rename(std::regex("^(a|b)_out$"), "out", spec);

  BOOST_CHECK(new_spec.action_labels().size() == 1);
  BOOST_CHECK(std::string(new_spec.action_labels().front().name()) == "out");
}

BOOST_AUTO_TEST_CASE(multiple_action_declarations)
{
  const std::string SPEC =
     "act a1,a2;\n"
     "proc P = a1.a2.P;\n"
     "init P;\n";

  const std::string RENAME_SPEC =
    "act b1;\n"
    "rename a1 => b1;\n"
    "act b2;\n"
    "rename a2 => b2;\n";

  stochastic_specification spec = lps::linearise(SPEC);
  std::istringstream ar_spec_stream(RENAME_SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(ar_spec_stream, spec);
  stochastic_specification new_spec = action_rename(ar_spec,spec,data::rewriter(),false);

  BOOST_CHECK(new_spec.action_labels().size() == 4);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test1();
  test2();
  test3();
  test4();
  test5();

  test_regex1();
  test_regex2();
  test_regex3();
  test_regex4();
}
