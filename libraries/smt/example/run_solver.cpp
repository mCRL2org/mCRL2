// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file run_solver.cpp

#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/smt/cvc4.h"
#include "mcrl2/smt/solver.h"

#include <vector>

using namespace mcrl2;

int main(int /*argc*/, char** /*argv*/)
{
  // note that the newline characters are significant
  data::data_specification data_spec;
  smt::smt4_data_specification* smt_data_spec = new smt::smt4_data_specification(data_spec);
  smt::solver solver(smt_data_spec);
  data::variable x("x", data::sort_pos::pos());

  smt::smt_problem problem1;
  problem1.add_variable(x);
  problem1.add_assertion(data::parse_data_expression("2 + x == 5", data::variable_vector({x})));
  std::cout << "Result of checking 'exists x:Pos. 2 + x == 5': " << std::boolalpha << solver.solve(problem1) << std::endl;

  smt::smt_problem problem2;
  problem2.add_variable(x);
  problem2.add_assertion(data::parse_data_expression("5 + x == 2", data::variable_vector({x})));
  std::cout << "Result of checking 'exists x:Pos. 5 + x == 2': " << std::boolalpha << solver.solve(problem2) << std::endl;

  delete smt_data_spec;

  return 0;
}
