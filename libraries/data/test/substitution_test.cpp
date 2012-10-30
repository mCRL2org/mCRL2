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

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/concepts.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/replace.h"

using namespace mcrl2;
using namespace mcrl2::data;

inline
data_expression operator+(data_expression const& l, data_expression const& r)
{
  return sort_nat::plus(l, r);
}

inline
data_expression operator*(data_expression const& l, data_expression const& r)
{
  return sort_nat::times(l, r);
}

void test_basic()
{
  using namespace mcrl2::data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::data::concepts::MutableSubstitution;

  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< std::map< variable, data_expression > > >));
  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< std::map< variable, variable > > >));

  mutable_map_substitution< std::map< variable, data_expression > > s;

  BOOST_CHECK(static_cast< variable >(s(x)) == x);
  BOOST_CHECK(static_cast< variable >(s(y)) != x);

  data::function_symbol c("c", sort_nat::nat());

  BOOST_CHECK(c + x == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x), s) == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x * y), s) == c + x * y);

  s[y] = c;

  BOOST_CHECK(data::replace_free_variables(x, s) == x);
#ifdef MCRL2_NEW_REPLACE_VARIABLES
// in the old version this fails due to the unfortunate interface of replace_free_variables
  BOOST_CHECK(data::replace_free_variables(y, s) == c);
#endif
  BOOST_CHECK(data::replace_free_variables(c + x * y, s) == c + x * c);
  BOOST_CHECK(data::replace_free_variables(lambda(x,x), s) == lambda(x,x));
  BOOST_CHECK(data::replace_free_variables(lambda(x,y), s) == lambda(x,c));

  // Replacing free variables only
  mutable_map_substitution< std::map< variable, data_expression > > sb;

  sb[y] = c;

  BOOST_CHECK(data::replace_free_variables(lambda(y,y), sb) == lambda(y,y));
  BOOST_CHECK(data::replace_free_variables(lambda(y,y)(x) + y, sb) == lambda(y,y)(x) + c);
}

void test_indexed_substitution()
{
  using namespace mcrl2::data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::data::concepts::MutableSubstitution;

  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_indexed_substitution< variable, std::vector< data_expression > > >));
  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_indexed_substitution< variable, std::vector< variable > > >));

  mutable_indexed_substitution< variable, std::vector< data_expression > > s;

  BOOST_CHECK(static_cast< variable >(s(x)) == x);
  BOOST_CHECK(static_cast< variable >(s(y)) != x);

  data::function_symbol c("c", sort_nat::nat());

  BOOST_CHECK(c + x == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x), s) == c + x);
  BOOST_CHECK(data::replace_free_variables(data_expression(c + x * y), s) == c + x * y);

  s[y] = c;

#ifdef MCRL2_NEW_REPLACE_VARIABLES
// in the old version this fails due to the unfortunate interface of replace_free_variables
  BOOST_CHECK(data::replace_free_variables(x, s) == x);
  BOOST_CHECK(data::replace_free_variables(y, s) == c);
#endif
std::cerr << "TEST " << x << "  " << y << " OUD \n";
std::cerr << "RES " << c + x * c << "\n";
std::cerr << "RES " << data::replace_free_variables(c + x * y, s) << "\n";
  BOOST_CHECK(data::replace_free_variables(c + x * y, s) == c + x * c);
  BOOST_CHECK(data::replace_free_variables(lambda(x,x), s) == lambda(x,x));
  BOOST_CHECK(data::replace_free_variables(lambda(x,y), s) == lambda(x,c));

  // Replacing free variables only
  mutable_indexed_substitution< variable, std::vector< data_expression > > sb;

  sb[y] = c;

  BOOST_CHECK(data::replace_free_variables(lambda(y,y), sb) == lambda(y,y));
  BOOST_CHECK(data::replace_free_variables(lambda(y,y)(x) + y, sb) == lambda(y,y)(x) + c);
}

