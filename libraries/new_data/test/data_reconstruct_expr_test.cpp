// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_reconstruct_expr_test.cpp
/// \brief Tests data reconstruction of expressions against a specification.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include <mcrl2/atermpp/atermpp.h>
#include <mcrl2/atermpp/detail/utility.h>

#include <mcrl2/new_data/parser.h>
#include <mcrl2/new_data/data_specification.h>
#include <mcrl2/new_data/basic_sort.h>
#include <mcrl2/new_data/sort_expression.h>
#include <mcrl2/new_data/detail/data_reconstruct.h>

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

void test_data_reconstruct_sort_expr()
{
  std::string text =
  "map f: List(Nat) -> List(Nat);\n"
  ;

  data_specification data = parse_data_specification(text);

  sort_expression l(basic_sort("List@0"));
  std::cerr << l << std::endl;
  BOOST_CHECK(find_term(new_data::detail::data_specification_to_aterm_data_spec(data)(2),l));

  BOOST_CHECK(new_data::detail::reconstruct_exprs(aterm(l),
         new_data::detail::data_specification_to_aterm_data_spec(data)) != l);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_data_reconstruct_sort_expr();

  return 0;
}
