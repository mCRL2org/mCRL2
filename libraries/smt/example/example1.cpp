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
#include "mcrl2/data/parse.h"
#include "mcrl2/smt/solver.h"

using namespace mcrl2;

int main(int /*argc*/, char** /*argv*/)
{
  // note that the newline characters are significant
  data::data_specification data_spec = data::parse_data_specification(
                                   "sort                              \n"
                                   "  Bit   = struct b0(r:Real) | b1; \n"
                                   "                                  \n"
                                   "map                               \n"
                                   "  invert: Bit -> Bit;             \n"
                                   "                                  \n"
                                   "var                               \n"
                                   "  r1: Real;                       \n"
                                   "                                  \n"
                                   "eqn                               \n"
                                   "  invert(b1)= b0(1);              \n"
                                   "  invert(b0(r1))= b1;             \n"
                                 );

  smt::smt_solver solv(data_spec);
  data::variable vb1 = data::parse_variable("bit_1: Bit", data_spec);
  data::variable vb2 = data::parse_variable("bit_2: Bit", data_spec);
  data::variable_list bit_vars({vb1});
  // This may not terminate
  smt::answer result = solv.solve(bit_vars, data::parse_data_expression("invert(invert(bit_1)) != bit_1", bit_vars, data_spec), std::chrono::seconds(5));
  std::cout << "result " << result << std::endl;

  return 0;
}
