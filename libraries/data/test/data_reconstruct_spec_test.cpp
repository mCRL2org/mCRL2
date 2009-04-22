// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct_spec_test.cpp
/// \brief Tests data reconstruction of complete specifications.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include <mcrl2/atermpp/atermpp.h>
#include <mcrl2/atermpp/detail/utility.h>

#include <mcrl2/new_data/parser.h>
#include <mcrl2/new_data/data_specification.h>
#include <mcrl2/new_data/basic_sort.h>
#include <mcrl2/new_data/sort_expression.h>
#include <mcrl2/new_data/bool.h>
#include <mcrl2/new_data/real.h>
#include <mcrl2/new_data/int.h>
#include <mcrl2/new_data/nat.h>
#include <mcrl2/new_data/pos.h>
#include <mcrl2/new_data/bool.h>
#include <mcrl2/new_data/detail/data_reconstruct.h>
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace atermpp::detail;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::new_data;

struct compare_term: public std::unary_function<aterm_appl, bool>
{
  const aterm_appl& t_;

  compare_term(const aterm_appl& t)
   : t_(t)
  {}

  template <typename Term>
  bool operator()(Term t1) const
  {
    return t_ == t1;
  }
};

template <typename Term>
bool find_term(Term t1, const aterm_appl& t2)
{
  return find_if(t1, compare_term(t2)) != aterm_appl();
}

void test_find_term()
{
  std::clog << "test_find_term" << std::endl;

  aterm_appl a = make_term("h(x)");
  aterm_appl t = find_if(a, compare_term(a));
  BOOST_CHECK(t == a);
  BOOST_CHECK(find_term(a, a));

  aterm_appl b = make_term("g(y)");
  aterm_appl t1 = find_if(b, compare_term(a));
  BOOST_CHECK(t1 == aterm_appl());
  BOOST_CHECK(find_term(b, a) == false);
}

