// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_communication_test.cpp
/// \brief Test for applying communication operator

#define BOOST_TEST_MODULE linearise_communication_test
#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/included/unit_test.hpp>

#include "../../process/include/mcrl2/process/detail/alphabet_parse.h"
#include "../include/mcrl2/lps/linearise_communication.h"
#include "../include/mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::process;
using namespace mcrl2::lps;

struct LogDebug
{
  LogDebug()
  {
    log::logger::set_reporting_level(log::debug);
  }
};
BOOST_GLOBAL_FIXTURE(LogDebug);

// The following data specification and multiactions are for a large testcase that is extracted from a real-life case
// study (translating Cordis models to mCRL2). Calculating communication expressions, especially on the large sets
// used to take close to 10 minutes.
inline
data::data_specification data_spec()
{
  data::data_specification data_spec = data::detail::default_specification();
  data_spec.add_sort(data::basic_sort("D"));
  for (std::size_t i = 0; i < 48; ++i)
  {
    data::sort_expression s = (16 <= i && i <= 25?data::sort_bool::bool_():data::basic_sort("D"));
    data_spec.add_mapping(data::function_symbol("d"+ std::to_string(i), s));
  }
  return data_spec;
}

inline
action_label_list action_declarations()
{
  const std::vector<std::size_t> bool_indices({22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
  const std::vector<std::size_t> d_indices({1, 4, 5, 9, 11, 13, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 56, 57, 64, 83});
  action_label_list result;
  for (std::size_t i = 0; i < 6; ++i)
  {
    result.emplace_front(core::identifier_string("b" + std::to_string(i)), data::sort_expression_list());
  }

  for (std::size_t i = 0; i < 84; ++i)
  {
    data::sort_expression_list s;
    if (std::find(d_indices.begin(), d_indices.end(), i) != d_indices.end()) { s.push_front(data::basic_sort("D")); }
    else if (std::find(bool_indices.begin(), bool_indices.end(), i) != bool_indices.end()) { s.push_front(data::sort_bool::bool_()) ;}

    result.emplace_front(core::identifier_string("a" + std::to_string(i)), s);
    result.emplace_front(core::identifier_string("a" + std::to_string(i) + "_r"), s);
    result.emplace_front(core::identifier_string("a" + std::to_string(i) + "_s"), s);
  }
  return result;
}

inline
action_name_multiset_list allow_set()
{
  const std::string allow_string("{ b1, b2, b3, a1 | a4 | a5 | a9 | a10 | a11 | a13 | a56 | a57 | a64, a1 | a4 | a5 | a9 | a11 | a13 | a56 | a57 | a64, a1 | a4 | a5 | a9 | a11 | a13 | a64, a1 | a8 | a64, a1 | a8 | a64 | a63, a1 | a11 | a64, a1 | a12 | a64, a1 | a20 | a64, a1 | a64, a1 | a64 | a63, a1 | a64 | a63 | a65, a2, a3, a4 | a5 | a9 | a20 | a64, a4 | a5 | a9 | a40 | a54 | a64 | a72, a4 | a5 | a13 | a20 | a64, a4 | a5 | a13 | a39 | a52 | a64 | a71, a4 | a9 | a20 | a64, a4 | a9 | a54 | a64 | a70, a4 | a20 | a64, a4 | a20 | a64 | a73, a4 | a39 | a52 | a64, a4 | a39 | a52 | a64 | a71, a4 | a52 | a64, a4 | a52 | a64 | a68, a5 | a13 | a20 | a64, a5 | a13 | a52 | a64 | a69, a5 | a20 | a64, a5 | a20 | a64 | a74, a5 | a40 | a54 | a64, a5 | a40 | a54 | a64 | a72, a5 | a54 | a64, a5 | a54 | a64 | a68, a6, a7, a8 | a20 | a52 | a64 | a63 | a76, a8 | a20 | a54 | a64 | a63 | a76, a8 | a20 | a64 | a63, a8 | a20 | a64 | a63 | a76, a9 | a20 | a64, a9 | a52 | a64, a13 | a20 | a64, a13 | a54 | a64, a14, a15, a16, a17, a18, a19, a20, a20 | a52 | a54 | a64 | a63 | a65, a20 | a52 | a64 | a63 | a65, a20 | a54 | a64 | a63 | a65, a20 | a64, a20 | a64 | a63 | a65, a20 | a64 | a66 | a67, a20 | a64 | a74, a21, a22 | a23 | a24 | a25 | a26 | a27 | a28 | a29 | a30 | a31 | a32 | a33 | a34 | a35 | a36 | a37 | a38 | a39 | a40 | a41, a22 | a32, a23 | a33, a27 | a37, a28 | a38, a29 | a39, a30 | a40, a31 | a41, a39 | a52 | a64, a40 | a54 | a64, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52 | a64, a52 | a64 | a68, a52 | a64 | a71, a53, a54 | a64, a54 | a64 | a68, a54 | a64 | a72, a55, a58, a59, a60, a61, a62, a64, a64 | a63, a64 | a63 | a65, a64 | a66 | a67, a75, a77, a78, a79, a80, a81, a82, a83, b4, b5 }");
  action_name_multiset_list result = process::detail::parse_allow_set(allow_string);
  result = sort_multi_action_labels(result);
  return result;
}

inline
communication_expression_list comm_set()
{
  const std::string comm_string("{ a1_r | a1_s -> a1, a2_r | a2_s -> a2, a3_r | a3_s -> a3, a4_r | a4_s -> a4, a5_r | a5_s -> a5,"
    "a6_r | a6_s -> a6, a7_r | a7_s -> a7, a8_r | a8_s -> a8, a9_r | a9_s -> a9, a10_r | a10_s -> a10, a11_r | a11_s -> a11,"
    "a12_r | a12_s -> a12, a13_r | a13_s -> a13, a14_r | a14_s -> a14, a15_r | a15_s -> a15, a16_r | a16_s -> a16, a17_r | a17_s -> a17,"
    "a18_r | a18_s -> a18, a19_r | a19_s -> a19, a20_r | a20_s -> a20, a21_r | a21_s -> a21, a22_r | a22_s -> a22, a23_r | a23_s -> a23,"
    "a24_r | a24_s -> a24, a25_r | a25_s -> a25, a26_r | a26_s -> a26, a27_r | a27_s -> a27, a28_r | a28_s -> a28, a29_r | a29_s -> a29,"
    "a30_r | a30_s -> a30, a31_r | a31_s -> a31, a32_r | a32_s -> a32, a33_r | a33_s -> a33, a34_r | a34_s -> a34, a35_r | a35_s -> a35,"
    "a36_r | a36_s -> a36, a37_r | a37_s -> a37, a38_r | a38_s -> a38, a39_r | a39_s -> a39, a40_r | a40_s -> a40, a41_r | a41_s -> a41,"
    "a42_r | a42_s -> a42, a43_r | a43_s -> a43, a44_r | a44_s -> a44, a45_r | a45_s -> a45, a46_r | a46_s -> a46, a47_r | a47_s -> a47,"
    "a48_r | a48_s -> a48, a49_r | a49_s -> a49, a50_r | a50_s -> a50, a51_r | a51_s -> a51, a52_r | a52_s -> a52, a53_r | a53_s -> a53,"
    "a54_r | a54_s -> a54, a55_r | a55_s -> a55, a56_r | a56_s -> a56, a57_r | a57_s -> a57, a58_r | a58_s -> a58, a59_r | a59_s -> a59,"
    "a60_r | a60_s -> a60, a61_r | a61_s -> a61, a62_r | a62_s -> a62, a63_r | a63_s -> a63, a64_r | a64_s -> a64, a65_r | a65_s -> a65,"
    "a66_r | a66_s -> a66, a67_r | a67_s -> a67, a68_r | a68_s -> a68, a69_r | a69_s -> a69, a70_r | a70_s -> a70, a71_r | a71_s -> a71,"
    "a72_r | a72_s -> a72, a73_r | a73_s -> a73, a74_r | a74_s -> a74, a75_r | a75_s -> a75, a76_r | a76_s -> a76, a77_r | a77_s -> a77,"
    "a78_r | a78_s -> a78, a79_r | a79_s -> a79, a80_r | a80_s -> a80, a81_r | a81_s -> a81, a82_r | a82_s -> a82, a83_r | a83_s -> a83 }");

  communication_expression_list result = process::detail::parse_comm_set(comm_string);
  result = sort_communications((result));
  return result;
}

inline
tuple_list filter_allow(const tuple_list& l, const action_name_multiset_list& allowed)
{
  tuple_list result;
  for (std::size_t i = 0; i < l.size(); ++i)
  {
    if (allow_(allowed, l.actions[i], action(action_label("Terminate", data::sort_expression_list()), data::data_expression_list())))
    {
      result.conditions.push_back(l.conditions[i]);
      result.actions.push_back(l.actions[i]);
    }
  }
  return result;
}

BOOST_AUTO_TEST_CASE(test_multact_19)
{
  const std::string multiact_19_string("a1_r(d1)|a1_s(d2)|a4_r(d3)|a4_s(d4)|a5_r(d5)|a5_s(d6)|a9_r(d7)|a9_s(d8)|a11_r(d9)|a11_s(d10)|a13_r(d9)|a13_s(d11)|a56_r(d12)|a56_s(d13)|a57_r(d9)|a57_s(d14)|a64_r(d15)|a64_s(d47)|a82_r");
  const multi_action multact_19 = parse_multi_action(multiact_19_string, action_declarations(), data_spec());
  const action_list actions = sort_actions(multact_19.actions());
  assert(std::is_sorted(actions.begin(), actions.end(), action_compare()));

  data::rewriter R(data_spec());

  action_name_multiset_list allow_set_ = allow_set();
  tuple_list result = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set_), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(result.size(), 0); // This shows we have a dramatic set size!

  allow_set_ = sort_multi_action_labels(process::detail::parse_allow_set("{ a1|a4|a5|a9|a11|a13|a56|a57|a64|a82_r }"));
  result = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set_), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(result.size(), 1);
  result = filter_allow(result, allow_set_);
  BOOST_CHECK_EQUAL(result.size(), 1);

  allow_set_ = sort_multi_action_labels(process::detail::parse_allow_set("{ a1|a4|a5|a9|a11|a13|a56|a57|a64|a82_r, a1|a4_r|a4_s|a5|a9|a11|a13|a56|a57|a64|a82_r }"));
  result = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set_), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(result.size(), 2);
  result = filter_allow(result, allow_set_);
  BOOST_CHECK_EQUAL(result.size(), 2);

  allow_set_ = sort_multi_action_labels(process::detail::parse_allow_set("{ a1|a4|a5|a9|a11|a13|a56|a57|a64|a82 }"));
  result = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set_), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(result.size(), 0);
  result = filter_allow(result, allow_set_);
  BOOST_CHECK_EQUAL(result.size(), 0);

}

