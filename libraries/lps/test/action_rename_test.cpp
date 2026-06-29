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
#include <set>

using namespace mcrl2;
using lps::stochastic_specification;
using lps::action_rename_specification;
using lps::action_summand;
using lps::action_summand_vector;
using lps::deadlock_summand;

inline
std::multiset<std::string> multiaction_names(const lps::multi_action& ma)
{
  std::multiset<std::string> result;
  for (const auto& a : ma.actions())
  {
    result.insert(std::string(a.label().name()));
  }
  return result;
}

inline
bool has_action_label(const stochastic_specification& spec, const std::string& name)
{
  return std::any_of(spec.action_labels().begin(), spec.action_labels().end(),
    [&name](const process::action_label& al){ return std::string(al.name()) == name; });
}

// Check whether a summand with the given names in its multi-action is present in the specification.
// We use multisets to correctly handle duplicate action names and avoid ordering issues.
inline
bool has_summand_with_multiaction(const stochastic_specification& spec, std::initializer_list<std::string> names)
{
  const std::multiset<std::string> expected(names);
  return std::any_of(spec.process().action_summands().begin(), spec.process().action_summands().end(),
    [&expected](const action_summand& as){ return multiaction_names(as.multi_action()) == expected; });
}

BOOST_AUTO_TEST_CASE(multiple_rename_rules_per_action)
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
  BOOST_CHECK_EQUAL(new_spec.process().summand_count(), 3u);
}

BOOST_AUTO_TEST_CASE(new_declarations_in_rename_spec)
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
  BOOST_CHECK_EQUAL(new_spec.process().summand_count(), 2u);
}

BOOST_AUTO_TEST_CASE(rename_constant_to_delta)
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
  BOOST_CHECK_EQUAL(new_spec.process().summand_count(), 2u);
}

BOOST_AUTO_TEST_CASE(conditional_rename_to_tau)
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
  BOOST_CHECK_EQUAL(new_spec.process().summand_count(), 2u);
}

BOOST_AUTO_TEST_CASE(partial_rename_to_delta)
{
  // Test whether partial renaming to delta is going well. See bug report #1009.
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
  BOOST_CHECK_EQUAL(new_spec.process().summand_count(), 8u);
}

// Check whether renaming with a regular expression works well
BOOST_AUTO_TEST_CASE(regex_rename)
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out and ac_out, leaving the rest
  stochastic_specification new_spec = action_rename(std::regex("^([^b]*)_out"), "out_$1", spec);

  BOOST_CHECK(check_well_typedness(new_spec));
  BOOST_CHECK_EQUAL(new_spec.action_labels().size(), 5u);
  BOOST_CHECK( has_action_label(new_spec, "out_a"));
  BOOST_CHECK( has_action_label(new_spec, "b_out"));
  BOOST_CHECK( has_action_label(new_spec, "cout"));
  BOOST_CHECK( has_action_label(new_spec, "ab_out"));
  BOOST_CHECK( has_action_label(new_spec, "out_ac"));
  BOOST_CHECK(!has_action_label(new_spec, "a_out"));
  BOOST_CHECK(!has_action_label(new_spec, "ac_out"));

  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"ab_out", "out_a"}));
  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"b_out"}));
  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"cout"}));
}

// Check whether renaming some actions to delta works
BOOST_AUTO_TEST_CASE(regex_rename_to_delta)
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out, leaving the rest
  stochastic_specification new_spec = action_rename(std::regex("^a_out"), "delta", spec);

  BOOST_CHECK_EQUAL(new_spec.action_labels().size(), 4u);
  BOOST_CHECK( has_action_label(new_spec, "b_out"));
  BOOST_CHECK( has_action_label(new_spec, "cout"));
  BOOST_CHECK( has_action_label(new_spec, "ab_out"));
  BOOST_CHECK( has_action_label(new_spec, "ac_out"));  // declared but unused; kept since it was not renamed
  BOOST_CHECK(!has_action_label(new_spec, "a_out"));

  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"b_out"}));
  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"cout"}));

  BOOST_CHECK_EQUAL(new_spec.process().deadlock_summands().size(), 2u);
  auto find_result = std::find_if(spec.process().action_summands().begin(), spec.process().action_summands().end(),
    [](const action_summand& as)
    {
      return multiaction_names(as.multi_action()) == std::multiset<std::string>{"a_out", "ab_out"};
    });
  BOOST_CHECK(find_result != spec.process().action_summands().end());
  if(find_result != spec.process().action_summands().end())
  {
    BOOST_CHECK(std::any_of(new_spec.process().deadlock_summands().begin(), new_spec.process().deadlock_summands().end(),
                                [&find_result](const deadlock_summand& ds){ return ds.condition() == find_result->condition(); }));
  }
}

// Check whether renaming some actions to tau works
BOOST_AUTO_TEST_CASE(regex_rename_to_tau)
{
  const std::string SPEC =
  "act a_out, b_out, cout, ab_out, ac_out;\n"
  "init a_out|ab_out . b_out . cout . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename a_out and cout, leaving the rest
  stochastic_specification new_spec = action_rename(std::regex("^(a_out|cout)$"), "tau", spec);

  BOOST_CHECK_EQUAL(new_spec.action_labels().size(), 3u);
  BOOST_CHECK( has_action_label(new_spec, "b_out"));
  BOOST_CHECK( has_action_label(new_spec, "ab_out"));
  BOOST_CHECK( has_action_label(new_spec, "ac_out"));
  BOOST_CHECK(!has_action_label(new_spec, "a_out"));
  BOOST_CHECK(!has_action_label(new_spec, "cout"));

  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"ab_out"}));
  BOOST_CHECK(has_summand_with_multiaction(new_spec, {"b_out"}));
  // tau is represented as an empty multi-action (no action names)
  BOOST_CHECK(has_summand_with_multiaction(new_spec, {}));
}

// Check whether the list of actions contains no duplicates after renaming multiple actions
// to one action.
BOOST_AUTO_TEST_CASE(regex_rename_no_duplicate_labels)
{
  const std::string SPEC =
  "act a_out, b_out;\n"
  "init a_out . b_out . delta;";

  stochastic_specification spec = lps::linearise(SPEC);
  // This should rename both actions to 'out'
  stochastic_specification new_spec = action_rename(std::regex("^(a|b)_out$"), "out", spec);

  BOOST_CHECK_EQUAL(new_spec.action_labels().size(), 1u);
  BOOST_CHECK(has_action_label(new_spec, "out"));
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

  BOOST_CHECK_EQUAL(new_spec.action_labels().size(), 4u);
}
