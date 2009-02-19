// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/bag.h"
#include "mcrl2/new_data/structured_sort.h"
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/parser.h"
#include "mcrl2/new_data/replace.h"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/detail/data_functional.h"
#include "mcrl2/new_data/detail/implement_data_types.h"
#include "mcrl2/new_data/detail/data_specification_compatibility.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::new_data;
using namespace mcrl2::new_data::detail;

template <typename Rewriter>
void data_rewrite_test(Rewriter& R, data_expression const& input, data_expression const& expected_output) {
  data_expression output = R(input);

  BOOST_CHECK(output == expected_output);

  if (output != expected_output) {
    std::clog << "--- test failed --- " << core::pp(input) << " ->* " << core::pp(expected_output) << std::endl
              << "input    " << core::pp(input) << std::endl
              << "expected " << core::pp(expected_output) << std::endl
              << "R(input) " << core::pp(output) << std::endl
              << " -- term representations -- " << std::endl
              << "input    " << input << std::endl
              << "expected " << expected_output << std::endl
              << "R(input) " << output << std::endl;
  }
}

void bool_rewrite_test() {
  using namespace mcrl2::new_data::sort_bool_;

  data_specification specification = parse_data_specification(
    ""
  );

  new_data::rewriter R(specification);

  data_rewrite_test(R, true_(), true_());
  data_rewrite_test(R, false_(), false_());

  data_rewrite_test(R, and_(true_(), false_()), false_());
  data_rewrite_test(R, and_(false_(), true_()), false_());

  data_rewrite_test(R, or_(true_(), false_()), true_());
  data_rewrite_test(R, or_(false_(), true_()), true_());

  data_rewrite_test(R, implies(true_(), false_()), false_());
  data_rewrite_test(R, implies(false_(), true_()), true_());
}

void pos_rewrite_test() {
  using namespace mcrl2::new_data::sort_pos;

  data_specification specification = parse_data_specification(
    "sort A = Pos;"
  );

  new_data::rewriter R(specification);

  data_expression p1(R(parse_data_expression("1")));
  data_expression p2(R(parse_data_expression("2")));
  data_expression p3(R(parse_data_expression("3")));
  data_expression p4(R(parse_data_expression("4")));

  data_rewrite_test(R, sort_pos::plus(p1, p2), p3);
  data_rewrite_test(R, sort_pos::plus(p2, p1), p3);

  data_rewrite_test(R, sort_pos::times(p1, p1), p1);
  data_rewrite_test(R, sort_pos::times(p1, p2), p2);

  data_rewrite_test(R, (sort_pos::min)(p1, p1), p1);
  data_rewrite_test(R, (sort_pos::min)(p1, p2), p1);

  data_rewrite_test(R, (sort_pos::max)(p1, p1), p1);
  data_rewrite_test(R, (sort_pos::max)(p1, p2), p2);

  data_rewrite_test(R, sort_pos::succ(p1), p2);

  data_rewrite_test(R, sort_pos::abs(p4), p4);
}

