// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file control_flow_test.cpp
/// \brief Tests for control flow algorithm.

#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/control_flow.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_control_flow()
{
  std::string text =
    "pbes nu X(n: Nat)  = X(0) || X(n) || X(2);   \n"
    "     nu Y(m: Nat)  = Y(m) || X(1) || X(m);   \n"
    "     nu Z(p: Nat) = (forall b:Bool. true) && (true => (X(p) || X(1))) && val(true) && Y(p); \n"
    "init X(0);                                  \n"
    ;
  pbes<> p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::pbes_control_flow_algorithm algorithm;
  algorithm.run(p);
  algorithm.print_graph();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_control_flow();

  return 0;
}
