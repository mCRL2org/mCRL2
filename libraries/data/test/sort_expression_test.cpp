// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_expression_test.cpp
/// \brief Basic regression test for sort expressions.

#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void basic_sort_test()
{
  basic_sort s("S");
  BOOST_CHECK(s.is_basic_sort());
  BOOST_CHECK(!s.is_function_sort());
  BOOST_CHECK(!is_alias(s));
  BOOST_CHECK(!s.is_structured_sort());
  BOOST_CHECK(!s.is_container_sort());
  BOOST_CHECK(s.name() == "S");
  BOOST_CHECK(s == s);

  basic_sort t("T");
  BOOST_CHECK(s != t);
  BOOST_CHECK(s.name() != t.name());

  sort_expression t_e(t);
  basic_sort t_e_(t_e);
  BOOST_CHECK(t_e_ == t);
  BOOST_CHECK(t_e_.name() == t.name());
}

void function_sort_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  basic_sort s("S");

  sort_expression_vector s01;
  s01.push_back(s0);
  s01.push_back(s1);
  boost::iterator_range<sort_expression_vector::const_iterator> s01_range = boost::make_iterator_range(s01);
  function_sort fs(s01_range, s);
  BOOST_CHECK(!fs.is_basic_sort());
  BOOST_CHECK(fs.is_function_sort());
  BOOST_CHECK(!is_alias(fs));
  BOOST_CHECK(!fs.is_structured_sort());
  BOOST_CHECK(!fs.is_container_sort());
  BOOST_CHECK(fs == fs);
  BOOST_CHECK(fs.domain().size() == static_cast< size_t >(s01_range.size()));

  // Element wise check
  sort_expression_vector::const_iterator i = s01_range.begin();
  function_sort::domain_const_range::iterator j = fs.domain().begin();
  while (i != s01_range.end() && j != fs.domain().end())
  {
    BOOST_CHECK(*i == *j);
    ++i;
    ++j;
  }

  BOOST_CHECK(fs.domain() == s01_range);
  BOOST_CHECK(fs.codomain() == s);

  sort_expression fs_e(fs);
  function_sort fs_e_(fs_e);
  BOOST_CHECK(fs_e_ == fs);
  BOOST_CHECK(fs_e_.domain() == fs.domain());
  BOOST_CHECK(fs_e_.codomain() == fs.codomain());

  BOOST_CHECK(fs == function_sort(s0, s1, s));
  BOOST_CHECK(fs.domain() == function_sort(s0, s1, s).domain());
}

void alias_test()
{
  basic_sort s0("S0");

  basic_sort s0_name("other_S");
  alias s0_(s0_name, s0);
  BOOST_CHECK(is_alias(s0_));
  BOOST_CHECK(s0_.name() == s0_name);
  BOOST_CHECK(s0_.reference() == s0);

  sort_expression s0_e(s0_);
  alias s0_e_(s0_e);
  BOOST_CHECK(s0_e_ == s0_);
  BOOST_CHECK(s0_e_.name() == s0_.name());
  BOOST_CHECK(s0_e_.reference() == s0_.reference());
}

void structured_sort_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  structured_sort_constructor_argument p0(s0, "p0");
  structured_sort_constructor_argument p1(s1);
  BOOST_CHECK(p0.name() == "p0");
  BOOST_CHECK(p1.name() == data::no_identifier());
  BOOST_CHECK(p0.sort() == s0);
  BOOST_CHECK(p1.sort() == s1);

  structured_sort_constructor_argument_vector a1;
  a1.push_back(p0);
  a1.push_back(p1);
  boost::iterator_range<structured_sort_constructor_argument_vector::const_iterator> a1_range(a1);
  structured_sort_constructor_argument_vector a2;
  a2.push_back(p0);
  boost::iterator_range<structured_sort_constructor_argument_vector::const_iterator> a2_range(a2);

  structured_sort_constructor c1("c1", a1_range, "is_c1");
  structured_sort_constructor c2("c2", a2_range);
  BOOST_CHECK(c1.name() == "c1");
  BOOST_CHECK(c1.arguments() == a1_range);
  BOOST_CHECK(c1.recogniser() == "is_c1");
  BOOST_CHECK(c2.name() == "c2");
  BOOST_CHECK(c2.arguments() == a2_range);
  BOOST_CHECK(c2.recogniser() == data::no_identifier());

  structured_sort_constructor_vector cs;
  cs.push_back(c1);
  cs.push_back(c2);

  structured_sort s(cs);

  BOOST_CHECK(!s.is_basic_sort());
  BOOST_CHECK(!s.is_function_sort());
  BOOST_CHECK(!is_alias(s));
  BOOST_CHECK(s.is_structured_sort());
  BOOST_CHECK(!s.is_container_sort());

  BOOST_CHECK(s.struct_constructors() == cs);

  sort_expression s_e(s);
  structured_sort s_e_(s_e);
  BOOST_CHECK(s_e_ == s);
  BOOST_CHECK(s_e_.struct_constructors() == s.struct_constructors());

  structured_sort_constructor_argument_vector nv(make_vector(structured_sort_constructor_argument(sort_nat::nat())));
  structured_sort_constructor_argument_vector bv(make_vector(structured_sort_constructor_argument(sort_bool::bool_())));
  structured_sort_constructor b("B", boost::make_iterator_range(nv));
  structured_sort_constructor c("C", boost::make_iterator_range(bv));
  structured_sort bc(make_vector(b,c));

  BOOST_CHECK(bc.struct_constructors() == make_vector(b,c));
  structured_sort_constructor_vector bc_constructors(bc.struct_constructors().begin(), bc.struct_constructors().end());
  BOOST_CHECK(bc_constructors[0] == b);
  BOOST_CHECK(bc_constructors[1] == c);
  BOOST_CHECK(!bc_constructors[0].arguments().empty());
  BOOST_CHECK(!bc_constructors[1].arguments().empty());
  BOOST_CHECK(sort_nat::is_nat(bc_constructors[0].arguments().begin()->sort()));
  BOOST_CHECK(sort_bool::is_bool(bc_constructors[1].arguments().begin()->sort()));
}

void container_sort_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  container_sort ls0(container_sort::list(), s0);
  container_sort ls1(container_sort::list(), s1);

  BOOST_CHECK(ls0.container_type() == container_sort::list());
  BOOST_CHECK(ls0.element_sort() == s0);
  BOOST_CHECK(ls1.element_sort() == s1);
  BOOST_CHECK(ls0.element_sort() != ls1.element_sort());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  basic_sort_test();
  core::garbage_collect();

  function_sort_test();
  core::garbage_collect();

  alias_test();
  core::garbage_collect();

  structured_sort_test();
  core::garbage_collect();

  container_sort_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}


