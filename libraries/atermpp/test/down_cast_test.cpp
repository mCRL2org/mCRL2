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
#include "mcrl2/atermpp/aterm_list.h"

using namespace std;

class t1 : public atermpp::aterm
{
public:
  t1(const int x) {}
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

inline
void f(const t3& t)
{
}

BOOST_AUTO_TEST_CASE(aterm_down_cast)
{
  atermpp::function_symbol fs("f",2);
  atermpp::function_symbol x("x",0);
  atermpp::function_symbol y("y",0);
  atermpp::aterm_appl fxy(fs,atermpp::aterm_appl(x),atermpp::aterm_appl(y));
  atermpp::aterm fxy_term(fxy);

  const atermpp::aterm_appl& test = atermpp::down_cast<atermpp::aterm_appl>(fxy_term);
  BOOST_CHECK(fxy_term == test);
  BOOST_CHECK(fxy == test);
  BOOST_CHECK(fxy == fxy_term);

  const atermpp::aterm& t1(fxy);
  const atermpp::aterm_appl& t2 = atermpp::down_cast<atermpp::aterm_appl>(t1);
  f(atermpp::down_cast<t3>(t1));
}

BOOST_AUTO_TEST_CASE(aterm_container_cast)
{
  atermpp::term_list<t1> s1;
  atermpp::term_list<t2> s2;
  s2 = atermpp::container_cast< atermpp::term_list<t2> >(s1);
  s1 = atermpp::container_cast< atermpp::term_list<t1> >(s2);
}

BOOST_AUTO_TEST_CASE(no_down_cast_needed)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  t3 x32(x2);
  const t3& x33(x31);
}

BOOST_AUTO_TEST_CASE(function_calls)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  const t1& y(t3);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