void test_data_reconstruct_struct()
{
  std::clog << "test_data_reconstruct_struct" << std::endl;

  std::string text =
  "sort D = struct d1 | d2;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string d1_name("d1");
  identifier_string d2_name("d2");
  identifier_string D_name("D");

  aterm_appl d1 = gsMakeStructCons(d1_name, aterm_list(), gsMakeNil());
  aterm_appl d2 = gsMakeStructCons(d2_name, aterm_list(), gsMakeNil());
  aterm_appl s = gsMakeSortStruct(make_list(d1, d2));
  aterm_appl d = gsMakeSortRef(D_name, s);

  std::cerr << rec_data << std::endl;

  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), d2_name));
  BOOST_CHECK(find_term(rec_data(0), D_name));
  BOOST_CHECK(find_term(rec_data(0), d1));
  BOOST_CHECK(find_term(rec_data(0), d2));
  BOOST_CHECK(find_term(rec_data(0), s));
  BOOST_CHECK(find_term(rec_data(0), d));
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_struct_complex()
{
  std::clog << "test_data_reconstruct_struct_complex" << std::endl;

  std::string text =
  "sort D = struct d1 | d2?is_d2 | d3(arg3: Bool)?is_d3;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  // Using knowledge of the internal format, the structured sort should
  // look like the following:
  identifier_string d1_name("d1");
  identifier_string d2_name("d2");
  identifier_string d3_name("d3");
  identifier_string is_d2_name("is_d2");
  identifier_string arg3_name("arg3");
  identifier_string is_d3_name("is_d3");
  identifier_string D_name("D");

  aterm_appl arg3_proj = gsMakeStructProj(arg3_name, sort_bool_::bool_());
  aterm_appl d1 = gsMakeStructCons(d1_name, aterm_list(), gsMakeNil());
  aterm_appl d2 = gsMakeStructCons(d2_name, aterm_list(), is_d2_name);
  aterm_appl d3 = gsMakeStructCons(d3_name, make_list(arg3_proj), is_d3_name);
  aterm_appl s = gsMakeSortStruct(make_list(d1, d2, d3));
  aterm_appl d = gsMakeSortRef(D_name, s);

  // Incrementally check that all elements occur in the sort declarations,
  // such that we can pinpoint any failures.
  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), d2_name));
  BOOST_CHECK(find_term(rec_data(0), d3_name));
  BOOST_CHECK(find_term(rec_data(0), is_d2_name));
  BOOST_CHECK(find_term(rec_data(0), arg3_name));
  BOOST_CHECK(find_term(rec_data(0), is_d3_name));
  BOOST_CHECK(find_term(rec_data(0), arg3_proj));
  BOOST_CHECK(find_term(rec_data(0), D_name));
  BOOST_CHECK(find_term(rec_data(0), d1));
  BOOST_CHECK(find_term(rec_data(0), d2));
  BOOST_CHECK(find_term(rec_data(0), d3));
  BOOST_CHECK(find_term(rec_data(0), s));
  BOOST_CHECK(find_term(rec_data(0), d));

  // Check that maps do not occur in the function declarations
  BOOST_CHECK(!find_term(rec_data(1), d1_name));
  BOOST_CHECK(!find_term(rec_data(1), d2_name));
  BOOST_CHECK(!find_term(rec_data(1), d3_name));
  BOOST_CHECK(!find_term(rec_data(2), is_d2_name));
  BOOST_CHECK(!find_term(rec_data(2), is_d3_name));
  BOOST_CHECK(!find_term(rec_data(2), arg3_name));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_struct_nest()
{
  std::clog << "test_data_reconstruct_struct_nest" << std::endl;

  std::string text =
  "sort D = struct d?is_d;\n"
  "DPos = struct cd(d1:D)?is_cd | cpos(p:Pos)?is_cpos;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string d_name("d");
  identifier_string cd_name("cd");
  identifier_string cpos_name("cpos");
  identifier_string is_d_name("is_d");
  identifier_string d1_name("d1");
  identifier_string is_cd_name("is_cd");
  identifier_string p_name("p");
  identifier_string is_cpos_name("is_cpos");
  identifier_string D_name("D");
  identifier_string DPos_name("DPos");
  basic_sort D_id("D");
  basic_sort DPos_id("DPos");

  aterm_appl d1_proj = gsMakeStructProj(d1_name, D_id);
  aterm_appl p_proj = gsMakeStructProj(p_name, sort_pos::pos());

  aterm_appl d = gsMakeStructCons(d_name, aterm_list(), is_d_name);
  aterm_appl cd = gsMakeStructCons(cd_name, make_list(d1_proj), is_cd_name);
  aterm_appl cpos = gsMakeStructCons(cpos_name, make_list(p_proj), is_cpos_name);

  aterm_appl s1 = gsMakeSortStruct(make_list(d));
  aterm_appl s2 = gsMakeSortStruct(make_list(cd, cpos));
  aterm_appl D = gsMakeSortRef(D_name, s1);
  aterm_appl DPos = gsMakeSortRef(DPos_name, s2);

  BOOST_CHECK(find_term(rec_data(0), d_name));
  BOOST_CHECK(find_term(rec_data(0), cd_name));
  BOOST_CHECK(find_term(rec_data(0), cpos_name));
  BOOST_CHECK(find_term(rec_data(0), is_d_name));
  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), is_cd_name));
  BOOST_CHECK(find_term(rec_data(0), p_name));
  BOOST_CHECK(find_term(rec_data(0), is_cpos_name));
  BOOST_CHECK(find_term(rec_data(0), D_name));
  BOOST_CHECK(find_term(rec_data(0), D_id));
  BOOST_CHECK(find_term(rec_data(0), DPos_name));
  BOOST_CHECK(find_term(rec_data(0), d1_proj));
  BOOST_CHECK(find_term(rec_data(0), p_proj));
  BOOST_CHECK(find_term(rec_data(0), d));
  BOOST_CHECK(find_term(rec_data(0), cd));
  BOOST_CHECK(find_term(rec_data(0), cpos));
  BOOST_CHECK(find_term(rec_data(0), s1));
  BOOST_CHECK(find_term(rec_data(0), s2));
  BOOST_CHECK(find_term(rec_data(0), D));
  BOOST_CHECK(find_term(rec_data(0), DPos));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_simple_constructor()
{
  std::clog << "test_data_reconstruct_simple_constructor" << std::endl;

  std::string text =
  "sort S;\n"
  "cons c: S;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  // some more specific checks:
  identifier_string S_name("S");
  identifier_string c_name("c");
  basic_sort S_id("S");
  new_data::function_symbol c(c_name, S_id);

  BOOST_CHECK(find_term(rec_data(0), S_name));
  BOOST_CHECK(find_term(rec_data(0), S_id));
  BOOST_CHECK(find_term(rec_data(1), c_name));
  BOOST_CHECK(find_term(rec_data(1), c));

  // check that no structured sort has been added
  // This test case is showing the presence of bug #335
  aterm_appl c_struct = gsMakeStructCons(c_name, aterm_list(), gsMakeNil());
  aterm_appl S_struct = gsMakeSortStruct(make_list(c_struct));
  BOOST_CHECK(!find_term(rec_data(0), c_name));
  BOOST_CHECK(!find_term(rec_data(0), c_struct));
  BOOST_CHECK(!find_term(rec_data(0), S_struct));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

/// Test case for issue #344, reported by Yaroslav Usenko
/// c: Bool -> Bool disappeared from the specification after
/// data reconstruction
void test_data_reconstruct_bool_function()
{
  std::clog << "test_data_reconstruct_struct_bool_function" << std::endl;

  std::string text =
  "map c: Bool -> Bool;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  // some more specific checks:
  identifier_string c_name("c");
  sort_expression b = sort_bool_::bool_();
  sort_expression bb = function_sort(make_list(b), b);
  new_data::function_symbol cbb(c_name, bb);

  BOOST_CHECK(find_term(rec_data(2), c_name));
  BOOST_CHECK(find_term(rec_data(2), b));
  BOOST_CHECK(find_term(rec_data(2), bb));
  BOOST_CHECK(find_term(rec_data(2), cbb));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(0))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

/// Test case for issue #351
void test_data_reconstruct_bool_function_one_eq()
{
  std::clog << "test_data_reconstruct_struct_complex" << std::endl;

  std::string text =
  "map c: Bool -> Bool;\n"
  "eqn c(true) = true;\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;


  // some more specific checks:
  identifier_string c_name("c");
  sort_expression b = sort_bool_::bool_();
  sort_expression bb = function_sort(make_list(b), b);
  new_data::function_symbol cbb(c_name, bb);
  data_expression t = sort_bool_::true_();
  application ct(cbb, make_list(t));

  BOOST_CHECK(find_term(rec_data(2), c_name));
  BOOST_CHECK(find_term(rec_data(2), b));
  BOOST_CHECK(find_term(rec_data(2), bb));
  BOOST_CHECK(find_term(rec_data(2), cbb));
  BOOST_CHECK(find_term(rec_data(3), t));
  BOOST_CHECK(find_term(rec_data(3), ct));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(0))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
}

