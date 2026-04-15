// Author(s): GitHub Copilot
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE lts_combine_communication_test
#include <boost/test/included/unit_test.hpp>

#include "../lts_combine.h"

#include "mcrl2/data/nat.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/process/action_label.h"
#include "mcrl2/process/process_expression.h"

#include <string>

namespace
{

using namespace mcrl2;

lps::multi_action parse_label(const std::string& text,
                              const process::action_label_list& declarations,
                              const data::data_specification& data_spec)
{
  return lps::parse_multi_action(text, declarations, data_spec);
}

process::action_label_list declarations_no_data()
{
  return process::parse_action_declaration("a,b,c,d,e,f;");
}

process::action_label_list declarations_with_nat()
{
  return process::parse_action_declaration("a,b,c: Nat;");
}

} // namespace

BOOST_AUTO_TEST_CASE(test_no_communication_rule_unchanged)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a", act_decls, data_spec);
  const process::communication_expression_list comm_set;

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "a");
}

BOOST_AUTO_TEST_CASE(test_single_rule_exact_match)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|b", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "c");
}

BOOST_AUTO_TEST_CASE(test_single_rule_partial_match)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|b|d", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "c|d");
}

BOOST_AUTO_TEST_CASE(test_multiple_disjoint_rules_same_multiaction)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|b|d|e", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c, d|e -> f }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "c|f");
}

BOOST_AUTO_TEST_CASE(test_repeated_pairs)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|a|b|b", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "c|c");
}

BOOST_AUTO_TEST_CASE(test_equal_arguments_can_communicate)
{
  const auto act_decls = declarations_with_nat();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a(1)|b(1)", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "c(1)");
}

BOOST_AUTO_TEST_CASE(test_unequal_arguments_no_communication)
{
  const auto act_decls = declarations_with_nat();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a(1)|b(2)", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> c }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "a(1)|b(2)");
}

BOOST_AUTO_TEST_CASE(test_three_way_rule)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|b|c", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b|c -> d }");

  const lps::multi_action result = mcrl2::apply_communication(input, comm_set);
  BOOST_CHECK_EQUAL(lps::pp(result), "d");
}

BOOST_AUTO_TEST_CASE(test_tau_result_rule_is_rejected)
{
  const auto act_decls = declarations_no_data();
  const data::data_specification data_spec;

  const lps::multi_action input = parse_label("a|b", act_decls, data_spec);
  const process::communication_expression_list comm_set = process::detail::parse_comm_set("{ a|b -> tau }");

  BOOST_CHECK_THROW(mcrl2::apply_communication(input, comm_set), mcrl2::runtime_error);
}
