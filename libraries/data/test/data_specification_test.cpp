// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_specification_test.cpp
/// \brief Basic regression test for data specifications.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void test_sorts()
{
  basic_sort s("S");
  basic_sort s0("S0");
  alias s1("S1", s);
 
  sort_expression_list sl;
  sl.push_back(s);
  sl.push_back(s0);
  sl.push_back(s1);
  boost::iterator_range<sort_expression_list::const_iterator> sl_range(boost::make_iterator_range(sl));

  data_specification spec;
  spec.add_sort(s);
  spec.add_sort(s0);
  spec.add_sort(s1);
  data_specification spec1;
  spec1.add_sorts(sl_range);

  BOOST_CHECK(spec.sorts() == sl_range);
  BOOST_CHECK(spec1.sorts() == sl_range);
  BOOST_CHECK(spec == spec1);

  basic_sort s2("S2");
  sort_expression_list s2l(make_vector(reinterpret_cast<sort_expression&>(s2)));
  boost::iterator_range<sort_expression_list::const_iterator> s2l_range(s2l);
  spec.add_system_defined_sort(s2);
  spec1.add_system_defined_sorts(s2l_range);
  BOOST_CHECK(spec == spec1);

  BOOST_CHECK(spec.is_system_defined(s2));
  BOOST_CHECK(!spec.is_system_defined(s1));
  BOOST_CHECK(!spec.is_system_defined(s0));
  BOOST_CHECK(!spec.is_system_defined(s));
  BOOST_CHECK(spec1.is_system_defined(s2));
  BOOST_CHECK(!spec1.is_system_defined(s1));
  BOOST_CHECK(!spec1.is_system_defined(s0));
  BOOST_CHECK(!spec1.is_system_defined(s));
  spec.remove_sorts(s2l_range);
  spec1.remove_sort(s2);
  BOOST_CHECK(spec == spec1);
}

void test_constructors()
{
  basic_sort s("S");
  basic_sort s0("S0");
  function_sort s0s(make_vector(static_cast<sort_expression>(s0)),s);
  function_symbol f("f", s);
  function_symbol g("g", s0s);
  function_symbol h("h", s0);
  function_symbol_list fgl(make_vector(f,g));
  function_symbol_list hl(make_vector(h));
  function_symbol_list fghl(make_vector(f,g,h));
  boost::iterator_range<function_symbol_list::const_iterator> fgl_range(boost::make_iterator_range(fgl));
  boost::iterator_range<function_symbol_list::const_iterator> hl_range(boost::make_iterator_range(hl));
  boost::iterator_range<function_symbol_list::const_iterator> fghl_range(boost::make_iterator_range(fghl));
  
  data_specification spec;
  spec.add_sort(s);
  spec.add_sort(s0);
  data_specification spec1(spec);

  spec.add_constructor(f);
  spec.add_constructor(g);
  spec.add_constructor(h);
  spec1.add_constructors(fghl_range);

  BOOST_CHECK(spec.constructors(s) == fgl_range);
  BOOST_CHECK(spec.constructors().size() == 3);
  BOOST_CHECK(std::find(spec.constructors().begin(), spec.constructors().end(), f) != spec.constructors().end());
  BOOST_CHECK(std::find(spec.constructors().begin(), spec.constructors().end(), g) != spec.constructors().end());
  BOOST_CHECK(std::find(spec.constructors().begin(), spec.constructors().end(), h) != spec.constructors().end());

  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.constructors() == spec1.constructors());
  BOOST_CHECK(spec.constructors(s) == fgl_range);
  BOOST_CHECK(spec.constructors(s0) == hl_range);
  BOOST_CHECK(spec1.constructors(s) == fgl_range);
  BOOST_CHECK(spec1.constructors(s0) == hl_range);
  spec.add_constructor(function_symbol("i", s0));
  function_symbol i("i", s0);
  spec.remove_constructor(i);
  BOOST_CHECK(spec == spec1);

  spec.add_system_defined_constructor(i);
  function_symbol_list il(make_vector(i));
  boost::iterator_range<function_symbol_list::const_iterator> il_range(il);
  spec1.add_system_defined_constructors(il_range);
  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.is_system_defined(i));
  BOOST_CHECK(!spec.is_system_defined(f));
  BOOST_CHECK(!spec.is_system_defined(g));
  BOOST_CHECK(!spec.is_system_defined(h));
  BOOST_CHECK(spec1.is_system_defined(i));
  BOOST_CHECK(!spec1.is_system_defined(f));
  BOOST_CHECK(!spec1.is_system_defined(g));
  BOOST_CHECK(!spec1.is_system_defined(h));

  spec.remove_constructor(i);
  spec1.remove_constructors(il_range);
  BOOST_CHECK(spec == spec1);
}

