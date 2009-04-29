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

#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/assignment_list_substitution.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/mutable_substitution_adapter.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/map_substitution_adapter.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;

data_expression operator+(data_expression const& l, data_expression const& r) {
  return sort_nat::plus(l, r);
}

data_expression operator*(data_expression const& l, data_expression const& r) {
  return sort_nat::times(l, r);
}

void test1()
{
  using namespace mcrl2::data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::data::concepts::MutableSubstitution;

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

void test_assignment_list_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));

  assignment_list_substitution f(l);

  BOOST_CHECK(f(x) == y);
  BOOST_CHECK(f(y) == y);
  BOOST_CHECK(f(z) == z);
  BOOST_CHECK(f(u) == z);

  assignment yz(y,z);
  l = make_list(xy, uz, yz);
  assignment_list_substitution g(l);

  BOOST_CHECK(g(x) == y); // Assignments are not simultaneous, hence we expect y
  BOOST_CHECK(g(y) == z);
  BOOST_CHECK(g(z) == z);
  BOOST_CHECK(g(u) == z);
}

void test_list_substitution()
{
  using namespace atermpp;

  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  variable y1("y1", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));

  BOOST_CHECK(substitute(make_list_substitution(x,y1), x) == y1);
  BOOST_CHECK(substitute(make_list_substitution(x,y1), y) == y);
  BOOST_CHECK(substitute(make_list_substitution(x,y1), z) == z);
  BOOST_CHECK(substitute(make_list_substitution(x,y1), u) == u);

  BOOST_CHECK(substitute(make_list_substitution(x,y1), xy) == assignment(y1,y));
  BOOST_CHECK(substitute(make_list_substitution(x,y1), uz) == uz);

  BOOST_CHECK(substitute(make_list_substitution(x,y1), l) == assignment_list(make_list(assignment(y1,y), uz)));
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
  using namespace mcrl2::data::detail;

  mutable_substitution<variable, data_expression_with_variables> sigma;
  variable v("v", sort_nat::nat());
  data_expression e = v;

  data_expression_with_variables e1;
  e1 = e;

  sigma[v] = e;

  // Compile test
  mutable_substitution<variable, variable> sigmaprime;

  sigma[v] = v;
}

void test_map_substitution_adapter()
{
  atermpp::map< variable, variable > mapping;
  variable v("v", sort_nat::nat());
  variable vv("vv", sort_nat::nat());

  // Compile test
  map_substitution_adapter< atermpp::map< variable, variable > > sigma(mapping);

  mapping[v] = vv;

  BOOST_CHECK(sigma(v) == vv);
}

int test_main(int a, char**aa)
{
  MCRL2_ATERMPP_INIT(a, aa);

  test1();
  core::garbage_collect();

  test_assignment_list_substitution();
  core::garbage_collect();

  test_list_substitution();
  core::garbage_collect();

  test_mutable_substitution_adapter();
  core::garbage_collect();

  test_mutable_substitution();
  core::garbage_collect();

  return EXIT_SUCCESS;
}
