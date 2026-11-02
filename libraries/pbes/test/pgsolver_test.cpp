// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pgsolver_test.cpp
/// \brief Tests for reading and writing BESs in PGSolver format.

#define BOOST_TEST_MODULE pgsolver_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pbes/detail/pbessolve_algorithm.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pgsolver_io.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

namespace
{

const std::string GAME_BODY = "0 2 0 1 \"undef\";\n"
                              "1 2 1 2, 3 \"&\";\n"
                              "2 2 1 4, 5 \"&\";\n"
                              "4 2 1 5, 7, 8 \"&\";\n"
                              "7 2 1 5, 10 \"&\";\n"
                              "8 2 1 5, 11 \"&\";\n"
                              "10 2 1 5, 13 \"&\";\n"
                              "11 2 1 5, 14 \"&\";\n"
                              "13 2 1 5, 16 \"&\";\n"
                              "14 2 1 5, 17 \"&\";\n"
                              "16 2 1 5, 20, 21 \"&\";\n"
                              "17 2 1 2, 5 \"&\";\n"
                              "20 2 1 1, 5 \"&\";\n"
                              "21 2 1 5, 24 \"&\";\n"
                              "24 2 1 5, 7 \"&\";\n"
                              "3 1 0 6 \"undef\";\n"
                              "5 1 0 31 \"undef\";\n"
                              "6 1 1 5, 9 \"&\";\n"
                              "9 1 0 12 \"undef\";\n"
                              "12 1 1 5, 15 \"&\";\n"
                              "15 1 1 18, 19 \"&\";\n"
                              "18 1 1 5, 22 \"&\";\n"
                              "19 1 1 5, 23 \"&\";\n"
                              "22 1 0 25 \"undef\";\n"
                              "23 1 0 26 \"undef\";\n"
                              "25 1 0 5 \"undef\";\n"
                              "26 1 1 5, 27 \"&\";\n"
                              "27 1 0 28 \"undef\";\n"
                              "28 1 1 5, 29 \"&\";\n"
                              "29 1 0 30 \"undef\";\n"
                              "30 1 1 3, 5 \"&\";\n"
                              "31 0 0 31 \"undef\";\n";

pbes parse(const std::string& text, bool maxpg = true)
{
  pbes result;
  parse_bes_pgsolver(result, text, maxpg);
  return result;
}

propositional_variable_instantiation X(std::size_t i)
{
  return propositional_variable_instantiation("X" + std::to_string(i));
}

void check_equation(const pbes_equation& eqn, const fixpoint_symbol& sigma, std::size_t id, const pbes_expression& rhs)
{
  BOOST_CHECK_EQUAL(eqn.symbol(), sigma);
  BOOST_CHECK_EQUAL(eqn.variable().name(), X(id).name());
  BOOST_CHECK_EQUAL(eqn.formula(), rhs);
}

void check_parse_error(const std::string& text, const std::string& expected_message)
{
  try
  {
    parse(text);
    BOOST_ERROR("expected a parse error for: " + text);
  }
  catch (const mcrl2::runtime_error& e)
  {
    BOOST_CHECK_MESSAGE(std::string(e.what()).find(expected_message) != std::string::npos,
      "message '" << e.what() << "' does not contain '" << expected_message << "'");
  }
}

void check_round_trip(const std::string& bes_text, bool maxpg)
{
  pbes original = txt2pbes(bes_text);
  std::stringstream stream;
  save_bes_pgsolver(original, stream, maxpg);

  pbes loaded;
  load_bes_pgsolver(loaded, stream, maxpg);
  BOOST_CHECK(loaded.is_well_typed());
  BOOST_CHECK_EQUAL(detail::pbessolve(loaded), detail::pbessolve(original));
}

} // namespace

BOOST_AUTO_TEST_CASE(test_headers)
{
  for (const std::string& header:
    {std::string(), std::string("parity 31;\n"), std::string("start 0;\n"), std::string("parity 31;\nstart 0;\n")})
  {
    pbes p = parse(header + GAME_BODY);
    BOOST_CHECK_EQUAL(p.equations().size(), 32U);
    BOOST_CHECK_EQUAL(p.initial_state(), X(0));
    BOOST_CHECK(p.is_well_typed());
  }

  BOOST_CHECK_EQUAL(parse("start 31;\n" + GAME_BODY).initial_state(), X(31));
}

