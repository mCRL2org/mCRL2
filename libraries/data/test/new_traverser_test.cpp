// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Traverser tests.

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/new_find.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/data/binding_aware_traverser.h"

using namespace mcrl2;
using namespace mcrl2::data;

class my_traverser: public data::traverser<my_traverser>
{
public:
  typedef data::traverser<my_traverser> super;

#if BOOST_MSVC
  // Workaround for malfunctioning MSVC 2008 overload resolution
  template <typename Container>
  void operator()(Container const& x)
  {
    super::operator()(x);
  }
#endif
};
  
void test_my_traverser()
{
  my_traverser t;

  data_expression d;
  t(d);

  data_expression_list dl;
  t(dl);

  variable v;
  t(v);
  
  data_equation eq;
  t(eq);
}

class my_binding_aware_traverser: public data::binding_aware_traverser<my_binding_aware_traverser>
{
public:
  typedef data::binding_aware_traverser<my_binding_aware_traverser> super;

  // This is essential, to make the empty default implementation of enter visible.
  using super::enter;

  void enter(const data_expression& x)
  {
    std::cout << "Entering " << pp(x) << " with bound variables ";
    for (std::multiset<variable_type>::iterator i = m_bound_variables.begin(); i != m_bound_variables.end(); ++i)
    {
      std::cout << pp(*i) << " ";
    }
    std::cout << std::endl;
  }

// TODO: It is not exactly clear yet why this overload is necessary
#if BOOST_MSVC
  template <typename Container>
  void operator()(Container const& x)
  {
    super::operator()(x);
  }
#endif

};

template <typename T>
void test_my_binding_aware_traverser(const T& x)
{
  my_binding_aware_traverser t;
  t(x);
}

void test_my_binding_aware_traverser()
{
  data_expression d = variable("n", sort_nat::nat());
  test_my_binding_aware_traverser(d);
  
  data_expression_list dl;
  test_my_binding_aware_traverser(dl);

  variable var("n", sort_nat::nat());
  test_my_binding_aware_traverser(var);
  
  data_equation eq(variable("b", sort_bool::bool_()), variable("b", sort_bool::bool_()), variable("b", sort_bool::bool_()));
  test_my_binding_aware_traverser(eq);
  
  variable_vector v;
  v.push_back(variable("x", sort_nat::nat()));
  v.push_back(variable("y", sort_nat::nat()));
  data_expression vbody = variable("b", sort_bool::bool_());
  data_expression z = forall(v, vbody);
  test_my_binding_aware_traverser(z);

  variable_vector w;
  w.push_back(variable("z", sort_nat::nat()));
  data_expression wbody = exists(w, z);
  z = forall(w, wbody);
  test_my_binding_aware_traverser(z); 
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_my_traverser();
  test_my_binding_aware_traverser();

  return EXIT_SUCCESS;
}
