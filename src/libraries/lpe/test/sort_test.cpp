#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "lpe/function.h"

using namespace atermpp;
using namespace lpe;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  sort A("A");
  sort B("B");
  sort C("C");
  sort D("D");
  sort AB = arrow(A, B);
  sort BC = arrow(B, C);
  sort CD = arrow(C, D);

  sort s;
  sort_list domain;
  sort range;
  
  s      = arrow(A, BC);     // A->(B->C)
  domain = s.domain_sorts(); // [A,B]
  range  = s.range_sort();        // C
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), A) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), B) != domain.end());
  BOOST_CHECK(range == C);

  s      = arrow(AB, C);     // (A->B)->C
  domain = s.domain_sorts(); // [A->B]
  range  = s.range_sort();        // C
  BOOST_CHECK(domain.size() == 1);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), AB) != domain.end());
  BOOST_CHECK(range == C);

  s      = arrow(AB, CD);    // (A->B)->(C->D)
  domain = s.domain_sorts(); // [A->B,C]
  range  = s.range_sort();        // D
  BOOST_CHECK(domain.size() == 2);
  BOOST_CHECK(std::find(domain.begin(), domain.end(), AB) != domain.end());
  BOOST_CHECK(std::find(domain.begin(), domain.end(), C) != domain.end());
  BOOST_CHECK(range == D);

  return 0;
}
