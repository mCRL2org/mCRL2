// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file down_cast_test.cpp
/// \brief Test for down casting

#define BOOST_TEST_MODULE down_cast_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/unused.h"
#include "mcrl2/atermpp/aterm_list.h"

class t1 : public atermpp::aterm
{
public:
  t1(const int x) 
  {}

  explicit t1(const aterm& x) 
   : atermpp::aterm(x)
  {}
};

class t2 : public t1
{
public:
  t2(const t1& x)
    : t1(x)
  {}

  explicit t2(const aterm& x)
   : t1(x)
  {}
};

class t3: public t1
{
public:
  explicit t3(const t1& x)
    : t1(x)
  {}

  explicit t3(const aterm& x)
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
  mcrl2::utilities::mcrl2_unused(t2);
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
  mcrl2::utilities::mcrl2_unused(x33);
}

BOOST_AUTO_TEST_CASE(function_calls)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  const t1& y(t3);
}