struct my_assignment_sequence_substitution: public std::unary_function<variable, data_expression>
{
  typedef variable variable_type;
  typedef data_expression expression_type;

  assignment_list assignments;

  my_assignment_sequence_substitution(assignment_list assignments_)
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

void test_my_assignment_sequence_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));

  my_assignment_sequence_substitution f(l);

  BOOST_CHECK(f(x) == y);
  BOOST_CHECK(f(y) == y);
  BOOST_CHECK(f(z) == z);
  BOOST_CHECK(f(u) == z);

  assignment yz(y,z);
  l = make_list(xy, uz, yz);
  my_assignment_sequence_substitution g(l);

  BOOST_CHECK(g(x) == y); // Assignments are not simultaneous, hence we expect y
  BOOST_CHECK(g(y) == z);
  BOOST_CHECK(g(z) == z);
  BOOST_CHECK(g(u) == z);
}

void test_my_list_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  variable y1("y1", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));
  assignment_list r = make_list(assignment(x, y1));

  BOOST_CHECK(replace_variables(x,  my_assignment_sequence_substitution(r)) == y1);
  BOOST_CHECK(replace_variables(y,  my_assignment_sequence_substitution(r)) == y);
  BOOST_CHECK(replace_variables(z,  my_assignment_sequence_substitution(r)) == z);
  BOOST_CHECK(replace_variables(u,  my_assignment_sequence_substitution(r)) == u);
  std::cerr << replace_variables(xy,  my_assignment_sequence_substitution(r)) << std::endl;
//  BOOST_CHECK(replace_variables(xy, my_assignment_sequence_substitution(r)) == assignment(y1,y));
  BOOST_CHECK(replace_variables(uz, my_assignment_sequence_substitution(r)) == uz);
  std::cerr << replace_variables(l,   my_assignment_sequence_substitution(r)) << std::endl;
//  BOOST_CHECK(replace_variables(l,  my_assignment_sequence_substitution(r)) == assignment_list(make_list(assignment(y1,y), uz)));
}

void test_assignment_sequence_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));

  assignment_sequence_substitution f(l);

  BOOST_CHECK(f(x) == y);
  BOOST_CHECK(f(y) == y);
  BOOST_CHECK(f(z) == z);
  BOOST_CHECK(f(u) == z);

  assignment yz(y,z);
  l = make_list(xy, uz, yz);
  assignment_sequence_substitution g(l);

  BOOST_CHECK(g(x) == y); // Assignments are not simultaneous, hence we expect y
  BOOST_CHECK(g(y) == z);
  BOOST_CHECK(g(z) == z);
  BOOST_CHECK(g(u) == z);
}

void test_list_substitution()
{
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  variable u("u", sort_nat::nat());

  variable y1("y1", sort_nat::nat());

  assignment xy(x,y);
  assignment uz(u,z);
  assignment_list l(make_list(xy, uz));
  assignment_list r = make_list(assignment(x, y1));

  BOOST_CHECK(replace_variables(x, assignment_sequence_substitution(r)) == y1);
  BOOST_CHECK(replace_variables(y, assignment_sequence_substitution(r)) == y);
  BOOST_CHECK(replace_variables(z, assignment_sequence_substitution(r)) == z);
  BOOST_CHECK(replace_variables(u, assignment_sequence_substitution(r)) == u);
  std::cerr << replace_variables(xy, assignment_sequence_substitution(r)) << std::endl;
//  BOOST_CHECK(replace_variables(xy, assignment_sequence_substitution(r)) == assignment(y1,y));
  BOOST_CHECK(replace_variables(uz, assignment_sequence_substitution(r)) == uz);

  std::cerr << replace_variables(l, assignment_sequence_substitution(r)) << std::endl;
//  BOOST_CHECK(replace_variables(l, assignment_sequence_substitution(r)) == assignment_list(make_list(assignment(y1,y), uz)));
}

