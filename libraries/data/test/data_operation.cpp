// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_operation.cpp
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

  identifier_string f_name("f");
  sort_identifier T("T");

  data_operation f(f_name, T);
  BOOST_CHECK(f.name() == f_name);
  BOOST_CHECK(f.sort() == T);

  data_operation f1(gsMakeOpId(gsString2ATermAppl("f"), gsMakeSortId(gsString2ATermAppl("T"))));
  BOOST_CHECK(f1.name() == f_name);
  BOOST_CHECK(f1.sort() == T);

  return 0;
}

