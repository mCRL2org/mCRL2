// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constelm_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE constelm_test
#include <boost/test/included/unit_test.hpp>
#include "mcrl2/data/detail/enumerator_iteration_limit.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

std::string t1 =
  "% simple test, n is constant \n"
  "                             \n"
  "pbes nu X(n: Nat) =          \n"
  "       X(n);                 \n"
  "                             \n"
  "init X(0);                   \n"
  ;
std::string x1 = "binding_variables = X";

std::string t2 =
  "% simple test, n is NOT constant \n"
  "                                 \n"
  "pbes nu X(n: Nat) =              \n"
  "       X(n + 1);                 \n"
  "                                 \n"
  "init X(0);                       \n"
  ;
std::string x2 = "binding_variables = X(n: Nat)";

std::string t3 =
  "% multiple parameters: n1 is constant, n2 is not \n"
  "                                                 \n"
  "pbes nu X(n1,n2: Nat) =                          \n"
  "       X(n1, n2 + 1);                            \n"
  "                                                 \n"
  "init X(0, 1);                                    \n"
  ;
std::string x3 = "binding_variables = X(n2: Nat)";

std::string t4 =
  "% conditions: n is not constant, even if conditions are enabled\n"
  "                                                               \n"
  "pbes nu X(n: Nat) =                                            \n"
  "       val(2 < n) || X(n + 1);                                 \n"
  "                                                               \n"
  "init X(0);                                                     \n"
  ;
std::string x4 = "binding_variables = X(n: Nat)";

std::string t5 =
  "% conditions: n is constant if conditions are enabled \n"
  "                                                      \n"
  "pbes nu X(n: Nat) =                                   \n"
  "       val(2 < n) || X(n + 1);                        \n"
  "                                                      \n"
  "init X(5);                                            \n"
  ;
std::string x5 = "binding_variables = X";

std::string t6 =
  "% reachability: n1 is not constant, equation Y is not reachable and should be removed \n"
  "                                                                                      \n"
  "pbes nu X(n1: Nat) = X(n1+1);                                                         \n"
  "     mu Y(n2: Nat) = Y(n2+1);                                                         \n"
  "                                                                                      \n"
  "init X(5);                                                                            \n"
  ;
std::string x6 = "binding_variables = X(n1: Nat)";

std::string t7 =
  "% multiple edges from one vertex, one edge invalidates the assertion of the other: no constants should be found \n"
  "                                                                                                                \n"
  "pbes                                                                                                            \n"
  "   mu X1(n1,m1:Nat) = X2(n1) || X2(m1);                                                                         \n"
  "   mu X2(n2:Nat)     = X1(n2,n2);                                                                               \n"
  "init                                                                                                            \n"
  "   X1(2,1);                                                                                                     \n"
  ;
std::string x7 = "binding_variables = X1(n1,m1: Nat), X2(n2: Nat)";

std::string t8 =
  "% conditions: parameters b,c and d will always be removed, with conditions on, parameter \n"
  "% n will NOT be removed. changing b,c or d to false or n to a bigger number              \n"
  "% then 5 WILL result in the removal of n                                                 \n"
  "                                                                                         \n"
  "pbes nu X(b,c,d:Bool, n:Nat) =                                                           \n"
  "    val(b) || ( val(c) && ( val(d) && (val(n > 5) || X(b,c,d,n+1))));                    \n"
  "                                                                                         \n"
  "init                                                                                     \n"
  "X(false,true,true,5);                                                                    \n"
  ;
std::string x8 = "binding_variables = X(n: Nat)";

std::string t9 =
  "% conditions: universal quantification which can be solved, n1 and n2 are \n"
  "% constants if conditions are enabled                                     \n"
  "                                                                          \n"
  "pbes mu X(n1,n2:Nat) =                                                    \n"
  "    forall m:Nat. (val(n1>n2) && X(n1+1,n2+1));                           \n"
  "                                                                          \n"
  "init X(1,2);                                                              \n"
  ;
std::string x9 = "binding_variables = X";

std::string t10 =
  "% conditions: existential quantification which can be solved, n1 and n2 \n"
  "% are constants if conditions are enabled                               \n"
  "                                                                        \n"
  "pbes mu X(n1,n2:Nat) =                                                  \n"
  "    exists m:Nat. (val(n1>n2) && X(n1+1,n2+1));                         \n"
  "                                                                        \n"
  "init X(1,2);                                                            \n"
  ;
std::string x10 = "binding_variables = X";

std::string t11 =
  "pbes mu X(n1,n2:Nat) =                                        \n"
  "    forall m:Nat. (val(m>n2) && X(n1+1,n2+1));                \n"
  "                                                              \n"
  "init X(1,2);                                                  \n"
  ;
std::string x11 = "binding_variables = X";

std::string t12 =
  "% example 4.2.1 from \"Tools for PBES\" report                                     \n"
  "% constants without conditions: o4, n2, n4, o4                                     \n"
  "% constants with conditions: everything (equations X1,X2,X3 and X4 are removed)    \n"
  "                                                                                   \n"
  "pbes                                                                               \n"
  "   mu X1(n1,m1,o1,p1:Nat) = (val(n1<m1) || X2(o1,p1)) && X3(n1) && X1(n1,m1,4,p1); \n"
  "   mu X2(n2,m2:Nat)       = X5(n2,m2) || X5(m2,n2);                                \n"
  "   nu X3(n3:Nat)          = val(n3<3) || X1(n3,n3,4,n3+1);                         \n"
  "   nu X4(n4,m4,o4:Nat)    = val(n4 <= m4+o4) || (X3(n4) && X4(n4,m4+1,n4));        \n"
  "   mu X5(n5,m5:Nat)       = val(n5>m5) || X3(n5);                                  \n"
  "                                                                                   \n"
  "init                                                                               \n"
  "   X4(0,0,0);                                                                      \n"
  ;
