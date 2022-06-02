// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normalize_sorts_test.cpp
/// \brief Test for normalizing sorts.

#define BOOST_TEST_MODULE normalize_sorts_test
#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/parse.h"

#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using namespace mcrl2::data;

void test_normalize_sorts()
{
  std::string DATASPEC =
    "sort Bit = struct e0 | e1;      \n"
    "     AbsBit = struct arbitrary; \n"
    "                                \n"
    "map  inv: Bit -> Bit;           \n"
    "     h: Bit -> AbsBit;          \n"
    "                                \n"
    "eqn  inv(e0)  =  e1;            \n"
    "     inv(e1)  =  e0;            \n"
    ;

  data_specification dataspec = parse_data_specification(DATASPEC);

  data::function_symbol f;
  f = parse_function_symbol("abseq : AbsBit # AbsBit -> Set(Bool)", DATASPEC);
  dataspec.add_mapping(f);
  f = parse_function_symbol("absinv : AbsBit -> Set(AbsBit)", DATASPEC);
  dataspec.add_mapping(f);

  data_equation_vector equations = dataspec.user_defined_equations();
  data::normalize_sorts(equations, dataspec);
}

// The test below checks whether the calculation of a confluent and terminating rewrite system for types using
// Knuth-Bendix completion is efficient. Aleksi Peltonen showed in the spring of 2018 that Knuth-Bendix completion
// was exponential, causing the example below not to terminate within reasonable time. 
void test_apply_knuth_bendix_completion_on_sorts()
{
  std::string DATASPEC =
    "sort A_t = Nat; B_t = Nat; C_t = Nat; D_t = Nat; E_t = Nat; F_t = Nat; G_t = Nat; H_t = Nat; I_t = Nat; J_t=Nat; K_t=Nat; \n"
    "     L_t = Nat; M_t = Nat; N_t = Nat; O_t = Nat;\n"
    "     S_t = struct s( A:A_t, B:B_t, C:C_t, D:D_t, E:E_t, F:F_t, G:G_t, H:H_t, I:I_t, J:J_t, K:K_t, L:L_t, M:M_t, N:N_t, O:O_t); \n";

  data_specification dataspec = parse_data_specification(DATASPEC);

  data_equation_vector equations = dataspec.user_defined_equations();
  data::normalize_sorts(equations, dataspec);
}

// The specification below led to an infinite loop in January 2019 when applying Knuth-Bendix completion. 
// The reason was a missing normalisation of the rhs of type equations, leading to a loop. front_doorstate was
// mapped to rear_doorstate and vice versa. 
// The specification is not well typed, as there are constants with different types. This test is to 
// check whether there is no loop, and typechecking never finishes. 
void test_loop_free_knuth_bendix_completion()
{
  std::string DATASPEC =
    "sort front_doorstate = struct open | closed; \n"
    "     rear_doorstate  = struct open | closed;\n"    
    ;

  try 
  {
    data_specification dataspec = parse_data_specification(DATASPEC);
  }
  catch (mcrl2::runtime_error& e)
  {
    // This is ok. A runtime exception is expected. 
    return;
  }
  BOOST_CHECK(false); // No exception is not ok. 
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_normalize_sorts();
  test_apply_knuth_bendix_completion_on_sorts();
  test_loop_free_knuth_bendix_completion();
}
