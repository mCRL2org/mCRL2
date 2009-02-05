// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_application.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/data.h"
#include "mcrl2/data/sort.h"
#include "mcrl2/atermpp/make_list.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  sort_identifier A("A");
  sort_identifier B("B");
  sort_arrow AAAB(make_list(A,A,A), B);
  sort_expression sAAAB(AAAB);
  data_operation f(identifier_string("f"), sAAAB);
  data_expression ef(f);
  data_variable x("x", A);
  data_variable y("y", B);
  data_expression ex(x);
  data_expression ey(y);

  /* This is an extremely ugly workaround for the fact that a simple check
   * e.arguments() == xxx does not work.
   */
  data_expression_list xxx(make_list(ex,ex,ex));
  data_application e(ef, xxx);
  BOOST_CHECK(e.head() == f);
  BOOST_CHECK(e.arguments().size() == xxx.size());
  data_expression_list::iterator i = e.arguments().begin();
  data_expression_list::iterator j = xxx.begin();
  while(i != e.arguments().end() && j != xxx.end())
  {
    BOOST_CHECK(*i == *j);
    ++i;
    ++j;
  }

  /* This illustrates that the constructor for a data application does not
   * check type correctness of the resulting term!
   */
  //data_application e1(f, make_list(ex,ex));
  //data_application e2(f, make_list(ex,ex,ey));

  return 0;
}
