// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Test for traversers.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
//#include "mcrl2/bes/find.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/traverser.h"

using namespace mcrl2;
using namespace mcrl2::bes;

class custom_traverser: public boolean_expression_traverser<custom_traverser>
{
  public:
    typedef boolean_expression_traverser<custom_traverser> super;

    using super::enter;
    using super::leave;
    using super::operator();
};

void test_custom_traverser()
{
  custom_traverser t;

  boolean_variable v;
  t(v);

  true_ T;
  t(T);

  boolean_expression e;
  t(e);

  boolean_equation eq;
  t(eq);

  boolean_equation_system<> eqn;
  t(eqn);

}

class traverser1: public boolean_variable_traverser<traverser1>
{
  public:
    typedef boolean_variable_traverser<traverser1> super;

    using super::enter;
    using super::leave;
    using super::operator();

    unsigned int variable_count;
    unsigned int equation_count;
    unsigned int expression_count;

    traverser1()
      : variable_count(0),
        equation_count(0),
        expression_count(0)
    {}

    void enter(const boolean_variable& v)
    {
      variable_count++;
    }

    void enter(const boolean_equation& eq)
    {
      equation_count++;
    }

    void enter(const boolean_expression& x)
    {
      expression_count++;
    }
};

void test_traverser1()
{
  traverser1 t1;
  boolean_expression x = boolean_variable("X");
  t1(x);

  BOOST_CHECK(t1.variable_count == 1);
  BOOST_CHECK(t1.expression_count == 1);
  BOOST_CHECK(t1.equation_count == 0);

  //--------------------------//

  traverser1 t2;

  std::string bes1 =
    "pbes              \n"
    "                  \n"
    "nu X1 = X2 && X1; \n"
    "mu X2 = X1 || X2; \n"
    "                  \n"
    "init X1;          \n"
    ;
  boolean_equation_system<> b;
  std::stringstream from(bes1);
  from >> b;

  t2(b);

  BOOST_CHECK(t2.variable_count == 7);
  BOOST_CHECK(t2.expression_count == 7);
  BOOST_CHECK(t2.equation_count == 2);

}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_custom_traverser();
  test_traverser1();

  return EXIT_SUCCESS;
}
