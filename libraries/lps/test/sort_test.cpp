// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/mcrl22lps.h>
#include "mcrl2/utilities/aterm_ext.h"

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;

const std::string SPECIFICATION =
  "sort A;                     \n"
  "sort B;                     \n"
  "sort C;                     \n"
  "sort D;                     \n"
  "sort S1 = A # B -> C;       \n"
  "sort S2 = (A -> B) -> C;    \n"
  "sort S3 = A -> (B -> C);    \n"
  "sort S4 = A # (A -> B) -> C;\n"
  "sort S5 = A # A -> B -> C;  \n"
  "                            \n"
  "cons s1: S1;                \n"
  "cons s2: S2;                \n"
  "cons s3: S3;                \n"
  "cons s4: S4;                \n"
  "cons s5: S5;                \n" 
  "                            \n"
  "act z;                      \n"
  "proc X(i: Nat) = z.X(i);    \n"
  "init X(2);                  \n"
;  

const std::string ABP_SPECIFICATION =
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n";

void test_sort_enumeration()
{
  using namespace data_expr;

  specification spec = mcrl22lps(ABP_SPECIFICATION);
  data_specification data = spec.data();

  std::set<sort_expression> sorts;
  for (action_label_list::iterator i = spec.action_labels().begin(); i != spec.action_labels().end(); ++i)
  {
    sort_expression_list s = i->sorts();
    sorts.insert(s.begin(), s.end());
  }
  for (std::set<sort_expression>::iterator i = sorts.begin(); i != sorts.end(); ++i)
  {
    std::cout << "<sort>" << pp(*i) << " " << pp(data.constructors(*i)) << " " << pp(enumerate_constructors(data.constructors(*i), *i)) << std::endl;
  }

  enumerator e(spec.data());
  sort_expression s1 = sort_expr::bool_();
  atermpp::vector<data_expression> v = e.enumerate_finite_sort(s1);

  data_variable d1(core::identifier_string("d1"), sort_expr::bool_());
  data_variable d2(core::identifier_string("d2"), sort_expr::bool_());
  data_variable d3(core::identifier_string("d3"), sort_expr::bool_());
  std::vector<data_variable> vars;
  vars.push_back(d1);
  vars.push_back(d2);
  data_expression t = and_(data_expr::equal_to(d1, d2), data_expr::not_equal_to(d1,d3));
  atermpp::set<data_expression> w = e.enumerate_expression_values(t, vars.begin(), vars.end());
  std::cout << "<variables>" << pp(data_expression_list(w.begin(), w.end())) << std::endl;
  //BOOST_CHECK(false);
}

void test_sort_equality()
{
  specification spec = mcrl22lps(SPECIFICATION);

  data_operation s1 = find_constructor(spec.data(), "s1");
  data_operation s2 = find_constructor(spec.data(), "s2");
  data_operation s3 = find_constructor(spec.data(), "s3");
  data_operation s4 = find_constructor(spec.data(), "s4");
  data_operation s5 = find_constructor(spec.data(), "s5");
  
  BOOST_CHECK(s1.sort() != sort_expression());
  BOOST_CHECK(s2.sort() != sort_expression());
  BOOST_CHECK(s3.sort() != sort_expression());
  BOOST_CHECK(s4.sort() != sort_expression());
  BOOST_CHECK(s5.sort() != sort_expression());

  BOOST_CHECK(s1.sort() != s2.sort());
  BOOST_CHECK(s2.sort() != s3.sort());
  BOOST_CHECK(s4.sort() != s5.sort()); 
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  //test_sort_equality();
  test_sort_enumeration();

  return 0;
}
