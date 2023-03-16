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
#include "mcrl2/smt/translate_specification.h"
#include "mcrl2/smt/solver.h"

using namespace mcrl2;

int main(int /*argc*/, char** /*argv*/)
{
  // note that the newline characters are significant
  data::data_specification data_spec = data::parse_data_specification(
                                   "sort                                                              \n"
                                   "  Bit   = struct b0 | b1;                                         \n"
                                   "                                                                  \n"
                                   "map                                                               \n"
                                   "  invert: Bit -> Bit;                                             \n"
                                   "                                                                  \n"
                                   "eqn                                                               \n"
                                   "  invert(b1)= b0;                                                 \n"
                                   "  invert(b0)= b1;                                                 \n"
                                   "                                                                  \n"
                                   "map                                                               \n"
                                   "  insertB: Bool # Nat # List(Bool) -> List(Bool);                 \n"
                                   "var                                                               \n"
                                   "  d,d': Bool; i: Nat; q: List(Bool);                              \n"
                                   "eqn                                                               \n"
                                   "  i == 0 -> insertB(d,i,q)     = d  |> tail(q);                   \n"
                                   "  i > 0  -> insertB(d,i,d'|>q) = d' |> insertB(d,Int2Nat(i-1),q); \n"
                                   "                                                                  \n"
                                 );
  smt::native_translations ntm = smt::initialise_native_translation(data_spec);

  std::ostringstream out;
  std::unordered_map<data::data_expression, std::string> cache;
  smt::translate_data_specification(data_spec, out, cache, ntm);
  std::cout << out.str() << std::endl;

  smt::smt_solver solv(data_spec);

  smt::answer result;
  data::variable vp1("p1", data::sort_pos::pos());
  data::variable vp2("p2", data::sort_pos::pos());
  data::variable_list pos_vars({vp1, vp2});
  result = solv.solve(pos_vars, data::parse_data_expression("(p1 == p2 + 2) && (p1 == p2 * 2)", pos_vars, data_spec));
  std::cout << "result " << result << std::endl;

  data::variable vb1 = data::parse_variable("bit1: Bit", data_spec);
  data::variable vb2 = data::parse_variable("bit2: Bit", data_spec);
  data::variable_list bit_vars({vb1, vb2});
  result = solv.solve(data::variable_list(), data::parse_data_expression("forall bit1: Bit. invert(invert(bit1)) == bit1", data::variable_list(), data_spec));
  std::cout << "result " << result << std::endl;

  return 0;
}
