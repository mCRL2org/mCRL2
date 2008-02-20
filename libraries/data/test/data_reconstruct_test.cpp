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
//  BOOST_CHECK(find_term(rec_data(0), d));
}

void test_multiple_reconstruct_calls()
{
  test_find_term();
  test_data_reconstruct_struct();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  test_multiple_reconstruct_calls();

  return 0;
}
