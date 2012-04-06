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

#include "mcrl2/aterm/aterm2.h"

using namespace std;
using namespace aterm;

void test_plain_aterm_construction()
{
  const char *s0="f0";
  const AFun f0=ATmakeAFun(s0,0,true);
  BOOST_CHECK(ATgetArity(f0)==0);
  BOOST_CHECK(!strcmp(ATgetName(f0),s0));
  BOOST_CHECK(ATisQuoted(f0));

  const ATermAppl a0=ATmakeAppl0(f0);
  BOOST_CHECK(ATgetAFun(a0)==f0);

  const char *s1="f1";
  const AFun f1=ATmakeAFun(s1,1,false);
  BOOST_CHECK(ATgetArity(f1)==1);
  BOOST_CHECK(!strcmp(ATgetName(f1),s1));
  BOOST_CHECK(!ATisQuoted(f1));

  const ATermAppl a1=ATmakeAppl1(f1,a0);
  BOOST_CHECK(ATgetAFun(a1)==f1);
  BOOST_CHECK(ATgetArgument(a1,0)==a0);

  const char *s2="f0"; // Intentionally reuse string "f0".
  const AFun f2=ATmakeAFun(s2,2,true);
  BOOST_CHECK(ATgetArity(f2)==2);
  BOOST_CHECK(!strcmp(ATgetName(f2),s0));
  BOOST_CHECK(!strcmp(ATgetName(f2),s2));
  BOOST_CHECK(ATisQuoted(f2));

  const ATermAppl a2=ATmakeAppl2(f2,a0,a1);
  BOOST_CHECK(ATgetAFun(a2)==f2);
  BOOST_CHECK(ATgetArgument(a2,0)==a0);
  BOOST_CHECK(ATgetArgument(a2,1)==a1);

}

int test_main(int argc, char* argv[])
{
  ATerm bottom_of_stack;
  ATinit(&bottom_of_stack);

  test_plain_aterm_construction(); // Run test twice. In the first run the elements are not constructed.
  test_plain_aterm_construction(); // In the second, they already exist.

  return 0;
}
