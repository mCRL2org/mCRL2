// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesfindabs_test.cpp
/// \brief Tests for the pbesfindabs tool: enumeration of all valid abstraction
///        sets of a PBES.

#include "mcrl2/pbes/tools/pbesfindabs.h"
#define BOOST_TEST_MODULE pbesfindabs_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pbes/txt2pbes.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace mcrl2;
using namespace pbes_system;

namespace
{

struct test_result
{
  std::size_t valid_count = 0;
  std::string contents;
};

test_result run_findabs(const std::string& text,
  const std::string& test_name,
  std::size_t threads = 1,
  double timeout = 0.0,
  bool always_timeout = false)
{
  pbes p = txt2pbes(text, false);
  algorithms::normalize(p);

  pbesfindabs_options options;
  options.output_file = (std::filesystem::temp_directory_path() / ("pbesfindabs_" + test_name + ".txt")).string();
  options.number_of_threads = threads;
  options.timeout = timeout;

  // The engine delegates the actual checking to a pluggable checker; the tool
  // installs one backed by worker processes, while the tests check in-process.
  // always_timeout stands in for a set whose worker never finishes in time.
  pbesfindabs_options snapshot = options;
  options.checker
    = [&p, snapshot, always_timeout](const std::vector<abstractable_parameter>& /*universe*/,
        const std::vector<std::vector<std::size_t>>& batch,
        bool is_over)
  {
    std::vector<check_outcome> outcomes;
    outcomes.reserve(batch.size());
    for (const std::vector<std::size_t>& set: batch)
    {
      if (always_timeout)
      {
        outcomes.push_back(skipped_t{});
        continue;
      }
      outcomes.push_back(pbesfindabs_engine::check_one_set(p, snapshot, set, is_over));
    }
    return outcomes;
  };

  pbesfindabs_engine engine;
  test_result result;
  result.valid_count = engine.run(p, options);

  std::ifstream in(options.output_file);
  std::ostringstream stream;
  stream << in.rdbuf();
  result.contents = stream.str();
  return result;
}

// Count the set blocks in the output file.
std::size_t count_sets(const std::string& contents)
{
  std::size_t count = 0;
  std::istringstream stream(contents);
  std::string line;
  while (std::getline(stream, line))
  {
    if (line.rfind("# Abstraction set ", 0) == 0)
    {
      ++count;
    }
  }
  return count;
}

std::string join_lines(std::vector<std::string> lines)
{
  std::sort(lines.begin(), lines.end());
  std::ostringstream key;
  for (std::size_t i = 0; i < lines.size(); ++i)
  {
    key << (i == 0 ? "" : "|") << lines[i];
  }
  return key.str();
}

// Parse the output file into one key per set block: the equation lines of the
// block, sorted and joined with '|'. The empty set yields the empty key.
std::vector<std::string> parse_set_keys(const std::string& contents)
{
  std::vector<std::string> keys;
  std::vector<std::string> block;
  bool in_block = false;
  std::istringstream stream(contents);
  std::string line;
  while (std::getline(stream, line))
  {
    if (line.rfind("# Abstraction set ", 0) == 0)
    {
      if (in_block)
      {
        keys.push_back(join_lines(block));
      }
      block.clear();
      in_block = true;
    }
    else if (!line.empty() && line[0] != '#')
    {
      block.push_back(line);
    }
  }
  if (in_block)
  {
    keys.push_back(join_lines(block));
  }
  std::sort(keys.begin(), keys.end());
  return keys;
}

} // namespace

// nu equation with answer true: only under-approximations are checked and
// every abstraction set of this PBES proves true.
BOOST_AUTO_TEST_CASE(test_true_under_only)
{
  test_result result
    = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b)); init X(false, true);", "true_under_only");
  BOOST_CHECK_EQUAL(result.valid_count, 4);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 4);
  BOOST_CHECK(result.contents.find("X: a b") != std::string::npos);
}

// mu equation over Bool with answer false: only over-approximations are
// checked and {n} proves false.
BOOST_AUTO_TEST_CASE(test_false_over_only)
{
  test_result result = run_findabs("pbes mu X(n: Bool) = (val(n) && X(false)); init X(true);", "false_over_only");
  BOOST_CHECK_EQUAL(result.valid_count, 2);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 2);
  BOOST_CHECK(result.contents.find("X: n") != std::string::npos);
}

