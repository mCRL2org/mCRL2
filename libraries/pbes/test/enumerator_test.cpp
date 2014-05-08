// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_enumerator()
{
  typedef pbes_system::simplify_data_rewriter<data::rewriter> pbes_rewriter;

  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_rewriter R(datar);

  data::variable_list v;
  pbes_expression phi;
  pbes_expression stop;

  enumerator_algorithm<pbes_rewriter> E(v, phi, stop, R, data_spec);
}

int test_main(int argc, char** argv)
{
  test_enumerator();

  return 0;
}
