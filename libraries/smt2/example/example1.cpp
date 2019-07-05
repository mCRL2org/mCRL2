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
#include "mcrl2/smt2/translate_expression.h"
#include "mcrl2/smt2/translate_specification.h"
#include "mcrl2/smt2/native_translation.h"
#include "mcrl2/smt2/solver.h"

#include <vector>
#include <sstream>

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
                                   "  invert2: Bit -> Bit;             \n"
                                   "                                  \n"
                                   "var                               \n"
                                   "  r1: Real;                       \n"
                                   "                                  \n"
                                   "eqn                               \n"
                                   "  invert(b1)= b0(1);              \n"
                                   "  invert(b0(r1))= b1;             \n"
                                   "  invert2(b0(r1))= b0(-r1);             \n"
                                 );
  smt::native_translations ntm = smt::initialise_native_translation(data_spec);

  std::ostringstream out;
  smt::translate_data_specification(data_spec, out, ntm);
  std::cout << out.str() << std::endl;

  smt::smt_solver solv(data_spec);
  //
  bool result;
  // data::variable vp1("p1", data::sort_pos::pos());
  // data::variable vp2("p2", data::sort_pos::pos());
  // data::variable_list pos_vars({vp1, vp2});
  // result = solv.solve(pos_vars, data::parse_data_expression("(p1 == p2 + 2) && (p1 == p2 * 2)", pos_vars, data_spec));
  // std::cout << "result " << std::boolalpha << result << std::endl;
  //
  data::variable vb1 = data::parse_variable("bit_1: Bit", data_spec);
  data::variable vb2 = data::parse_variable("bit_2: Bit", data_spec);
  data::variable_list bit_vars({vb1});
  result = solv.solve(bit_vars, data::parse_data_expression("invert(invert(bit_1)) != bit_1", bit_vars, data_spec));
  std::cout << "result " << std::boolalpha << result << std::endl;

  return 0;
}
