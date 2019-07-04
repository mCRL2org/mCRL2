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
                                   "sort                      \n"
                                   "  Bit   = struct b0 | b1; \n"
                                   "                          \n"
                                   "map                       \n"
                                   "  invert: Bit -> Bit;     \n"
                                   "                          \n"
                                   "eqn                       \n"
                                   "  invert(b1)= b0;         \n"
                                   "  invert(b0)= b1;         \n"
                                 );
  smt::native_translations ntm = smt::initialise_native_translation(data_spec);

  std::ostringstream out;
  smt::translate_data_specification(data_spec, out, ntm);
  std::cout << out.str() << std::endl;

  smt::smt_solver solv(data_spec);
  data::variable vp1("p1", data::sort_pos::pos());
  data::variable vp2("p2", data::sort_pos::pos());
  bool result = solv.solve(data::variable_list({vp1, vp2}), data::sort_bool::and_(data::equal_to(vp1, data::sort_pos::plus(vp2, data::sort_pos::pos(2))),
                                                                                  data::equal_to(vp1, data::sort_pos::times(vp2, data::sort_pos::pos(2)))));
  std::cout << "result " << std::boolalpha << result << std::endl;

  return 0;
}
