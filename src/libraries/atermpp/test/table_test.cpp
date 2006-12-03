// very simple test for atermpp::table

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "atermpp/atermpp.h"
#include "atermpp/table.h"

using namespace std;
using namespace atermpp;

void test_table()
{
  table t(100, 75);
  t.put(make_term("a"), make_term("f(a)"));
  BOOST_CHECK(t.table_keys().size() == 1); 
  aterm a = t.get(make_term("a"));
  BOOST_CHECK(a = make_term("f(a)"));
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  test_table();
  return 0;
}