void nat_rewrite_test() {
  using namespace mcrl2::new_data::sort_nat;

  data_specification specification = parse_data_specification(
    "sort A = Nat;"
  );

  new_data::rewriter R(specification);

  data_expression p0(R(parse_data_expression("0")));
  data_expression p1(R(pos2nat(parse_data_expression("1"))));
  data_expression p2(R(pos2nat(parse_data_expression("2"))));
  data_expression p3(R(pos2nat(parse_data_expression("3"))));
  data_expression p4(R(pos2nat(parse_data_expression("4"))));

  data_rewrite_test(R, plus(p0, p2), p2);
  data_rewrite_test(R, plus(p2, p0), p2);
  data_rewrite_test(R, plus(p1, p2), p3);
  data_rewrite_test(R, plus(p2, p1), p3);

  data_rewrite_test(R, times(p1, p1), p1);
  data_rewrite_test(R, times(p0, p2), p0);
  data_rewrite_test(R, times(p2, p0), p0);
  data_rewrite_test(R, times(p1, p2), p2);

  data_rewrite_test(R, (min)(p1, p1), p1);
  data_rewrite_test(R, (min)(p0, p2), p0);
  data_rewrite_test(R, (min)(p2, p0), p0);
  data_rewrite_test(R, (min)(p1, p2), p1);

  data_rewrite_test(R, (max)(p1, p1), p1);
  data_rewrite_test(R, (max)(p0, p2), p2);
  data_rewrite_test(R, (max)(p2, p0), p2);
  data_rewrite_test(R, (max)(p1, p2), p2);

  data_rewrite_test(R, succ(p0), R(nat2pos(p1)));
  data_rewrite_test(R, succ(p1), R(nat2pos(p2)));

  data_rewrite_test(R, pred(nat2pos(p1)), p0);
  data_rewrite_test(R, pred(nat2pos(p2)), p1);

  data_rewrite_test(R, abs(p1), p1);

  data_rewrite_test(R, div(p1, parse_data_expression("1")), p1);
  data_rewrite_test(R, div(p0, parse_data_expression("2")), p0);
  data_rewrite_test(R, div(p2, parse_data_expression("1")), p2);
  data_rewrite_test(R, div(p4, parse_data_expression("2")), p2);

  data_rewrite_test(R, mod(p1, nat2pos(p1)), p0);
  data_rewrite_test(R, mod(p0, nat2pos(p2)), p0);
  data_rewrite_test(R, mod(p2, nat2pos(p1)), p0);
  data_rewrite_test(R, mod(p4, nat2pos(p3)), p1);

  data_rewrite_test(R, exp(p2, p2), p4);
}

void int_rewrite_test() {
  using namespace mcrl2::new_data::sort_int_;

  data_specification specification = parse_data_specification(
    "sort A = Int;"
  );

  new_data::rewriter R(specification);

  data_expression p0(R(nat2int(parse_data_expression("0"))));
  data_expression p1(R(pos2int(parse_data_expression("1"))));
  data_expression p2(R(pos2int(parse_data_expression("2"))));
  data_expression p3(R(pos2int(parse_data_expression("3"))));
  data_expression p4(R(pos2int(parse_data_expression("4"))));

  data_rewrite_test(R, plus(p0, p2), p2);
  data_rewrite_test(R, plus(p2, p0), p2);
  data_rewrite_test(R, plus(p1, p2), p3);
  data_rewrite_test(R, plus(p2, p1), p3);
  data_rewrite_test(R, plus(negate(p4), p4), p0);
  data_rewrite_test(R, minus(p4, p4), p0);

  data_rewrite_test(R, times(p1, p1), p1);
  data_rewrite_test(R, times(p0, p2), p0);
  data_rewrite_test(R, times(p2, p0), p0);
  data_rewrite_test(R, times(p1, p2), p2);

  data_rewrite_test(R, (min)(p1, p1), p1);
  data_rewrite_test(R, (min)(p0, p2), p0);
  data_rewrite_test(R, (min)(p2, p0), p0);
  data_rewrite_test(R, (min)(p1, p2), p1);

  data_rewrite_test(R, (max)(p1, p1), p1);
  data_rewrite_test(R, (max)(p0, p2), p2);
  data_rewrite_test(R, (max)(p2, p0), p2);
  data_rewrite_test(R, (max)(p1, p2), p2);

  data_rewrite_test(R, succ(p0), p1);
  data_rewrite_test(R, succ(p1), p2);

  data_rewrite_test(R, pred(p1), p0);
  data_rewrite_test(R, pred(p2), p1);

  data_rewrite_test(R, nat2int(abs(p1)), p1);

  data_rewrite_test(R, div(p1, int2pos(p1)), p1);
  data_rewrite_test(R, div(p0, int2pos(p2)), p0);
  data_rewrite_test(R, div(p2, int2pos(p1)), p2);
  data_rewrite_test(R, div(p4, int2pos(p2)), p2);

  data_rewrite_test(R, mod(p1, int2pos(p1)), R(int2nat(p0)));
  data_rewrite_test(R, mod(p0, int2pos(p2)), R(int2nat(p0)));
  data_rewrite_test(R, mod(p2, int2pos(p1)), R(int2nat(p0)));
  data_rewrite_test(R, mod(p4, int2pos(p3)), R(int2nat(p1)));

  data_rewrite_test(R, exp(p2, int2nat(p2)), p4);
}