BOOST_AUTO_TEST_CASE(test_structure)
{
  const std::string game = "parity 2;\n"
                           "start 1;\n"
                           "0 2 0 2, 1, 2 \"a\";\n"
                           "1 1 1 0;\n"
                           "2 0 1 2, 1;\n";

  pbes max = parse(game, true);
  BOOST_REQUIRE_EQUAL(max.equations().size(), 3U);
  check_equation(max.equations()[0], fixpoint_symbol::nu(), 0, or_(X(1), X(2)));
  check_equation(max.equations()[1], fixpoint_symbol::mu(), 1, X(0));
  check_equation(max.equations()[2], fixpoint_symbol::nu(), 2, and_(X(1), X(2)));
  BOOST_CHECK_EQUAL(max.initial_state(), X(1));

  pbes min = parse(game, false);
  BOOST_REQUIRE_EQUAL(min.equations().size(), 3U);
  check_equation(min.equations()[0], fixpoint_symbol::nu(), 2, and_(X(1), X(2)));
  check_equation(min.equations()[1], fixpoint_symbol::mu(), 1, X(0));
  check_equation(min.equations()[2], fixpoint_symbol::nu(), 0, or_(X(1), X(2)));
}

BOOST_AUTO_TEST_CASE(test_layout)
{
  // Comments, arbitrary whitespace, and no whitespace around separators.
  pbes p = parse("% a comment\n"
                 "parity 1 ; % trailing comment\r\n"
                 "\t1 0 1 0,1;0 1 0\n0 \"x;,\";\n"
                 "% final comment without newline");
  BOOST_REQUIRE_EQUAL(p.equations().size(), 2U);
  check_equation(p.equations()[0], fixpoint_symbol::mu(), 0, X(0));
  check_equation(p.equations()[1], fixpoint_symbol::nu(), 1, and_(X(0), X(1)));
  BOOST_CHECK_EQUAL(p.initial_state(), X(1));
}

BOOST_AUTO_TEST_CASE(test_errors)
{
  check_parse_error("", "contains no nodes");
  check_parse_error("parity 3;", "contains no nodes");
  check_parse_error("0 0 0 0", "line 1, column 8: expected ';'");
  check_parse_error("0 0 0 0;\n1 0 2 0;", "line 2, column 5: the owner must be 0 or 1");
  check_parse_error("0 0 0;", "expected a successor");
  check_parse_error("0 0 0 0,;", "expected a successor");
  check_parse_error("0 0 0 1;", "successor 1 of node 0 is not defined");
  check_parse_error("0 0 0 0;\n0 1 0 0;", "line 2, column 1: node 0 is defined more than once");
  check_parse_error("start 2;\n0 0 0 0;", "the initial node 2 is not defined");
  check_parse_error("0 0 0 0 \"name;", "unterminated node name");
  check_parse_error("0 0 0 0; x", "expected a node identifier");
  check_parse_error("parity;", "expected the maximal node identifier");
  check_parse_error("99999999999999999999999 0 0 0;", "is too large");
}

BOOST_AUTO_TEST_CASE(test_round_trip)
{
  const std::vector<std::string> bess = {
    "pbes mu X = X;\n init X;",
    "pbes nu X = X;\n init X;",
    "pbes mu X = Y;\n nu Y = X;\n init X;",
    "pbes nu Y = X;\n mu X = Y;\n init Y;",
    "pbes mu X1 = X2;\n nu X2 = X1 || X3;\n mu X3 = X4 && X5;\n nu X4 = X1;\n nu X5 = X1 || X3;\n init X1;",
    "pbes nu X1 = X2 && X1;\n mu X2 = X1 || X3;\n nu X3 = X3;\n init X1;",
    "pbes nu X1 = X2 && X1;\n mu X2 = X1;\n init X1;",
  };
  for (const std::string& bes: bess)
  {
    check_round_trip(bes, true);
    check_round_trip(bes, false);
  }
}

BOOST_AUTO_TEST_CASE(test_load_pbes)
{
  std::stringstream stream("1 1 0 0;\n0 0 1 0;\n");
  pbes p;
  load_pbes(p, stream, pbes_format_pgsolver());
  BOOST_REQUIRE_EQUAL(p.equations().size(), 2U);
  BOOST_CHECK_EQUAL(p.initial_state(), X(1));
}
