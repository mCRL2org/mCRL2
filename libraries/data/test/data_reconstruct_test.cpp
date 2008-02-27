// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include <mcrl2/atermpp/atermpp.h>
#include <mcrl2/atermpp/detail/utility.h>

#include <mcrl2/data/parser.h>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/core/detail/data_reconstruct.h>

using namespace atermpp;
using namespace atermpp::detail;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;

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
  std::string text =
  "sort D = struct d1 | d2;\n"
  ;

  data_specification data = parse_data_specification(text);
  aterm_appl rec_data = reconstruct_spec(data);

  identifier_string d1_name("d1");
  identifier_string d2_name("d2");

  aterm_appl d1 = gsMakeStructCons(d1_name, aterm_list(), gsMakeNil());
  aterm_appl d2 = gsMakeStructCons(d2_name, aterm_list(), gsMakeNil());
  aterm_appl s = gsMakeSortStruct(make_list(d1, d2));
  aterm_appl d = gsMakeSortRef(sort_identifier("D"), s);

  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), d2_name));
  BOOST_CHECK(find_term(rec_data(0), d1));
  BOOST_CHECK(find_term(rec_data(0), d2));
  BOOST_CHECK(find_term(rec_data(0), s));
  BOOST_CHECK(find_term(rec_data(0), d));
}

void test_data_reconstruct_struct_complex()
{
  std::string text =
  "sort D = struct d1 | d2?is_d2 | d3(arg3: Bool)?is_d3;\n"
  ;

  data_specification data = parse_data_specification(text);
  aterm_appl rec_data = reconstruct_spec(data);

  // Using knowledge of the internal format, the structured sort should
  // look like the following:
  identifier_string d1_name("d1");
  identifier_string d2_name("d2");
  identifier_string d3_name("d3");
  identifier_string is_d2_name("is_d2");
  identifier_string arg3_name("arg3");
  identifier_string is_d3_name("is_d3");
  identifier_string arg3_proj = gsMakeStructProj(arg3_name, sort_expr::bool_());

  aterm_appl d1 = gsMakeStructCons(d1_name, aterm_list(), gsMakeNil());
  aterm_appl d2 = gsMakeStructCons(d2_name, aterm_list(), is_d2_name);
  aterm_appl d3 = gsMakeStructCons(d3_name, make_list(arg3_proj), is_d3_name);
  aterm_appl s = gsMakeSortStruct(make_list(d1, d2, d3));
  aterm_appl d = gsMakeSortRef(sort_identifier("D"), s);

  // Incrementally check that all elements occur in the sort declarations,
  // such that we can pinpoint any failures.
  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), d2_name));
  BOOST_CHECK(find_term(rec_data(0), d3_name));
  BOOST_CHECK(find_term(rec_data(0), is_d2_name));
  BOOST_CHECK(find_term(rec_data(0), arg3_name));
  BOOST_CHECK(find_term(rec_data(0), is_d3_name));
  BOOST_CHECK(find_term(rec_data(0), arg3_proj));
  BOOST_CHECK(find_term(rec_data(0), d1));
  BOOST_CHECK(find_term(rec_data(0), d2));
  BOOST_CHECK(find_term(rec_data(0), d3));
  BOOST_CHECK(find_term(rec_data(0), s));
  BOOST_CHECK(find_term(rec_data(0), d));
}

void test_data_reconstruct_struct_nest()
{
  std::string text =
  "sort D = struct d?is_d;\n"
  "DPos = struct cd(d1:D)?is_cd | cpos(p:Pos)?is_cpos;\n"
  ;

  data_specification data = parse_data_specification(text);
  aterm_appl rec_data = reconstruct_spec(data);

  identifier_string d_name("d");
  identifier_string cd_name("cd");
  identifier_string cpos_name("cpos");
  identifier_string is_d_name("is_d");
  identifier_string d1_name("d1");
  identifier_string is_cd_name("is_cd");
  identifier_string p_name("p");
  identifier_string is_cpos_name("is_cpos");
  sort_identifier D_id("D");
  sort_identifier DPos_id("DPos");

  aterm_appl d1_proj = gsMakeStructProj(d1_name, D_id);
  aterm_appl p_proj = gsMakeStructProj(p_name, sort_expr::pos());

  aterm_appl d = gsMakeStructCons(d1_name, aterm_list(), is_d_name);
  aterm_appl cd = gsMakeStructCons(cd_name, make_list(d1_proj), is_cd_name);
  aterm_appl cpos = gsMakeStructCons(cpos_name, make_list(p_proj), is_cpos_name);

  aterm_appl s1 = gsMakeSortStruct(make_list(d));
  aterm_appl s2 = gsMakeSortStruct(make_list(cd, cpos));
  aterm_appl D = gsMakeSortRef(D_id, s1);
  aterm_appl DPos = gsMakeSortRef(DPos_id, s2);

  BOOST_CHECK(find_term(rec_data(0), d_name));
  BOOST_CHECK(find_term(rec_data(0), cd_name));
  BOOST_CHECK(find_term(rec_data(0), cpos_name));
  BOOST_CHECK(find_term(rec_data(0), is_d_name));
  BOOST_CHECK(find_term(rec_data(0), d1_name));
  BOOST_CHECK(find_term(rec_data(0), is_cd_name));
  BOOST_CHECK(find_term(rec_data(0), p_name));
  BOOST_CHECK(find_term(rec_data(0), is_cpos_name));
  BOOST_CHECK(find_term(rec_data(0), D_id));
  BOOST_CHECK(find_term(rec_data(0), DPos_id));
  BOOST_CHECK(find_term(rec_data(0), d1_proj));
  BOOST_CHECK(find_term(rec_data(0), p_proj));
  BOOST_CHECK(find_term(rec_data(0), d));
  BOOST_CHECK(find_term(rec_data(0), cd));
  BOOST_CHECK(find_term(rec_data(0), cpos));
  BOOST_CHECK(find_term(rec_data(0), s1));
  BOOST_CHECK(find_term(rec_data(0), s2));
  BOOST_CHECK(find_term(rec_data(0), D));
  BOOST_CHECK(find_term(rec_data(0), DPos));
}

void test_multiple_reconstruct_calls()
{
  test_find_term();
  test_data_reconstruct_struct();
  test_data_reconstruct_struct_complex();
  test_data_reconstruct_struct_nest();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_multiple_reconstruct_calls();

  return 0;
}