void real_rewrite_test() {
  using namespace mcrl2::new_data::sort_real_;

  data_specification specification = parse_data_specification(
    "sort A = Real;"
  );

  new_data::rewriter R(specification);

  data_expression p0(R(nat2real(parse_data_expression("0"))));
  data_expression p1(R(pos2real(parse_data_expression("1"))));
  data_expression p2(R(pos2real(parse_data_expression("2"))));
  data_expression p3(R(pos2real(parse_data_expression("3"))));
  data_expression p4(R(pos2real(parse_data_expression("4"))));

  data_rewrite_test(R, plus(p0, p2), p2);
  data_rewrite_test(R, plus(p2, p0), p2);
  data_rewrite_test(R, plus(p1, p2), p3);
  data_rewrite_test(R, plus(p2, p1), p3);
  data_rewrite_test(R, plus(negate(p4), p4), p0);
  data_rewrite_test(R, minus(p4, p4), p0);

  data_rewrite_test(R, times(p1, p1), p1);
  data_rewrite_test(R, times(p0, p2), p0);
  data_rewrite_test(R, times(p2, p0), p0);
  data_rewrite_test(R, times(p1, p2), p2);

  data_rewrite_test(R, (min)(p1, p1), p1);
  data_rewrite_test(R, (min)(p0, p2), p0);
  data_rewrite_test(R, (min)(p2, p0), p0);
  data_rewrite_test(R, (min)(p1, p2), p1);

  data_rewrite_test(R, (max)(p1, p1), p1);
  data_rewrite_test(R, (max)(p0, p2), p2);
  data_rewrite_test(R, (max)(p2, p0), p2);
  data_rewrite_test(R, (max)(p1, p2), p2);

  data_rewrite_test(R, succ(p0), p1);
  data_rewrite_test(R, succ(p1), p2);

  data_rewrite_test(R, pred(p1), p0);
  data_rewrite_test(R, pred(p2), p1);

  data_rewrite_test(R, abs(p1), p1);

  data_rewrite_test(R, divides(p1, p1), p1);
  data_rewrite_test(R, divides(p0, p2), p0);
  data_rewrite_test(R, divides(p2, p1), p2);
  data_rewrite_test(R, divides(p4, p2), p2);

  data_rewrite_test(R, exp(p2, real2int(p2)), p4);

  data_rewrite_test(R, floor(parse_data_expression("2.9")), p2);

  data_rewrite_test(R, ceil(parse_data_expression("1.2")), p2);

  data_rewrite_test(R, round(parse_data_expression("1.6")), p2);
  data_rewrite_test(R, round(parse_data_expression("2.4")), p2);
}

void list_rewrite_test() {
  using namespace mcrl2::new_data::sort_bool_;
  using namespace mcrl2::new_data::sort_list;

  data_specification specification = parse_data_specification(
    "sort A = List(Bool);"
  );

  new_data::rewriter R(specification);

  sort_expression list_bool(list(bool_()));
  data_expression empty(nil(bool_()));
  data_expression head_true(cons_(bool_(), true_(), empty));

  data_rewrite_test(R, in(bool_(), true_(), head_true), true_());
  data_rewrite_test(R, in(bool_(), false_(), head_true), false_());
  data_rewrite_test(R, count(bool_(), head_true), new_data::function_symbol("1", sort_nat::nat()));
  data_rewrite_test(R, in(bool_(), false_(), snoc(bool_(), head_true, true_())), false_());
  data_rewrite_test(R, concat(bool_(), head_true, head_true), R(cons_(bool_(), true_(), head_true)));
  data_rewrite_test(R, element_at(bool_(), head_true, parse_data_expression("0")), true_());
  data_rewrite_test(R, head(bool_(), head_true), true_());
  data_rewrite_test(R, rhead(bool_(), head_true), true_());
  data_rewrite_test(R, rtail(bool_(), head_true), R(empty));
  data_rewrite_test(R, tail(bool_(), head_true), R(empty));
}

void set_rewrite_test() {
}

void bag_rewrite_test() {
}

