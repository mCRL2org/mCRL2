#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "atermpp/make_list.h"
#include "lps/function.h"

using namespace atermpp;
using namespace lps;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  sort A("A");
  sort_list lA = make_list(A);
  sort B("B");
  sort_list lB = make_list(B);
  sort C("C");
  sort_list lC = make_list(C);
  sort D("D");
  sort AB = arrow(lA, B);
  sort_list lAB = make_list(A,B);
  sort BC = arrow(lB, C);
  sort CD = arrow(lC, D);

  sort s;
  sort_list domain;
  sort range;
  
  s      = arrow(lA, BC);     // A->(B->C)
  domain = s.domain_sorts(); // [A,B]
  range  = s.range_sort();        // C
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(range == C);

  s      = arrow(lAB, C);     // (AxB)->C
  domain = s.domain_sorts(); // [A,B]
  range  = s.range_sort();        // C
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(range == C);

  s      = arrow(lAB, CD);    // (AxB)->(C->D)
  domain = s.domain_sorts(); // [A,B,C]
  range  = s.range_sort();        // D
  BOOST_CHECK(domain.size() == 3);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), C) != domain.end());
  BOOST_CHECK(range == D);

  return 0;
}
