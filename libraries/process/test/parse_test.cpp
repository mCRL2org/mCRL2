// Author(s): Jeroen Keiren, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Regression test for parsing process expressions

#include <iostream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/process/parse.h"

using namespace mcrl2;
using namespace mcrl2::process;

std::string DATA_DECL =
  "glob                   \n"
  "  m: Nat;              \n"
  "                       \n"
  "act                    \n"
  "  a: Nat;              \n"
  ;

std::string PROC_DECL =
  "proc P(n:Nat);         \n"
  ;

BOOST_AUTO_TEST_CASE(test_parse_process_specification)
{
  std::string text =
    "act  a: Nat;                     \n"
    "                                 \n"
    "glob v: Nat;                     \n"
    "                                 \n"
    "proc P(i,j: Nat) = a(i).P(1, 1); \n"
    "                                 \n"
    "init P(i = 1, j = v);            \n"
    ;

  process_specification p = parse_process_specification(text);
  std::cout << p << std::endl;
}

BOOST_AUTO_TEST_CASE(test_parse)
{
  process_expression x = parse_process_expression("a(m).P(0)", DATA_DECL, PROC_DECL);
  BOOST_CHECK(process::pp(x) == "a(m) . P(0)");
  test_parse_process_specification();
}

BOOST_AUTO_TEST_CASE(test_actdecl)
{
  std::string text = "a: Bool -> Bool;";
  action_label_list l = parse_action_declaration(text);
}

BOOST_AUTO_TEST_CASE(test_stochastic_operator)
{
  std::string text =
    "act  throw: Bool;                   \n"
    "                                    \n"
    "proc P = dist b:Bool[1/2].throw(b); \n"
    "                                    \n"
    "init P;                             \n"
    ;
  process_specification p = parse_process_specification(text);
  std::cout << p << std::endl;

  text =
    "proc P = sum x:Real.(x==x) -> tau.P;\n"
    "init P;\n"
    ;
  p = parse_process_specification(text);
  std::cout << p << std::endl;
}

template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
void test_parse_process_expression(const std::string& text,
                                   const VariableContainer& variables = VariableContainer(),
                                   const data::data_specification& dataspec = data::data_specification(),
                                   const ActionLabelContainer& action_labels = std::vector<action_label>(),
                                   const ProcessIdentifierContainer& process_identifiers = ProcessIdentifierContainer()
                                  )
{
  process_expression x = parse_process_expression(text, variables, dataspec, action_labels, process_identifiers);
  std::string text1 = process::pp(x);
  BOOST_CHECK_EQUAL(text, text1);
}

BOOST_AUTO_TEST_CASE(parse_process_expression_test)
{
  std::string text =
    "act  a, b, c;                    \n"
    "                                 \n"
    "glob n: Nat;                     \n"
    "                                 \n"
    "proc P(b: Bool) = delta;         \n"
    "proc Q(m: Nat) = delta;          \n"
    "                                 \n"
    "init delta;                      \n"
    ;
  process_specification procspec = parse_process_specification(text);
  std::vector<process_identifier> process_identifiers;
  for (const process_equation& eqn: procspec.equations())
  {
    process_identifiers.push_back(eqn.identifier());
  }
  test_parse_process_expression("a", procspec.global_variables(), procspec.data(), procspec.action_labels(), process_identifiers);
  test_parse_process_expression("a . P(true)", procspec.global_variables(), procspec.data(), procspec.action_labels(), process_identifiers);
  test_parse_process_expression("a . Q(n)", procspec.global_variables(), procspec.data(), procspec.action_labels(), process_identifiers);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return EXIT_SUCCESS;
}
