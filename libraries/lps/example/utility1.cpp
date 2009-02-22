// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utility.cpp
/// \brief Add your file description here.

//[ data_utility_example
#include <iostream>
#include <string>
#include <cassert>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/find.h"
#include "mcrl2/new_data/data.h"
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/utility.h"

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::new_data::data_expr;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  variable d("d:D");
  variable d00("d00:D");
  variable d02("d02:D");
  data_expression e = and_(equal_to(d, d00), not_equal_to(d02, d00));

  // generate two variables that do not appear in the expression e
  variable v = fresh_variable(e, sort_expression("D"), "d");
  assert(v == variable("d01:D"));
  v = fresh_variable(make_list(e, v), sort_expression("D"), "d");
  assert(v == variable("d03:D"));

  // do the same using a fresh_variable_generator
  fresh_variable_generator generator(e, sort_expression("D"), "d");
  v = generator();
  assert(v == variable("d01:D"));
  v = generator();
  assert(v == variable("d03:D"));

  // find all identifiers appearing in e
  std::set<identifier_string> ids = core::find_identifiers(e);
  assert(ids.size() == 8);
  assert(ids.find(identifier_string("d"))    != ids.end());
  assert(ids.find(identifier_string("d00"))  != ids.end());
  assert(ids.find(identifier_string("d02"))  != ids.end());
  assert(ids.find(identifier_string("D"))    != ids.end());
  assert(ids.find(identifier_string("Bool")) != ids.end());
  assert(ids.find(identifier_string("=="))   != ids.end());
  assert(ids.find(identifier_string("!="))   != ids.end());
  assert(ids.find(identifier_string("&&"))   != ids.end());

  return 0;
}
//]