void test_data_reconstruct_list()
{
  std::clog << "test_data_reconstruct_list" << std::endl;

  std::string text =
  "map f:List(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string f_name("f");
  sort_expression b = sort_bool_::bool_();
  aterm_appl list_bool = gsMakeSortExprList(b);
  aterm_appl f = gsMakeOpId(f_name, list_bool);

  BOOST_CHECK(find_term(rec_data(2), f_name));
  BOOST_CHECK(find_term(rec_data(2), list_bool));
  BOOST_CHECK(find_term(rec_data(2), f));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(0))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_list_alias()
{
  std::clog << "test_data_reconstruct_list_alias" << std::endl;

  std::string text =
  "sort S = List(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  sort_expression b = sort_bool_::bool_();
  aterm_appl list_bool = gsMakeSortExprList(b);

  BOOST_CHECK(find_term(rec_data(0), list_bool));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_list_struct()
{
  std::clog << "test_data_reconstruct_list_struct" << std::endl;

  std::string text =
  "sort ListD = List(struct d);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string d_name("d");
  identifier_string ListD_name("ListD");
  aterm_appl d = gsMakeStructCons(d_name, aterm_list(), gsMakeNil());
  aterm_appl s1 = gsMakeSortStruct(make_list(d));
  aterm_appl list_s1 = gsMakeSortExprList(s1);
  aterm_appl ListD = gsMakeSortRef(ListD_name, list_s1);

  BOOST_CHECK(find_term(rec_data(0), d_name));
  BOOST_CHECK(find_term(rec_data(0), ListD_name));
  BOOST_CHECK(find_term(rec_data(0), d));
  BOOST_CHECK(find_term(rec_data(0), s1));
  BOOST_CHECK(find_term(rec_data(0), list_s1));
  BOOST_CHECK(find_term(rec_data(0), ListD));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_bag()
{
  std::clog << "test_data_reconstruct_bag" << std::endl;

  std::string text =
  "map f:Bag(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string f_name("f");
  sort_expression b = sort_bool_::bool_();
  aterm_appl bag_bool = gsMakeSortExprBag(b);
  aterm_appl f = gsMakeOpId(f_name, bag_bool);

  BOOST_CHECK(find_term(rec_data(2), f_name));
  BOOST_CHECK(find_term(rec_data(2), bag_bool));
  BOOST_CHECK(find_term(rec_data(2), f));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(0))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_set()
{
  std::clog << "test_data_reconstruct_set" << std::endl;

  std::string text =
  "map f:Set(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  identifier_string f_name("f");
  sort_expression b = sort_bool_::bool_();
  aterm_appl set_bool = gsMakeSortExprSet(b);
  aterm_appl f = gsMakeOpId(f_name, set_bool);

  BOOST_CHECK(find_term(rec_data(2), f_name));
  BOOST_CHECK(find_term(rec_data(2), set_bool));
  BOOST_CHECK(find_term(rec_data(2), f));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(0))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_bag_alias()
{
  std::clog << "test_data_reconstruct_bag_alias" << std::endl;

  std::string text =
  "sort S = Bag(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  sort_expression b = sort_bool_::bool_();
  aterm_appl bag_bool = gsMakeSortExprBag(b);

  BOOST_CHECK(find_term(rec_data(0), bag_bool));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

void test_data_reconstruct_set_alias()
{
  std::clog << "test_data_reconstruct_set_alias" << std::endl;

  std::string text =
  "sort S = Set(Bool);\n"
  ;

  std::clog << "IN: " << text << std::endl;

  data_specification data = remove_all_system_defined(parse_data_specification(text));
  aterm_appl rec_data = new_data::detail::reconstruct_spec(new_data::detail::data_specification_to_aterm_data_spec(data));

  std::clog << "RECONSTRUCTED: " << rec_data << std::endl;

  sort_expression b = sort_bool_::bool_();
  aterm_appl set_bool = gsMakeSortExprSet(b);

  BOOST_CHECK(find_term(rec_data(0), set_bool));

  BOOST_CHECK(aterm_list(aterm_appl(rec_data(1))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(2))(0)).empty());
  BOOST_CHECK(aterm_list(aterm_appl(rec_data(3))(0)).empty());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_find_term();
  core::garbage_collect();

  test_data_reconstruct_struct();
  core::garbage_collect();

  test_data_reconstruct_struct_complex();
  core::garbage_collect();

  test_data_reconstruct_struct_nest();
  core::garbage_collect();

  test_data_reconstruct_simple_constructor();
  core::garbage_collect();

  test_data_reconstruct_bool_function();
  core::garbage_collect();

  test_data_reconstruct_bool_function_one_eq();
  core::garbage_collect();

  test_data_reconstruct_list();
  core::garbage_collect();

  test_data_reconstruct_list_alias();
  core::garbage_collect();

  test_data_reconstruct_list_struct();
  core::garbage_collect();

  test_data_reconstruct_bag();
  core::garbage_collect();

  test_data_reconstruct_set();
  core::garbage_collect();

  test_data_reconstruct_bag_alias();
  core::garbage_collect();

  test_data_reconstruct_set_alias();
  core::garbage_collect();

  return 0;
}