// The set {b} is not data closed because b occurs in the guard of a transition
// that keeps parameter a concrete. It is reported as not valid without solving.
// Not-data-closedness is NOT upward closed, so the superset {a, b} is still
// explored (it is data closed and its under-approximation proves true).
BOOST_AUTO_TEST_CASE(test_not_data_closed)
{
  test_result result
    = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(b, b)); init X(false, false);", "not_data_closed");
  BOOST_CHECK_EQUAL(result.valid_count, 3);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 3);
  BOOST_CHECK(result.contents.find("X: a b") != std::string::npos);
  // {b} alone is not data closed and must never be written.
  BOOST_CHECK(result.contents.find("X: b\n") == std::string::npos);
}

// Sets whose approximation does not decide the original answer are invalid:
// without recursion, abstracting any parameter makes the under-approximation
// false, so only the empty set is valid.
BOOST_AUTO_TEST_CASE(test_inconclusive)
{
  test_result result = run_findabs("pbes mu X(a: Bool, b: Bool) = val(a && b); init X(true, true);", "inconclusive");
  BOOST_CHECK_EQUAL(result.valid_count, 1);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 1);
  BOOST_CHECK(result.contents.find("X:") == std::string::npos);
}

// Running with multiple threads must produce the same valid sets.
BOOST_AUTO_TEST_CASE(test_multithreaded)
{
  test_result single
    = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b)); init X(false, true);", "mt_single");
  test_result parallel
    = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b)); init X(false, true);", "mt_parallel", 4);
  BOOST_CHECK_EQUAL(single.valid_count, 4);
  BOOST_CHECK_EQUAL(parallel.valid_count, single.valid_count);
}

// Running with a generous timeout on several threads must behave exactly like
// no timeout at all: every isolated check completes and its verdict is used.
BOOST_AUTO_TEST_CASE(test_timeout_generous)
{
  test_result result = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b)); init X(false, true);",
    "timeout_generous",
    4,
    3600.0);
  BOOST_CHECK_EQUAL(result.valid_count, 4);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 4);
}

// When every isolated check is abandoned (the child would be terminated), the
// non-empty sets are skipped; only the empty set, written without solving,
// survives. Since a skipped set prunes nothing, the enumeration still walks all
// levels and terminates.
BOOST_AUTO_TEST_CASE(test_timeout_always_abandoned)
{
  test_result result = run_findabs("pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b)); init X(false, true);",
    "timeout_abandoned",
    1,
    20.0,
    true);
  BOOST_CHECK_EQUAL(count_sets(result.contents), result.valid_count);
  BOOST_CHECK_EQUAL(result.valid_count, 1u);
}

// Regression test on the minisluice temperature/red-green PBES (two nu
// equations Z and X0 with parameters s3_P, s_P, col_P1, col_P2 and global
// sluice variables). The original PBES solves to true, so only
// under-approximations can prove it. The run exercises all three verdicts:
//   - {Z: col_P2} is not data closed, because col_P2 occurs in the concrete
//     parameter of the call to X0 in the first conjunct of Z's equation;
//   - its superset {Z: col_P2, X0: col_P2} is data closed and valid. It is
//     only found because supersets of not-data-closed sets keep being
//     explored; with naive pruning by invalidity it would have been skipped;
//   - {X0: s3_P} and {X0: col_P1} are blocked: they are data closed, but
//     their under-approximation does not prove true. Every superset of them
//     is therefore pruned and none may contain "X0: s3_P" or "X0: col_P1"
//     as an abstraction of the corresponding equation.
BOOST_AUTO_TEST_CASE(test_sluice_superset_and_blocking)
{
  const std::string sluice = R"(sort Colour = struct red | green;
     Sluice = struct s1 | s2;
     Enum3 = struct e2_3 | e1_3 | e0_3;

map  init_colour: Sluice -> Colour;

var  s,t0: Sluice;
     b: Bool;
     f,g: Sluice -> Colour;
eqn  init_colour(s)  =  red;
     if(b, f, g)(t0)  =  if(b, f(t0), g(t0));

glob dc,dc1,dc2,dc3,dc4: Sluice;

pbes nu Z(s3_P: Enum3, s_P: Sluice, col_P1,col_P2: Colour) =
       (val(!(s_P == s1)) || val(!(s3_P == e1_3)) || X0(e2_3, dc3, if(s_P == s1, red, col_P1), if(s_P == s2, red, col_P2))) && (val(!(s3_P == e0_3)) || Z(e2_3, dc4, if(s_P == s1, green, col_P1), if(s_P == s2, green, col_P2))) && (val(!(s3_P == e1_3)) || Z(e2_3, dc3, if(s_P == s1, red, col_P1), if(s_P == s2, red, col_P2))) && (val(!(s3_P == e2_3 && !(col_P1 == green))) || Z(e2_3, dc1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P1 == green)) || Z(e1_3, s1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P2 == green))) || Z(e2_3, dc1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P2 == green)) || Z(e1_3, s2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P1 == red))) || Z(e2_3, dc2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P1 == red)) || Z(e0_3, s1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P2 == red))) || Z(e2_3, dc2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P2 == red)) || Z(e0_3, s2, col_P1, col_P2));
     nu X0(s3_P: Enum3, s_P: Sluice, col_P1,col_P2: Colour) =
       (val(!(s_P == s1)) || val(!(s3_P == e1_3))) && (val(s_P == s1) || val(!(s3_P == e0_3)) || X0(e2_3, dc4, if(s_P == s1, green, col_P1), if(s_P == s2, green, col_P2))) && (val(!(s3_P == e1_3)) || X0(e2_3, dc3, if(s_P == s1, red, col_P1), if(s_P == s2, red, col_P2))) && (val(!(s3_P == e2_3 && !(col_P1 == green))) || X0(e2_3, dc1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P1 == green)) || X0(e1_3, s1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P2 == green))) || X0(e2_3, dc1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P2 == green)) || X0(e1_3, s2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P1 == red))) || X0(e2_3, dc2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P1 == red)) || X0(e0_3, s1, col_P1, col_P2)) && (val(!(s3_P == e2_3 && !(col_P2 == red))) || X0(e2_3, dc2, col_P1, col_P2)) && (val(!(s3_P == e2_3 && col_P2 == red)) || X0(e0_3, s2, col_P1, col_P2));

