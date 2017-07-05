// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normalize_sorts_test.cpp
/// \brief Test for normalizing sorts.

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/parse.h"
#include <algorithm>
#include <boost/test/minimal.hpp>
#include <iterator>
#include <set>
#include <vector>

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

int test_main(int argc, char* argv[])
{
  test_normalize_sorts();

  return 0;
}
