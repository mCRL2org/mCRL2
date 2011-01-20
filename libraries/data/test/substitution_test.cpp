// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <functional>
#include <string>
#include <utility>

#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/assignment_list_substitution.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/mutable_substitution_adapter.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/replace.h"

using namespace mcrl2;
using namespace mcrl2::data;

data_expression operator+(data_expression const& l, data_expression const& r) {
  return sort_nat::plus(l, r);
}

data_expression operator*(data_expression const& l, data_expression const& r) {
  return sort_nat::times(l, r);
}

void test_basic()
{
  using namespace mcrl2::data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::data::concepts::MutableSubstitution;

  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< atermpp::map< variable, data_expression > > >));
  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< atermpp::map< variable, variable > > >));

  mutable_map_substitution< atermpp::map< variable, data_expression > > s;

  BOOST_CHECK(static_cast< variable >(s(x)) == x);
  BOOST_CHECK(static_cast< variable >(s(y)) != x);

  function_symbol c("c", sort_nat::nat());

  BOOST_CHECK(c + x == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x), s) == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x * y ), s) == c + x * y);

  s[y] = c;

  BOOST_CHECK(data::replace_free_variables(x, s) == x);
#ifdef MCRL2_NEW_REPLACE_VARIABLES
// in the old version this fails due to the unfortunate interface of replace_free_variables
  BOOST_CHECK(data::replace_free_variables(y, s) == c);
#endif
  BOOST_CHECK(data::replace_free_variables(c + x * y, s) == c + x * c);
  BOOST_CHECK(data::replace_free_variables(lambda(x,x), s) == lambda(x,x));
  BOOST_CHECK(data::replace_free_variables(lambda(x,y), s) == lambda(x,c));

  // Replacing free as well as bound variables
  mutable_map_substitution< atermpp::map< variable, data_expression >, textual_substitution > st;

  st[y] = c;

  // The following should result in assertion failure:
//  BOOST_CHECK(st(lambda(y,y)) != lambda(y,y));
//  BOOST_CHECK(st(application(lambda(y,y),x) + y) != application(lambda(y,y), x) + c);
  st[y] = x;

// 20-01-2010 Removed these tests, since we don't want replace_free_variables to touch variables of lambda expressions (Wieger).
//  BOOST_CHECK(data::replace_free_variables(lambda(y,y), st) == lambda(x,x));
//  BOOST_CHECK(data::replace_free_variables(lambda(y,y)(x) + y, st) == lambda(x,x)(x) + x);

  // Replacing free variables only
  mutable_map_substitution< atermpp::map< variable, data_expression >, structural_substitution > sb;

  sb[y] = c;

  BOOST_CHECK(data::replace_free_variables(lambda(y,y), sb) == lambda(y,y));
  BOOST_CHECK(data::replace_free_variables(lambda(y,y)(x) + y, sb) == lambda(y,y)(x) + c);
  core::garbage_collect();
}

struct my_assignment_list_substitution: public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  assignment_list assignments;

  my_assignment_list_substitution(assignment_list assignments_)
    : assignments(assignments_)
  {}
  
  data_expression operator()(const variable& v) const
  {
    for (assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i->rhs();
      }
    }
    return v;
  }
};

void test_my_assignment_list_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));

  my_assignment_list_substitution f(l);

  BOOST_CHECK(f(x) == y);
  BOOST_CHECK(f(y) == y);
  BOOST_CHECK(f(z) == z);
  BOOST_CHECK(f(u) == z);

  assignment yz(y,z);
  l = make_list(xy, uz, yz);
  my_assignment_list_substitution g(l);

  BOOST_CHECK(g(x) == y); // Assignments are not simultaneous, hence we expect y
  BOOST_CHECK(g(y) == z);
  BOOST_CHECK(g(z) == z);
  BOOST_CHECK(g(u) == z);
  core::garbage_collect();
}

