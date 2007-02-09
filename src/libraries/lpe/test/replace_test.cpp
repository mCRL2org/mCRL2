// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : test/algorithm.cpp
// date          : 19/09/06
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#include <iostream>
#include <iterator>
#include <vector>
#include <boost/test/minimal.hpp>

#include "atermpp/atermpp.h"
#include "atermpp/algorithm.h"
#include "lpe/data_init.h"
#include "lpe/data_functional.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

struct add_zero
{
  aterm_appl operator()(aterm_appl t) const
  {
    if (is_data_variable(t))
    {
      data_variable d(t);
      return data_variable(unquote(d.name()) + "0", d.sort());
    }
    else
      return t;
  }
};

void test_replace()
{
  using namespace lpe::data_init;

  data_variable d("d:D");
  data_variable e("e:D");
  data_variable f("f:D");
  data_variable d0("d0:D");
  data_variable e0("e0:D");
  data_variable f0("f0:D");

  data_expression g = and_(equal_to(d, e), not_equal_to(e, f));

  data_expression h = replace(g, add_zero());
  BOOST_CHECK(h == and_(equal_to(d0, e0), not_equal_to(e0, f0)));

  data_expression h1 = partial_replace(g, add_zero());
  BOOST_CHECK(h == h1);

  data_expression i = replace(g, d, e); 
  BOOST_CHECK(i == and_(equal_to(e, e), not_equal_to(e, f)));
}

int test_main( int, char*[] )
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  test_replace();

  return 0;
}