BOOST_AUTO_TEST_CASE(test_multact_41a)
{
  const std::string multiact_41a_string("a22_r(d16)|a22_s(true)|a23_r(d17)|a23_s(true)|a24_r(d18)|a24_s(true)|a25_r(d19)|a25_s(true)|a26_r(d20)|a26_s(true)|a27_r(d21)|a27_s(true)|a28_r(d22)|a28_s(true)|a29_r(d23)|a29_s(true)|a30_r(d24)|a30_s(true)|a31_r(d25)|a31_s(true)|a32_r(d26)|a32_s(d27)|a33_r(d28)|a33_s(d29)|a34_r(d30)|a34_s(d31)|a35_r(d32)|a35_s(d33)|a36_r(d34)|a36_s(d35)|a37_r(d36)|a37_s(d37)|a38_r(d38)|a38_s(d39)|a39_r(d40)|a39_s(d41)|a40_r(d42)|a40_s(d43)|a41_r(d44)|a41_s(d45)|b4");
  const multi_action multact_41a = parse_multi_action(multiact_41a_string, action_declarations(), data_spec());

  const action_list actions = sort_actions(multact_41a.actions());
  assert(std::is_sorted(actions.begin(), actions.end(), action_compare()));

  data::rewriter R(data_spec());

  tuple_list S = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set()), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(S.size(), 1); // This shows we have a dramatic set size for communication result!
  tuple_list result = filter_allow(S, allow_set());
  BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_multact_41b)
{
  const std::string multiact_41b_string("a22_r(d16)|a22_s(true)|a23_r(d17)|a23_s(true)|a24_r(d18)|a24_s(true)|a25_r(d19)|a25_s(true)|a26_r(d20)|a26_s(true)|a27_r(d21)|a27_s(true)|a28_r(d22)|a28_s(true)|a29_r(d23)|a29_s(true)|a30_r(d24)|a30_s(true)|a31_r(d25)|a31_s(true)|a32_r(d26)|a32_s(d27)|a33_r(d28)|a33_s(d29)|a34_r(d30)|a34_s(d31)|a35_r(d32)|a35_s(d33)|a36_r(d34)|a36_s(d35)|a37_r(d36)|a37_s(d37)|a38_r(d38)|a38_s(d39)|a39_r(d40)|a39_s(d41)|a40_r(d42)|a40_s(d43)|a41_r(d44)|a41_s(d45)|a83_s(d46)");
  const multi_action multact_41b = parse_multi_action(multiact_41b_string, action_declarations(), data_spec());

  const action_list actions = sort_actions(multact_41b.actions());
  assert(std::is_sorted(actions.begin(), actions.end(), action_compare()));

  data::rewriter R(data_spec());

  tuple_list S = makeMultiActionConditionList(actions, comm_set(), get_actions(allow_set()), std::vector<core::identifier_string>(), true, R);
  BOOST_CHECK_EQUAL(S.size(), 0); // This shows we have a dramatic set size for communication result!
}

