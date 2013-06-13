// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file down_cast_test.cpp
/// \brief Test for down casting

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/function_symbol.h"

#include "mcrl2/core/down_cast.h"

using namespace std;

class t1
{
protected:
  int m;

public:
  t1(const int x)
    : m(x)
  {}
  bool operator ==(const t1& other)
  {
    return m == other.m;
  }
};

class t2 : public t1
{
public:
  t2(const t1& x)
    : t1(x)
  {}
};

class t3: public t1
{
public:
  explicit t3(const t1& x)
    : t1(x)
  {}
};

// atermpp::aterm cannot be downcasted to u.
class u: public atermpp::aterm
{
protected:
  int m;

public:
  u(const atermpp::aterm& x, const int y)
    : atermpp::aterm(x),
      m(y)
  {}
};

inline
void f(const t3& t)
{
}

BOOST_AUTO_TEST_CASE(aterm_down_cast)
{
  atermpp::function_symbol f("f",2);
  atermpp::function_symbol x("x",0);
  atermpp::function_symbol y("y",0);
  atermpp::aterm_appl fxy(f,atermpp::aterm_appl(x),atermpp::aterm_appl(y));
  atermpp::aterm fxy_term(fxy);

  atermpp::aterm_appl test = mcrl2::core::down_cast<atermpp::aterm_appl>(fxy_term);
  BOOST_CHECK(fxy_term == test);
  BOOST_CHECK(fxy == test);
  BOOST_CHECK(fxy == fxy_term);

  u test2(fxy_term, 3);
}

// Uses the second definition of down_cast,
// and will not be evaluated using the third definition of down_cast
BOOST_AUTO_TEST_CASE(implicit_down_cast)
{
  t1 x(1);
  t2 test = mcrl2::core::down_cast<t2>(x);
  BOOST_CHECK(x == static_cast<t1>(test));
}

// Uses the third definition of down_cast,
// and will not be evaluated using the second definition of down_cast
BOOST_AUTO_TEST_CASE(explicit_down_cast)
{
  t1 x(1);
  t3 test = mcrl2::core::down_cast<t3>(x);
  BOOST_CHECK(x == static_cast<t1>(test));
}

BOOST_AUTO_TEST_CASE(no_down_cast_needed)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  t3 x32(x2);
  t3 x33(x31);
}

BOOST_AUTO_TEST_CASE(function_calls)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  const t1& y(t3);
  // Now how do I call f(y)?
  // f(mcrl2::core::down_cast<t3>(y)); This does not work.
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
