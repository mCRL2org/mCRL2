// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io_text.h"
#include "mcrl2/atermpp/aterm_list.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace atermpp;

/// \brief Reads all terms from text in the textual aterm format.
static std::vector<aterm> read_terms(const std::string& text)
{
  std::stringstream stream(text);
  text_aterm_istream input(stream);

  std::vector<aterm> result;
  aterm term;
  input >> term;
  while (term.defined())
  {
    result.push_back(term);
    term = aterm();
    input >> term;
  }
  return result;
}

/// \brief Reads the single term in text, which must be an integer, and returns its value.
static std::size_t read_integer(const std::string& text)
{
  const std::vector<aterm> terms = read_terms(text);
  BOOST_REQUIRE_EQUAL(terms.size(), 1u);
  BOOST_REQUIRE(terms[0].type_is_int());
  return down_cast<aterm_int>(terms[0]).value();
}

// The tests up to integer_round_trip_test cover the common case of integers that fit in 32 bits, in terms that are
// separated by brackets, which is read the same way on every platform.

BOOST_AUTO_TEST_CASE(single_integer_test)
{
  BOOST_CHECK_EQUAL(read_integer("0"), 0u);
  BOOST_CHECK_EQUAL(read_integer("7"), 7u);
  BOOST_CHECK_EQUAL(read_integer("42\n"), 42u);
  BOOST_CHECK_EQUAL(read_integer("  1234567\n"), 1234567u);
  BOOST_CHECK_EQUAL(read_integer("2147483647\n"), static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()));
}

BOOST_AUTO_TEST_CASE(integers_in_terms_test)
{
  const aterm_int one(1);
  const aterm_int two(2);
  const aterm_int three(3);

  const std::vector<aterm> terms = read_terms("f(1,2)\n[1,2,3]\n[ 1 , 2 ]\nf(1, [2, 3], g(3))\n[]\n");
  BOOST_REQUIRE_EQUAL(terms.size(), 5u);
  BOOST_CHECK_EQUAL(terms[0], aterm(function_symbol("f", 2), one, two));
  BOOST_CHECK_EQUAL(terms[1], aterm_list({one, two, three}));
  BOOST_CHECK_EQUAL(terms[2], aterm_list({one, two}));
  BOOST_CHECK_EQUAL(terms[3],
    aterm(function_symbol("f", 3), one, aterm_list({two, three}), aterm(function_symbol("g", 1), three)));
  BOOST_CHECK_EQUAL(terms[4], aterm_list());
}

BOOST_AUTO_TEST_CASE(small_term_round_trip_test)
{
  const aterm a(function_symbol("a", 0));
  const std::vector<aterm> originals = {
    aterm(function_symbol("f", 2), aterm_int(0), aterm_int(100)),
    aterm_list({aterm_int(5), a, aterm_list({aterm_int(65535)})}),
    aterm(function_symbol("g", 1), aterm_int(static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))),
  };

  std::stringstream stream;
  {
    text_aterm_ostream output(stream, true);
    for (const aterm& term : originals)
    {
      output << term;
    }
  }

  const std::vector<aterm> terms = read_terms(stream.str());
  BOOST_REQUIRE_EQUAL(terms.size(), originals.size());
  for (std::size_t i = 0; i < originals.size(); ++i)
  {
    BOOST_CHECK_EQUAL(terms[i], originals[i]);
  }
}

// Integers in the text format must be read as std::size_t on every platform. On Windows a long has only 32 bits.
BOOST_AUTO_TEST_CASE(integer_round_trip_test)
{
  const std::vector<std::size_t> values = {
    0,
    1,
    static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()),
    static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()) + 1,
    static_cast<std::size_t>(1) << 32,
    static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()),
    static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()) + 1,
    std::numeric_limits<std::size_t>::max(),
  };

  std::stringstream stream;
  {
    text_aterm_ostream output(stream, true);
    for (const std::size_t value : values)
    {
      output << aterm_int(value);
    }
  }

  const std::vector<aterm> terms = read_terms(stream.str());
  BOOST_REQUIRE_EQUAL(terms.size(), values.size());
  for (std::size_t i = 0; i < values.size(); ++i)
  {
    BOOST_REQUIRE(terms[i].type_is_int());
    BOOST_CHECK_EQUAL(down_cast<aterm_int>(terms[i]).value(), values[i]);
  }
}

BOOST_AUTO_TEST_CASE(large_integers_as_arguments_test)
{
  const std::vector<aterm> terms = read_terms("f(4294967296,18446744073709551615)\n[9223372036854775808]\n");
  BOOST_REQUIRE_EQUAL(terms.size(), 2u);

  const aterm f(function_symbol("f", 2),
    aterm_int(static_cast<std::size_t>(1) << 32),
    aterm_int(std::numeric_limits<std::size_t>::max()));
  BOOST_CHECK_EQUAL(terms[0], f);

  const aterm_list list({aterm_int(static_cast<std::size_t>(1) << 63)});
  BOOST_CHECK_EQUAL(terms[1], list);
}

// The number must be read completely, also when it has more digits than any std::size_t needs.
BOOST_AUTO_TEST_CASE(leading_zeros_test)
{
  const std::string zeros(40, '0');
  BOOST_CHECK_EQUAL(read_integer(zeros + "5\n"), 5u);
  BOOST_CHECK_EQUAL(read_integer(zeros + "\n"), 0u);
  BOOST_CHECK_EQUAL(read_integer(zeros + "18446744073709551615\n"), std::numeric_limits<std::size_t>::max());
}

BOOST_AUTO_TEST_CASE(integer_out_of_range_test)
{
  // The largest std::size_t plus one.
  BOOST_CHECK_THROW(read_terms("18446744073709551616\n"), std::runtime_error);
  // A number with more digits than fit in 32 characters, at the top level and as an argument.
  const std::string many_nines(41, '9');
  BOOST_CHECK_THROW(read_terms(many_nines + "\n"), std::runtime_error);
  BOOST_CHECK_THROW(read_terms("f(" + many_nines + ")\n"), std::runtime_error);
}

// Integer terms are unsigned, so a negative number cannot be read.
BOOST_AUTO_TEST_CASE(negative_integer_test)
{
  BOOST_CHECK_THROW(read_terms("-1\n"), std::runtime_error);
  BOOST_CHECK_THROW(read_terms("f(-1)\n"), std::runtime_error);
}

// Integer terms on consecutive lines are separate terms, not the digits of a single number.
BOOST_AUTO_TEST_CASE(consecutive_integers_test)
{
  const std::vector<aterm> terms = read_terms("0\n42\nf(1,[2,3])\n");
  BOOST_REQUIRE_EQUAL(terms.size(), 3u);
  BOOST_CHECK_EQUAL(terms[0], aterm_int(0));
  BOOST_CHECK_EQUAL(terms[1], aterm_int(42));
  const aterm f(function_symbol("f", 2), aterm_int(1), aterm_list({aterm_int(2), aterm_int(3)}));
  BOOST_CHECK_EQUAL(terms[2], f);
}
