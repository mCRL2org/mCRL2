// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_structural_order_test.cpp
/// \brief Tests for the structural order on terms.

#define BOOST_TEST_MODULE aterm_structural_order_test
#include <boost/test/included/unit_test.hpp>

#include <algorithm>
#include <compare>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_structural_order.h"

using namespace atermpp;

static aterm t(const std::string& s)
{
  return read_term_from_string(s);
}

static std::string to_string(std::weak_ordering c)
{
  return std::is_lt(c) ? "less" : std::is_gt(c) ? "greater" : "equivalent";
}

// Checks that structural_compare(x, y) is expected, that structural_compare(y, x) is its inverse, and that
// structural_less is consistent with both.
static void check_compare(const aterm& x, const aterm& y, std::weak_ordering expected)
{
  const std::weak_ordering inverse = 0 <=> expected;
  BOOST_CHECK_MESSAGE(structural_compare(x, y) == expected,
      "structural_compare(" << x << ", " << y << ") is " << to_string(structural_compare(x, y)) << ", expected "
      << to_string(expected));
  BOOST_CHECK_MESSAGE(structural_compare(y, x) == inverse,
      "structural_compare(" << y << ", " << x << ") is " << to_string(structural_compare(y, x)) << ", expected "
      << to_string(inverse));
  BOOST_CHECK_EQUAL(structural_less()(x, y), std::is_lt(expected));
  BOOST_CHECK_EQUAL(structural_less()(y, x), std::is_gt(expected));
}

static void check_precedes(const aterm& x, const aterm& y)
{
  check_compare(x, y, std::weak_ordering::less);
}

static void check_equal(const aterm& x, const aterm& y)
{
  check_compare(x, y, std::weak_ordering::equivalent);
}

static void check_succeeds(const aterm& x, const aterm& y)
{
  check_compare(x, y, std::weak_ordering::greater);
}

BOOST_AUTO_TEST_CASE(test_undefined)
{
  check_equal(aterm(), aterm());
  check_precedes(aterm(), aterm_int(0));
  check_precedes(aterm(), t("a"));
  check_succeeds(t("a"), aterm());
}

BOOST_AUTO_TEST_CASE(test_function_symbols)
{
  check_equal(t("a"), t("a"));
  check_precedes(t("a"), t("b"));
  check_succeeds(t("b"), t("a"));

  // The name is compared before the arguments.
  check_precedes(t("f(z)"), t("g(a)"));
  check_succeeds(t("g(a)"), t("f(z)"));

  // Same name: the smaller arity comes first.
  check_precedes(t("f"), t("f(a)"));
  check_precedes(t("f(z)"), t("f(a,a)"));
  check_succeeds(t("f(a,a)"), t("f(z)"));
}

BOOST_AUTO_TEST_CASE(test_arguments)
{
  check_equal(t("f(a,b)"), t("f(a,b)"));
  check_equal(t("f(g(a),h(b,c))"), t("f(g(a),h(b,c))"));

  check_precedes(t("f(a,b)"), t("f(a,c)"));
  check_succeeds(t("f(a,c)"), t("f(a,b)"));

  // Lexicographic on the arguments: the first difference decides.
  check_precedes(t("f(a,z)"), t("f(b,a)"));
  check_succeeds(t("f(b,a)"), t("f(a,z)"));
  check_precedes(t("f(g(a),b)"), t("f(g(b),a)"));
  check_succeeds(t("f(g(b),a)"), t("f(g(a),b)"));
}

BOOST_AUTO_TEST_CASE(test_integers)
{
  check_equal(aterm_int(7), aterm_int(7));
  check_precedes(aterm_int(1), aterm_int(2));
  check_succeeds(aterm_int(2), aterm_int(1));

  // Integers precede applications, irrespective of their value.
  check_precedes(aterm_int(1000), t("a"));
  check_succeeds(t("a"), aterm_int(1000));

  check_equal(t("f(1)"), t("f(1)"));
  check_precedes(t("f(1)"), t("f(2)"));
  check_succeeds(t("f(2)"), t("f(1)"));
}

BOOST_AUTO_TEST_CASE(test_lists)
{
  check_equal(t("[]"), t("[]"));
  check_equal(t("[a,b,c]"), t("[a,b,c]"));

  check_precedes(t("[]"), t("[a]"));
  check_succeeds(t("[a]"), t("[]"));

  check_precedes(t("[a,b,c]"), t("[a,b,d]"));
  check_succeeds(t("[a,b,d]"), t("[a,b,c]"));

  // A list precedes its extensions.
  check_precedes(t("[a,b]"), t("[a,b,c]"));
  check_succeeds(t("[a,b,c]"), t("[a,b]"));
}

// The last argument is compared iteratively, so long lists do not exhaust the stack.
BOOST_AUTO_TEST_CASE(test_long_lists)
{
  const std::size_t n = 1000000;
  term_list<aterm_int> l1({aterm_int(1)});
  term_list<aterm_int> l2({aterm_int(2)});
  term_list<aterm_int> l3({aterm_int(1)});
  for (std::size_t i = 0; i < n; ++i)
  {
    l1.push_front(aterm_int(i));
    l2.push_front(aterm_int(i));
    l3.push_front(aterm_int(i));
  }
  check_equal(l1, l3);
  check_precedes(l1, l2);
  check_succeeds(l2, l1);
}

// The order must not depend on the order in which the terms are created, which determines their addresses.
BOOST_AUTO_TEST_CASE(test_independent_of_creation_order)
{
  const std::vector<std::string> names = {"f(e)", "f(d)", "f(c)", "f(b)", "f(a)"};
  std::vector<aterm> terms;
  for (const std::string& s: names)
  {
    terms.push_back(t(s));
  }
  std::sort(terms.begin(), terms.end(), structural_less());

  std::vector<aterm> expected;
  for (auto i = names.rbegin(); i != names.rend(); ++i)
  {
    expected.push_back(t(*i));
  }
  BOOST_CHECK(terms == expected);
}

// Equality coincides with term equality, the order is antisymmetric, and a spot check of transitivity, which
// std::sort and ordered containers rely on.
BOOST_AUTO_TEST_CASE(test_total_order)
{
  const std::vector<aterm> terms = {aterm(), aterm_int(0), aterm_int(3), t("a"), t("b"), t("f(a)"), t("f(b)"),
                                    t("f(a,a)"), t("g(a)"), t("[]"), t("[a]")};
  for (const aterm& x: terms)
  {
    for (const aterm& y: terms)
    {
      BOOST_CHECK_EQUAL(std::is_eq(structural_compare(x, y)), x == y);
      BOOST_CHECK(structural_compare(x, y) == (0 <=> structural_compare(y, x)));
      for (const aterm& z: terms)
      {
        if (structural_less()(x, y) && structural_less()(y, z))
        {
          BOOST_CHECK(structural_less()(x, z));
        }
      }
    }
  }
}
