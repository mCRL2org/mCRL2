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
   
int test_main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

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

  return 0;
}
