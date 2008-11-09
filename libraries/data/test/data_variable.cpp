// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_variable.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/data.h"
#include "mcrl2/data/sort.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/aterm_ext.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  identifier_string x_name("x");
  sort_identifier T("T");

  data_variable x("x:T");
  BOOST_CHECK(x.name() == x_name);
  BOOST_CHECK(x.sort() == T);

  data_variable y("y:S");
  BOOST_CHECK(y.name() != x_name);
  BOOST_CHECK(y.sort() != T);

  data_variable x1(x_name, T);
  BOOST_CHECK(x1.name() == x_name);
  BOOST_CHECK(x1.sort() == T);

  data_variable x2("x", T);
  BOOST_CHECK(x2.name() == x_name);
  BOOST_CHECK(x2.sort() == T);

  data_variable x3(gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortId(gsString2ATermAppl("T"))));
  BOOST_CHECK(x3.name() == x_name);
  BOOST_CHECK(x3.sort() == T);

  return 0;
}

