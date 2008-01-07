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

using namespace atermpp;
using namespace lps;

int test_main(int argc, char** argv)
{
  MCRL2_CORE_LIBRARY_INIT(argv) 

  sort_expression A("A");
  sort_expression_list lA = make_list(A);
  sort_expression B("B");
  sort_expression_list lB = make_list(B);
  sort_expression C("C");
  sort_expression_list lC = make_list(C);
  sort_expression D("D");
  sort_expression AB = arrow(lA, B);
  sort_expression_list lAB = make_list(A,B);
  sort_expression BC = arrow(lB, C);
  sort_expression CD = arrow(lC, D);

  sort_expression s;
  sort_expression_list domain;
  sort_expression range;
  sort_expression_list src;
  sort_expression tgt;
  
  s      = arrow(lA, BC);   // A->(B->C)
  domain = domain_sorts(s); // [A,B]
  range  = result_sort(s);   // C
  src    = source(s);       // [A]
  tgt    = target(s);      // B->C
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(range == C);
  BOOST_CHECK(src.size() == 1);
  BOOST_CHECK(std::find(src.begin(), src.end(), A) != src.end());
  BOOST_CHECK(tgt == BC);

  s      = arrow(lAB, C);   // (AxB)->C
  domain = domain_sorts(s); // [A,B]
  range  = result_sort(s);   // C
  src    = source(s);       // [A,B]
  tgt    = target(s);      // C
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(range == C);
  BOOST_CHECK(src.size() == 2);
  BOOST_CHECK(std::find(src.begin(), src.end(), A) != src.end());
  BOOST_CHECK(std::find(src.begin(), src.end(), B) != src.end());
  BOOST_CHECK(tgt == C);

  s      = arrow(lAB, CD);  // (AxB)->(C->D)
  domain = domain_sorts(s); // [A,B,C]
  range  = result_sort(s);   // D
  src    = source(s);       // [A,B]
  tgt    = target(s);      // C->D
  BOOST_CHECK(domain.size() == 3);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), C) != domain.end());
  BOOST_CHECK(range == D);
  BOOST_CHECK(src.size() == 2);
  BOOST_CHECK(std::find(src.begin(), src.end(), A) != src.end());
  BOOST_CHECK(std::find(src.begin(), src.end(), B) != src.end());
  BOOST_CHECK(tgt == CD);

  return 0;
}
