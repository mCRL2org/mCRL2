// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file is_quoted.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/utility.h"

using namespace std;
using namespace atermpp;

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  // unquoted function symbol s == "f"
  function_symbol s("\"f\"", 1, false);
  aterm_appl f(s, aterm("x"));

  // convert to string and back
  aterm_appl g = make_term(f.to_string());

  BOOST_CHECK(g.is_quoted());
  BOOST_CHECK(g.function() != s);
  function_symbol s1("f", 1, true);
  BOOST_CHECK(g.function() == s1);

  return 0;
}
