// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_list_test.cpp
/// \brief Add your file description here.

#include <sstream>
#include <algorithm>
#include <boost/test/minimal.hpp>

#include "mcrl2/aterm/aterm.h"

using namespace std;
using namespace aterm_deprecated;

void test_plain_aterm_construction()
{
  const char *s0="f0";
  const AFun f0(s0,0);
  BOOST_CHECK(f0.arity()==0);
  BOOST_CHECK(!strcmp(ATgetName(f0),s0));

  const ATermAppl a0=ATmakeAppl0(f0);
  BOOST_CHECK(a0.function()==f0.number());

  const char *s1="f1";
  const AFun f1(s1,1);
  BOOST_CHECK(f1.arity()==1);
  BOOST_CHECK(!strcmp(ATgetName(f1),s1));

  const ATermAppl a1=ATmakeAppl1(f1,a0);
  BOOST_CHECK(a1.function()==f1.number());
  BOOST_CHECK(ATgetArgument(a1,0)==a0);

  const char *s2="f0"; // Intentionally reuse string "f0".
  const AFun f2(s2,2);
  BOOST_CHECK(f2.arity()==2);
  BOOST_CHECK(!strcmp(ATgetName(f2),s0));
  BOOST_CHECK(!strcmp(ATgetName(f2),s2));

  const ATermAppl a2=ATmakeAppl2(f2,a0,a1);
  BOOST_CHECK(a2.function()==f2.number());
  BOOST_CHECK(ATgetArgument(a2,0)==a0);
  BOOST_CHECK(ATgetArgument(a2,1)==a1);

}

int test_main(int argc, char* argv[])
{
  // ATinit();

  test_plain_aterm_construction(); // Run test twice. In the first run the elements are not constructed.
  test_plain_aterm_construction(); // In the second, they already exist.

  return 0;
}
