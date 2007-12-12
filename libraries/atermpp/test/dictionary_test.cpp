// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file table_test.cpp
/// \brief Add your file description here.

// very simple test for atermpp::table

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "atermpp/atermpp.h"
#include "atermpp/dictionary.h"

using namespace std;
using namespace atermpp;

void test_dictionary()
{
  aterm a = make_term("a");
  aterm b = make_term("b");
  aterm c = make_term("c");
  
  dictionary dict;
  dict.put(a, b);
  BOOST_CHECK(dict.get(a) == b); 
  dict.put(b, b);
  BOOST_CHECK(dict.get(a) == b); 
  BOOST_CHECK(dict.get(b) == b); 
  dict.put(a, c);
  BOOST_CHECK(dict.get(a) == c); 
  dict.remove(a);
  BOOST_CHECK(dict.get(a) == aterm()); 
}

int test_main( int, char*[] )
{
  ATERM_LIBRARY_INIT()
  test_dictionary();
  return 0;
}
