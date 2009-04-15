// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <utility>

#include <boost/test/minimal.hpp>

#include "mcrl2/new_data/lambda.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/mutable_substitution_adapter.h"
#include "mcrl2/new_data/detail/concepts.h"
#include "mcrl2/new_data/detail/data_expression_with_variables.h"
#include "mcrl2/new_data/standard_utility.h"

using namespace mcrl2;
using namespace mcrl2::new_data;

data_expression operator+(data_expression const& l, data_expression const& r) {
  return sort_nat::plus(l, r);
}

data_expression operator*(data_expression const& l, data_expression const& r) {
  return sort_nat::times(l, r);
}

void test1()
{
  using namespace mcrl2::new_data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::new_data::concepts::MutableSubstitution;

  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_substitution< variable, data_expression > >));
  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_substitution< variable, variable > >));

  mutable_substitution< variable, data_expression > s;

  BOOST_CHECK(static_cast< variable >(s(x)) == x);
  BOOST_CHECK(static_cast< variable >(s(y)) != x);

  function_symbol c("c", sort_nat::nat());

  BOOST_CHECK(c + x == c + x);
  BOOST_CHECK(s(data_expression(c + x)) == c + x);
  BOOST_CHECK(s(data_expression(c + x * y )) == c + x * y);

  s[y] = c;

  BOOST_CHECK(static_cast< data_expression >(s(x)) == x);
  BOOST_CHECK(static_cast< data_expression >(s(y)) == c);
  BOOST_CHECK(s(y) == c);
  BOOST_CHECK(s(c + x * y) == c + x * c);
  BOOST_CHECK(s(lambda(x,x)) == lambda(x,x));
  BOOST_CHECK(s(lambda(x,y)) == lambda(x,c));

  // non-capture avoiding substitution
  BOOST_CHECK(s(lambda(y,y)) != lambda(y,y));
  BOOST_CHECK(s(application(lambda(y,y),x) + y) != application(lambda(y,y), x) + c);
}

void test_mutable_substitution_adapter()
{
  mutable_substitution<variable, data_expression> f;
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  f[x] = y;
  
  mutable_substitution_adapter<mutable_substitution<variable, data_expression> > g(f);
  BOOST_CHECK(g(x) == y);

  assignment a(y, z);
  mutable_substitution_adapter<assignment> h(a);
  BOOST_CHECK(h(x) == x);
  BOOST_CHECK(h(y) == z);
  h[x] = y;
  BOOST_CHECK(h(x) == y);
  h[y] = y;
  BOOST_CHECK(h(y) == z);
  h[z] = x;
  BOOST_CHECK(h(y) == x);
}

void test_mutable_substitution()
{
  using namespace mcrl2::new_data::detail;

  mutable_substitution<variable, data_expression_with_variables> sigma;
  variable v("v", sort_nat::nat());
  data_expression e = v;

  data_expression_with_variables e1;
  e1 = e;

  sigma[v] = e;
}

int test_main(int a, char**aa)
{
  MCRL2_ATERMPP_INIT(a, aa);

  test1();
  test_mutable_substitution_adapter();
  test_mutable_substitution();

  return EXIT_SUCCESS;
}
