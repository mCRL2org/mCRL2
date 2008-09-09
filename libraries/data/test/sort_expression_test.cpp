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

#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/list_sort.h"
#include "mcrl2/data/set_sort.h"
#include "mcrl2/data/bag_sort.h"

using namespace mcrl2::data;

void basic_sort_test()
{
  basic_sort s("S");
  BOOST_CHECK(s.is_basic_sort());
  BOOST_CHECK(!s.is_function_sort());
  BOOST_CHECK(!s.is_alias());
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
  
  sort_expression_list s01;
  s01.push_back(s0);
  s01.push_back(s1);
  boost::iterator_range<sort_expression_list::const_iterator> s01_range = boost::make_iterator_range(s01);
  function_sort fs(s01_range, s);
  BOOST_CHECK(!fs.is_basic_sort());
  BOOST_CHECK(fs.is_function_sort());
  BOOST_CHECK(!fs.is_alias());
  BOOST_CHECK(!fs.is_structured_sort());
  BOOST_CHECK(!fs.is_container_sort());
  BOOST_CHECK(fs == fs);
  BOOST_CHECK(fs.domain().size() == s01_range.size());

  // Element wise check
  sort_expression_list::const_iterator i = s01_range.begin();
  sort_expression_list::const_iterator j = fs.domain().begin();
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

  std::string s0_name("other_S");
  alias s0_(s0_name, s0);
  BOOST_CHECK(!s0_.is_basic_sort());
  BOOST_CHECK(!s0_.is_function_sort());
  BOOST_CHECK(s0_.is_alias());
  BOOST_CHECK(!s0_.is_structured_sort());
  BOOST_CHECK(!s0_.is_container_sort());
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
  structured_sort_constructor_argument p0("p0", s0);
  structured_sort_constructor_argument p1(s1);
  BOOST_CHECK(p0.name() == "p0");
  BOOST_CHECK(p1.name() == std::string());
  BOOST_CHECK(p0.sort() == s0);
  BOOST_CHECK(p1.sort() == s1);

  structured_sort_constructor_argument_list a1;
  a1.push_back(p0);
  a1.push_back(p1);
  boost::iterator_range<structured_sort_constructor_argument_list::const_iterator> a1_range(a1);
  structured_sort_constructor_argument_list a2;
  a2.push_back(p0);
  boost::iterator_range<structured_sort_constructor_argument_list::const_iterator> a2_range(a2);

  structured_sort_constructor c1("c1", a1_range, "is_c1");
  structured_sort_constructor c2("c2", a2_range);
  BOOST_CHECK(c1.name() == "c1");
  BOOST_CHECK(c1.arguments() == a1_range);
  BOOST_CHECK(c1.recogniser() == "is_c1");
  BOOST_CHECK(c2.name() == "c2");
  BOOST_CHECK(c2.arguments() == a2_range);
  BOOST_CHECK(c2.recogniser() == std::string());

  structured_sort_constructor_list cs;
  cs.push_back(c1);
  cs.push_back(c2);

  structured_sort s(cs);

  BOOST_CHECK(!s.is_basic_sort());
  BOOST_CHECK(!s.is_function_sort());
  BOOST_CHECK(!s.is_alias());
  BOOST_CHECK(s.is_structured_sort());
  BOOST_CHECK(!s.is_container_sort());

  BOOST_CHECK(s.struct_constructors() == cs);
  
  sort_expression s_e(s);
  structured_sort s_e_(s_e);
  BOOST_CHECK(s_e_ == s);
  BOOST_CHECK(s_e_.struct_constructors() == s.struct_constructors());
}

void container_sort_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  list_sort ls0(s0);
  list_sort ls1(s1);
  set_sort ss0(s0);
  set_sort ss1(s1);
  bag_sort bs0(s0);
  bag_sort bs1(s1);

  BOOST_CHECK(ls0.container_name() == "List");
  BOOST_CHECK(ls0.element_sort() == s0);
  BOOST_CHECK(ls0.is_list_sort());
  BOOST_CHECK(!ls0.is_set_sort());
  BOOST_CHECK(!ls0.is_bag_sort());
  BOOST_CHECK(ls0.element_sort() != ls1.element_sort());

  BOOST_CHECK(ss0.container_name() == "Set");
  BOOST_CHECK(ss0.element_sort() == s0);
  BOOST_CHECK(!ss0.is_list_sort());
  BOOST_CHECK(ss0.is_set_sort());
  BOOST_CHECK(!ss0.is_bag_sort());
  BOOST_CHECK(ss0.element_sort() != ss1.element_sort());

  BOOST_CHECK(bs0.container_name() == "Bag");
  BOOST_CHECK(bs0.element_sort() == s0);
  BOOST_CHECK(!bs0.is_list_sort());
  BOOST_CHECK(!bs0.is_set_sort());
  BOOST_CHECK(bs0.is_bag_sort());
  BOOST_CHECK(ss0.element_sort() != ss1.element_sort());

  container_sort cls0(ls0);
  BOOST_CHECK(cls0.container_name() == "List");
  BOOST_CHECK(cls0.is_list_sort());
  list_sort cls0_(cls0);
  BOOST_CHECK(cls0_ == ls0);
  BOOST_CHECK(cls0_.container_name() == ls0.container_name());
  BOOST_CHECK(cls0_.element_sort() == ls0.element_sort());
  sort_expression cls0_e(cls0);
  container_sort cls0_e_(cls0_e);
  BOOST_CHECK(cls0_e_ == cls0);
  BOOST_CHECK(cls0_e_.container_name() == cls0.container_name());
  BOOST_CHECK(cls0_e_.element_sort() == cls0.element_sort());

  container_sort css0(ss0);
  BOOST_CHECK(css0.container_name() == "Set");
  BOOST_CHECK(css0.is_set_sort());
  set_sort css0_(css0);
  BOOST_CHECK(css0_ == ss0);
  BOOST_CHECK(css0_.container_name() == ss0.container_name());
  BOOST_CHECK(css0_.element_sort() == ss0.element_sort());
  sort_expression css0_e(css0);
  container_sort css0_e_(css0_e);
  BOOST_CHECK(css0_e_ == css0);
  BOOST_CHECK(css0_e_.container_name() == css0.container_name());
  BOOST_CHECK(css0_e_.element_sort() == css0.element_sort());

  container_sort cbs0(bs0);
  BOOST_CHECK(cbs0.container_name() == "Bag");
  BOOST_CHECK(cbs0.is_bag_sort());
  bag_sort cbs0_(cbs0);
  BOOST_CHECK(cbs0_ == bs0);
  BOOST_CHECK(cbs0_.container_name() == bs0.container_name());
  BOOST_CHECK(cbs0_.element_sort() == bs0.element_sort());
  sort_expression cbs0_e(cbs0);
  container_sort cbs0_e_(cbs0_e);
  BOOST_CHECK(cbs0_e_ == cbs0);
  BOOST_CHECK(cbs0_e_.container_name() == cbs0.container_name());
  BOOST_CHECK(cbs0_e_.element_sort() == cbs0.element_sort());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  basic_sort_test();
  function_sort_test();
  alias_test();
  structured_sort_test();
  container_sort_test();

  return EXIT_SUCCESS;
}