void test_functions()
{
  basic_sort s("S");
  basic_sort s0("S0");
  function_sort s0s(make_vector(static_cast<sort_expression>(s0)), s);
  function_symbol f("f", s);
  function_symbol g("g", s0s);
  function_symbol h("h", s0);

  function_symbol_list fgl(make_vector(f,g));
  function_symbol_list hl(make_vector(h));
  function_symbol_list fghl(make_vector(f,g,h));
  boost::iterator_range<function_symbol_list::const_iterator> fgl_range(boost::make_iterator_range(fgl));
  boost::iterator_range<function_symbol_list::const_iterator> hl_range(boost::make_iterator_range(hl));
  boost::iterator_range<function_symbol_list::const_iterator> fghl_range(boost::make_iterator_range(fghl));

  data_specification spec;
  spec.add_sort(s);
  spec.add_sort(s0);
  data_specification spec1(spec);
  spec.add_function(f);
  spec.add_function(g);
  spec.add_function(h);
  spec1.add_functions(fghl);

  BOOST_CHECK(spec.functions().size() == 3);
  BOOST_CHECK(std::find(spec.functions().begin(), spec.functions().end(), f) != spec.functions().end());
  BOOST_CHECK(std::find(spec.functions().begin(), spec.functions().end(), g) != spec.functions().end());
  BOOST_CHECK(std::find(spec.functions().begin(), spec.functions().end(), h) != spec.functions().end());

  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.functions(s).size() == 2);
  BOOST_CHECK(std::find(spec.functions(s).begin(), spec.functions(s).end(), f) != spec.functions(s).end());
  BOOST_CHECK(std::find(spec.functions(s).begin(), spec.functions(s).end(), g) != spec.functions(s).end());
  BOOST_CHECK(std::find(spec.functions(s0).begin(), spec.functions(s0).end(), h) != spec.functions(s0).end());
  BOOST_CHECK(spec.functions() == fghl_range);
  BOOST_CHECK(spec1.functions(s) == fgl_range);
  BOOST_CHECK(spec1.functions(s0) == hl_range);
  BOOST_CHECK(spec1.functions() == fghl_range);
  BOOST_CHECK(spec1.functions(s).size() == 2);
  BOOST_CHECK(std::find(spec1.functions(s).begin(), spec1.functions(s).end(), f) != spec1.functions(s).end());
  BOOST_CHECK(std::find(spec1.functions(s).begin(), spec1.functions(s).end(), g) != spec1.functions(s).end());
  BOOST_CHECK(std::find(spec1.functions(s0).begin(), spec1.functions(s0).end(), h) != spec1.functions(s0).end());

  function_symbol i("i", s0);
  spec.add_system_defined_function(i);
  function_symbol_list il(make_vector(i));
  boost::iterator_range<function_symbol_list::const_iterator> il_range(il);
  spec1.add_system_defined_functions(il_range);
  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.is_system_defined(i));
  BOOST_CHECK(!spec.is_system_defined(f));
  BOOST_CHECK(!spec.is_system_defined(g));
  BOOST_CHECK(!spec.is_system_defined(h));
  BOOST_CHECK(spec1.is_system_defined(i));
  BOOST_CHECK(!spec1.is_system_defined(f));
  BOOST_CHECK(!spec1.is_system_defined(g));
  BOOST_CHECK(!spec1.is_system_defined(h));

  spec.remove_functions(il_range);
  spec1.remove_function(i);
  BOOST_CHECK(spec == spec1);  
}

void test_equations()
{
  basic_sort s("S");
  basic_sort s0("S0");
  function_sort s0s(make_vector(reinterpret_cast<sort_expression&>(s0)), s);
  function_symbol f("f", s0s);
  variable x("x", s0);
  data_expression_list xel(make_vector(reinterpret_cast<data_expression&>(x)));
  application fx(f, xel);
  variable_list xl(make_vector(x));
  boost::iterator_range<variable_list::const_iterator> x_range(xl);
  data_equation fxx(x_range, x, fx, x);

  data_specification spec;
  data_specification spec1;
  spec.add_sort(s);
  spec.add_sort(s0);
  spec1 = spec;
  spec.add_equation(fxx);
  data_equation_list fxxl(make_vector(fxx));
  boost::iterator_range<data_equation_list::const_iterator> fxxl_range(fxxl);
  spec1.add_equations(fxxl_range);

  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.equations() == fxxl_range);
  BOOST_CHECK(spec1.equations() == fxxl_range);

  data_equation fxf(x_range, x, fx, f);
  data_equation_list fxfl(make_vector(fxf));
  boost::iterator_range<data_equation_list::const_iterator> fxfl_range(fxfl);
  spec.add_system_defined_equation(fxf);
  spec1.add_system_defined_equations(fxfl_range);

  BOOST_CHECK(spec == spec1);
  BOOST_CHECK(spec.is_system_defined(fxf));
  BOOST_CHECK(!spec.is_system_defined(fxx));
  BOOST_CHECK(spec1.is_system_defined(fxf));
  BOOST_CHECK(!spec1.is_system_defined(fxx));

  BOOST_CHECK(spec.equations(f).size() == 2);
  BOOST_CHECK(std::find(spec.equations(f).begin(), spec.equations(f).end(), fxf) != spec.equations(f).end());
  BOOST_CHECK(std::find(spec.equations(f).begin(), spec.equations(f).end(), fxx) != spec.equations(f).end());
  spec.remove_equations(fxfl_range);
  spec1.remove_equation(fxf);
  BOOST_CHECK(spec == spec1);
}

void test_is_certainly_finite()
{
  basic_sort s("S");
  basic_sort s0("S0");
  function_sort s0s0(make_vector(reinterpret_cast<sort_expression&>(s0)), s0);
  function_symbol f("f", s);
  function_symbol g("g", s0s0);
  variable x("x", s0);
  application gx(g, make_vector(reinterpret_cast<data_expression&>(x)));
  data_specification spec;
  spec.add_sort(s);
  spec.add_sort(s0);
  spec.add_constructor(f);
  spec.add_constructor(g);
  BOOST_CHECK(spec.is_certainly_finite(s));
  BOOST_CHECK(!spec.is_certainly_finite(s0));
  // TODO extend with tests for other kinds of sort
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_sorts();
  test_constructors();
  test_functions();
  test_equations();
  test_is_certainly_finite();

  return EXIT_SUCCESS;
}