void structured_sort_rewrite_test() {
  using namespace sort_bool_;
  using namespace sort_nat;

  data_specification specification = parse_data_specification(
    ""
  );

  atermpp::vector< structured_sort_constructor_argument > arguments;

  arguments.push_back(structured_sort_constructor_argument("a0", bool_()));
  arguments.push_back(structured_sort_constructor_argument(bool_()));
  arguments.push_back(structured_sort_constructor_argument("n0", sort_nat::nat()));
  arguments.push_back(structured_sort_constructor_argument("n1", sort_nat::nat()));

  atermpp::vector< structured_sort_constructor > constructors;
  // without arguments or recogniser
  //  c0
  constructors.push_back(structured_sort_constructor("c0"));
  // without arguments, with recogniser
  //  c1?is_one
  constructors.push_back(structured_sort_constructor("c1", std::string("is_one")));
  // with arguments, without recogniser
  //  a(a0 : A)
  constructors.push_back(structured_sort_constructor("a",
     boost::make_iterator_range(arguments.begin(), arguments.begin() + 1)));
  // two arguments, with recogniser
  //  b(B)?is_b
  constructors.push_back(structured_sort_constructor("b",
     boost::make_iterator_range(arguments.begin() + 1, arguments.begin() + 2), "is_b"));
  //  c(n0 : Nat, n1 : Nat)?is_c
  constructors.push_back(structured_sort_constructor("c",
     boost::make_iterator_range(arguments.begin() + 2, arguments.end()), "is_c"));

  new_data::structured_sort ls(boost::make_iterator_range(constructors));

  specification.add_sort(ls);

  new_data::rewriter R(specification);

  data_expression c0(constructors[0].constructor_function(ls));
  data_expression c1(constructors[1].constructor_function(ls));
  data_expression a(application(constructors[2].constructor_function(ls), true_()));
  data_expression b(application(constructors[3].constructor_function(ls), false_()));
  data_expression n0(pos2nat(parse_data_expression("0")));
  data_expression n1(pos2nat(parse_data_expression("1")));
  data_expression c(application(constructors[4].constructor_function(ls), n0, n1));

  // recogniser tests
  data_rewrite_test(R, application(constructors[1].recogniser_function(ls), c0), false_());
  data_rewrite_test(R, application(constructors[3].recogniser_function(ls), c0), false_());
  data_rewrite_test(R, application(constructors[4].recogniser_function(ls), c0), false_());
  data_rewrite_test(R, application(constructors[1].recogniser_function(ls), c1), true_());
  data_rewrite_test(R, application(constructors[3].recogniser_function(ls), c1), false_());
  data_rewrite_test(R, application(constructors[4].recogniser_function(ls), c1), false_());
  data_rewrite_test(R, application(constructors[1].recogniser_function(ls), a),  false_());
  data_rewrite_test(R, application(constructors[3].recogniser_function(ls), a),  false_());
  data_rewrite_test(R, application(constructors[4].recogniser_function(ls), a),  false_());
  data_rewrite_test(R, application(constructors[1].recogniser_function(ls), b),  false_());
  data_rewrite_test(R, application(constructors[3].recogniser_function(ls), b),  true_());
  data_rewrite_test(R, application(constructors[4].recogniser_function(ls), b),  false_());
  data_rewrite_test(R, application(constructors[1].recogniser_function(ls), c),  false_());
  data_rewrite_test(R, application(constructors[3].recogniser_function(ls), c),  false_());
  data_rewrite_test(R, application(constructors[4].recogniser_function(ls), c),  true_());

  // projection tests
  data_rewrite_test(R, application(constructors[2].projection_functions(ls)[0], a),  true_());
  data_rewrite_test(R, application(constructors[4].projection_functions(ls)[0], c),  R(n0));
  data_rewrite_test(R, application(constructors[4].projection_functions(ls)[1], c),  R(n1));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

//  std::string expr1("struct ");
//  std::string expr1("exists b: Bool, c: Bool. if(b, c, b)");
//  std::string expr1("forall b: Bool, c: Bool. if(b, c, b)");

  bool_rewrite_test();
  pos_rewrite_test();
  nat_rewrite_test();
  int_rewrite_test();
  real_rewrite_test();
  list_rewrite_test();
  set_rewrite_test();
  bag_rewrite_test();
  structured_sort_rewrite_test();

  return 0;
}