init Z(e2_3, dc, red, red);
)";

  const std::vector<std::string> expected_keys = {
    "",
    "Z: s3_P",
    "X0: col_P2",
    "X0: col_P2|Z: s3_P",
    "X0: col_P2|Z: col_P2",
    "X0: col_P2|Z: s3_P col_P2",
  };

  test_result result = run_findabs(sluice, "sluice");
  BOOST_CHECK_EQUAL(result.valid_count, 6);
  std::vector<std::string> expected = expected_keys;
  std::sort(expected.begin(), expected.end());
  const std::vector<std::string> actual = parse_set_keys(result.contents);
  BOOST_CHECK_EQUAL(actual.size(), expected.size());
  for (std::size_t i = 0; i < expected.size() && i < actual.size(); ++i)
  {
    BOOST_CHECK_EQUAL(actual[i], expected[i]);
  }

  // No superset of the blocked sets {X0: s3_P} or {X0: col_P1} may appear,
  // and the not-data-closed singletons {Z: s_P}, {Z: col_P1}, {Z: col_P2} and
  // {X0: s_P} must not be written themselves.
  const std::vector<std::string> keys = parse_set_keys(result.contents);
  for (const std::string& key: keys)
  {
    BOOST_CHECK(key.find("X0: s3_P") == std::string::npos);
    BOOST_CHECK(key.find("X0: col_P1") == std::string::npos);
    BOOST_CHECK(key != "Z: s_P");
    BOOST_CHECK(key != "Z: col_P1");
    BOOST_CHECK(key != "Z: col_P2");
    BOOST_CHECK(key != "X0: s_P");
  }
}

// mu X(a,b) = (a || b) || X(a,b) with init X(false,false) solves to false, so
// only over-approximations are checked. Both singletons are data closed (the
// recursive call passes the abstracted variables at abstracted indices), but
// abstracting a or b over-approximates the guard (a || b) to true, turning the
// equation into mu X = true, which cannot prove false. Both singletons are
// therefore blocked, the level stops after level 1, and only the empty set is
// valid.
BOOST_AUTO_TEST_CASE(test_mu_blocked_singletons)
{
  test_result result
    = run_findabs("pbes mu X(a,b : Bool) = (a || b) || X(a,b); init X(false, false);", "mu_blocked_singletons");
  BOOST_CHECK_EQUAL(result.valid_count, 1);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 1);
  BOOST_CHECK(result.contents.find("X:") == std::string::npos);
}

// Dually, nu X(a,b) = (a || b) || X(a,b) with the same initial state solves to
// true through the self-loop. Abstracting any parameter only drops the
// constant guard, leaving nu X = X which still proves true, so all four
// abstraction sets are valid.
BOOST_AUTO_TEST_CASE(test_nu_all_valid)
{
  test_result result = run_findabs("pbes nu X(a,b : Bool) = (a || b) || X(a,b); init X(false, false);", "nu_all_valid");
  BOOST_CHECK_EQUAL(result.valid_count, 4);
  BOOST_CHECK_EQUAL(count_sets(result.contents), 4);
  BOOST_CHECK(result.contents.find("X: a b") != std::string::npos);
}
