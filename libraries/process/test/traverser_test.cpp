// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Traverser tests.

#include <iostream>
#include <string>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/traverser.h"

using namespace mcrl2;
using namespace mcrl2::process;

class my_traverser: public process::traverser<my_traverser>
{
public:
  typedef process::traverser<my_traverser> super;

  using super::enter;
  using super::leave;

#if BOOST_MSVC
  template <typename Container >
  void operator()(Container const& x)
  {
    super::operator()(x);
  }
#endif
};
  
void test_traverser()
{
  my_traverser t;

  data::data_expression d;
  t(d);

  data::data_expression_list dl;
  t(dl);

  data::variable v;
  t(v);
  
  data::data_equation eq;
  t(eq);

  process::rename r;
  t(r);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_traverser();

  return EXIT_SUCCESS;
}
