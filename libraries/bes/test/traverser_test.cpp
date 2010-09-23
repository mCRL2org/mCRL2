// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Test for traversers.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
//#include "mcrl2/bes/find.h"
#include "mcrl2/bes/bes_parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/bes/traverser.h"

using namespace mcrl2;
using namespace mcrl2::bes;

class custom_traverser: public traverser<custom_traverser>
{
  public:
    typedef traverser<custom_traverser> super;
      
    using super::enter;
    using super::leave;

#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template <typename Container>
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif

};

void test_custom_traverser()
{
  custom_traverser t;

  boolean_variable v;
  t(v);

  true_ T;
  t(T);

  boolean_expression e;
  t(e);

  boolean_equation eq;
  t(eq);

  boolean_equation_system<> eqn;
  t(eqn);

  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_custom_traverser();

  return EXIT_SUCCESS;
}