void test_mutable_substitution_composer()
{
  mutable_map_substitution< > f;
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  f[x] = y;

  mutable_substitution_composer<mutable_map_substitution< > > g(f);
  BOOST_CHECK(g(x) == y);

  assignment a(y, z);
  mutable_substitution_composer<assignment> h(a);
#ifdef MCRL2_DECLTYPE
  // This will not work until decltype can be used in mCRL2 to determine the appropriate return type.
  BOOST_CHECK(replace_free_variables(x, h) == x);
  BOOST_CHECK(replace_free_variables(y, h) == z);
  h[x] = y;
  BOOST_CHECK(replace_free_variables(x, h) == y);
  h[y] = y;
  BOOST_CHECK(replace_free_variables(y, h) == z);
  h[z] = x;
  BOOST_CHECK(replace_free_variables(y, h) == x);
#endif
}

void test_mutable_substitution()
{
  using namespace mcrl2::data::detail;

  mutable_map_substitution< std::map< variable, data_expression_with_variables > > sigma;
  variable v("v", sort_nat::nat());
  data_expression e = v;

  data_expression_with_variables e1;
  e1 = e;

  sigma[v] = e;

  // Compile test
  mutable_map_substitution< std::map< variable, variable > > sigmaprime;

  sigma[v] = v;
}

struct my_sort_substitution: public std::unary_function<data::basic_sort, data::sort_expression>
{
  data::sort_expression operator()(const data::basic_sort& s)
  {
    if (s == sort_nat::nat()) {
      return sort_pos::pos();
    }
    return s;
  }
};

void test_sort_substitution()
{
  using namespace mcrl2::data;
  using namespace mcrl2::data::sort_list;
  using namespace mcrl2::data::sort_set;
  using namespace mcrl2::data::sort_bag;

  data_specification specification = parse_data_specification("sort A = struct f(A) | g;");

  // s0 = A
  sort_expression s0 = basic_sort("A");

  // s1 = f(A)|g
  std::vector< structured_sort_constructor_argument > arguments1;
  arguments1.push_back(structured_sort_constructor_argument(basic_sort("A")));
  std::vector< structured_sort_constructor > constructors1;
  constructors1.push_back(structured_sort_constructor("f", arguments1));
  constructors1.push_back(structured_sort_constructor("g"));
  sort_expression s1=structured_sort(constructors1);

  // s2 = f(struct f(A)|g) |g
  std::vector< structured_sort_constructor_argument > arguments2;
  arguments2.push_back(structured_sort_constructor_argument(s1));
  std::vector< structured_sort_constructor > constructors2;
  constructors2.push_back(structured_sort_constructor("f", arguments2));
  constructors2.push_back(structured_sort_constructor("g"));
  sort_expression s2=structured_sort(constructors2);

  std::cout << "s0 = " << data::pp(s0) << std::endl;
  std::cout << "s1 = " << data::pp(s1) << std::endl;
  std::cout << "s2 = " << data::pp(s2) << std::endl;

  // apply the substitution s1 := s0 innermost to s2
  sort_expression s3 = data::replace_sort_expressions(s2, sort_expression_assignment(s1, s0), true);
  std::cout << "s3 = " << data::pp(s3) << std::endl;
  BOOST_CHECK(s3 == s0);

  // apply the substitution s1 := s0 to s2
  sort_expression s4 = data::replace_sort_expressions(s2, sort_expression_assignment(s1, s0), false);
  std::cout << "s4 = " << data::pp(s4) << std::endl;
  BOOST_CHECK(s4 == s2);

}

int test_main(int /* a */, char**  /* aa */)
{
  test_my_assignment_sequence_substitution();
  test_my_list_substitution();

  test_basic();
  test_assignment_sequence_substitution();
  test_list_substitution();
  test_mutable_substitution();
  test_sort_substitution();
  test_indexed_substitution();

  return EXIT_SUCCESS;
}