void test_my_list_substitution()
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
  assignment_list r = make_list(assignment(x, y1));

  BOOST_CHECK(replace_variables(x,  my_assignment_list_substitution(r)) == y1);
  BOOST_CHECK(replace_variables(y,  my_assignment_list_substitution(r)) == y);
  BOOST_CHECK(replace_variables(z,  my_assignment_list_substitution(r)) == z);
  BOOST_CHECK(replace_variables(u,  my_assignment_list_substitution(r)) == u);
std::cerr << replace_variables(xy,  my_assignment_list_substitution(r)) << std::endl;
  BOOST_CHECK(replace_variables(xy, my_assignment_list_substitution(r)) == assignment(y1,y));
  BOOST_CHECK(replace_variables(uz, my_assignment_list_substitution(r)) == uz);
std::cerr << replace_variables(l,   my_assignment_list_substitution(r)) << std::endl;
  BOOST_CHECK(replace_variables(l,  my_assignment_list_substitution(r)) == assignment_list(make_list(assignment(y1,y), uz)));
  core::garbage_collect();
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
  core::garbage_collect();
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
  assignment_list r = make_list(assignment(x, y1));

  BOOST_CHECK(replace_variables(x, assignment_list_substitution(r)) == y1);
  BOOST_CHECK(replace_variables(y, assignment_list_substitution(r)) == y);
  BOOST_CHECK(replace_variables(z, assignment_list_substitution(r)) == z);
  BOOST_CHECK(replace_variables(u, assignment_list_substitution(r)) == u);
std::cerr << replace_variables(xy, assignment_list_substitution(r)) << std::endl;
  BOOST_CHECK(replace_variables(xy, assignment_list_substitution(r)) == assignment(y1,y));
  BOOST_CHECK(replace_variables(uz, assignment_list_substitution(r)) == uz);

std::cerr << replace_variables(l, assignment_list_substitution(r)) << std::endl;
  BOOST_CHECK(replace_variables(l, assignment_list_substitution(r)) == assignment_list(make_list(assignment(y1,y), uz)));
  core::garbage_collect();
}

void test_mutable_substitution_adapter()
{
  mutable_map_substitution< > f;
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  f[x] = y;

  mutable_substitution_adapter<mutable_map_substitution< > > g(f);
  BOOST_CHECK(g(x) == y);

  assignment a(y, z);
  mutable_substitution_adapter<assignment> h(a);
#ifdef MCRL2_NEW_REPLACE_VARIABLES
// the old replace interface does not work here...
  BOOST_CHECK(replace_free_variables(x, h) == x);
  BOOST_CHECK(replace_free_variables(y, h) == z);
  h[x] = y;
  BOOST_CHECK(replace_free_variables(x, h) == y);
  h[y] = y;
  BOOST_CHECK(replace_free_variables(y, h) == z);
  h[z] = x;
  BOOST_CHECK(replace_free_variables(y, h) == x);
#endif
  core::garbage_collect();
}

void test_mutable_substitution()
{
  using namespace mcrl2::data::detail;

  mutable_map_substitution< atermpp::map< variable, data_expression_with_variables > > sigma;
  variable v("v", sort_nat::nat());
  data_expression e = v;

  data_expression_with_variables e1;
  e1 = e;

  sigma[v] = e;

  // Compile test
  mutable_map_substitution< atermpp::map< variable, variable > > sigmaprime;

  sigma[v] = v;
  core::garbage_collect();
}

void test_map_substitution_adapter()
{
  atermpp::map< variable, variable > mapping;
  variable v("v", sort_nat::nat());
  variable vv("vv", sort_nat::nat());

  // Compile test
  map_substitution< atermpp::map< variable, variable > const& > sigma(mapping);

  mapping[v] = vv;

  BOOST_CHECK(replace_free_variables(v, sigma) == vv);
  core::garbage_collect();
}

int test_main(int a, char**aa)
{
  MCRL2_ATERMPP_INIT(a, aa);

  test_my_assignment_list_substitution();
  test_my_list_substitution();

  test_basic();
  test_assignment_list_substitution();
  test_list_substitution();
  test_mutable_substitution_adapter();
  test_mutable_substitution();

  return EXIT_SUCCESS;
}
