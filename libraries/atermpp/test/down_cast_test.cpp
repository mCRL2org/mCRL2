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

#include "mcrl2/atermpp/aterm_list.h"

class t1 : public atermpp::aterm
{
public:
  t1(const int ) 
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
void f(const t3& )
{
}

BOOST_AUTO_TEST_CASE(aterm_down_cast)
{
  atermpp::function_symbol fs("f",2);
  atermpp::function_symbol x("x",0);
  atermpp::function_symbol y("y",0);
  atermpp::aterm fxy(fs,atermpp::aterm(x),atermpp::aterm(y));
  atermpp::aterm fxy_term(fxy);

  const atermpp::aterm& test = fxy_term;  // No down_cast required or allowed. Test could be removed or adapted.
  BOOST_CHECK(fxy_term == test);
  BOOST_CHECK(fxy == test);
  BOOST_CHECK(fxy == fxy_term);

  const atermpp::aterm& t1(fxy);
  [[maybe_unused]]
  const atermpp::aterm& t2 = t1; // See comment above. Test is now useless.
  f(atermpp::down_cast<t3>(t1));
}

BOOST_AUTO_TEST_CASE(no_down_cast_needed)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  t3 x32(x2);
  [[maybe_unused]]
  const t3& x33(x31);
}

BOOST_AUTO_TEST_CASE(function_calls)
{
  t1 x1(1);
  t2 x2(x1);
  t3 x31(x1);
  const t1& y(t3);
}