std::string x12 = "binding_variables = X1(n1,m1,p1: Nat), X2(m2: Nat), X3(n3: Nat), X4(m4: Nat), X5(n5,m5: Nat)";

std::string t13 =
  "pbes nu X =           \n"
  "        Y(true);      \n"
  "      mu Y(b: Bool) = \n"
  "        X;            \n"
  "                      \n"
  "init X;               \n"
  ;
std::string x13 = "binding_variables = X, Y";

std::string t14 =
  "pbes nu X(m:Nat) =           \n"
  "        forall n:Nat . X(n); \n"
  "                             \n"
  "init X(0);                   \n"
  ;
std::string x14 = "binding_variables = X(m: Nat)";

std::string t15 =
  "pbes                 \n"
  "nu X0(n:Nat) = X1;   \n"
  "mu X1 = X0(0) || X1; \n"
  "                     \n"
  "init X0(0);          \n"
  ;
std::string x15 = "binding_variables = X0, X1";

std::string t16 =
  "pbes                      \n"
  " nu X(n:Nat) = Y && X(n); \n"
  " mu Y = Z;                \n"
  " nu Z = Y;                \n"
  "                          \n"
  " init X(0);               \n"
  ;
std::string x16 = "binding_variables = X, Y, Z";

std::string t17 =
  "sort D = struct d1 | d2 | d3 | d4;\n"
  "pbes nu X1 =\n"
  "       X(2);\n"
  "     mu X(s3_X: Pos) =\n"
  "       val(s3_X == 4) || val(s3_X == 2) && X(3) || val(s3_X == 2) && X(4) || val(s3_X == 3) && X(1) || val(s3_X == 4) && X(1);\n"
  "init X1;\n";

std::string x17 = "binding_variables = X1, X(s3_X: Pos)";

std::string t18 =
  "% conditions: quantifications which cannot be solved, n1 and n2 are *not* \n"
  "% constants even if conditions are enabled.                               \n"
  "                                                                          \n"
  "pbes mu X(n1,n2:Nat) =                                                    \n"
  "    forall m1:Nat. exists m2:Nat. (val(m2 > n2 + m1) && X(n1+1,n2+1));    \n"
  "                                                                          \n"
  "init X(1,2);                                                              \n"
  ;

std::string x18 = "binding_variables = X(n1,n2: Nat)";

std::string t19 =
  "pbes nu X(b: Bool) =              \n"
  "       forall b2: Bool. Y(b, b2); \n"
  "     mu Y(b,b1: Bool) =           \n"
  "       val(b == b1) || Y(!b, b1); \n"
  "                                  \n"
  "init X(false);                    \n"
  ;

std::string x19 = "binding_variables = X, Y(b,b1: Bool)";

void test_pbes(const std::string& pbes_spec, const std::string& expected_result, bool compute_conditions, bool remove_equations = true)
{
  typedef simplify_data_rewriter<data::rewriter> my_pbes_rewriter;

  pbes p = txt2pbes(pbes_spec);
  pbes q = p;

  // data rewriter
  data::rewriter datar(q.data());

  // pbes rewriter
  my_pbes_rewriter pbesr(datar);

  // constelm algorithm
  pbes_constelm_algorithm<data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);

  // run the algorithm
  algorithm.run(q, compute_conditions);
  if (remove_equations)
  {
    remove_unreachable_variables(q);
  }
  BOOST_CHECK(q.is_well_typed());
  std::cout << "\n--- q ---\n" << pbes_system::pp(q) << std::endl;

  pbes_system::detail::pbes_property_map info1(q);
  pbes_system::detail::pbes_property_map info2(expected_result);
  std::string diff = info1.compare(info2);
  if (!diff.empty())
  {
    std::cerr << "\n------ FAILED TEST ------" << std::endl;
    std::cout << pbes_spec << std::endl;
    std::cerr << "--- expected result" << std::endl;
    std::cerr << expected_result << std::endl;
    std::cerr << "--- found result" << std::endl;
    std::cerr << info1.to_string() << std::endl;
    std::cerr << "--- differences" << std::endl;
    std::cerr << diff << std::endl;
  }
  BOOST_CHECK(diff.empty());

}

BOOST_AUTO_TEST_CASE(test_constelm)
{
  test_pbes(t1 , x1 , false);
  test_pbes(t2 , x2 , false);
  test_pbes(t3 , x3 , false);
  test_pbes(t4 , x4 , true);
  test_pbes(t5 , x5 , true);
  test_pbes(t6 , x6 , false);
  test_pbes(t7 , x7 , false);
  test_pbes(t8 , x8 , true);
  test_pbes(t9 , x9 , true);
  test_pbes(t10, x10, true);
  test_pbes(t11, x11, true);
  test_pbes(t12, x12, false);
  test_pbes(t13, x13, false);
  test_pbes(t14, x14, false);
  test_pbes(t15, x15, false);
  test_pbes(t16, x16, true); 
  test_pbes(t17, x17, false);
  test_pbes(t19, x19, false);

  data::detail::set_enumerator_iteration_limit(10); // This final test requires 50*50 enumerations and the default limit of 1000 takes too long.
  test_pbes(t18, x18, true);
}
