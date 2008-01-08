// Author(s): Wieger Wesselink
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
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"

using namespace atermpp;
using namespace lps;
using namespace lps::data_expr;

int main(int argc, char*[] argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  data_variable d("d:D");
  data_variable d00("d00:D");
  data_variable d02("d02:D");
  data_expression e = and_(equal_to(d, d00), not_equal_to(d02, d00));

  // generate two variables that do not appear in the expression e
  data_variable v = fresh_variable(e, sort("D"), "d");
  assert(v == data_variable("d01:D"));
  v = fresh_variable(make_list(e, v), sort("D"), "d");
  assert(v == data_variable("d03:D"));

  // do the same using a fresh_variable_generator
  fresh_variable_generator generator(e, sort("D"), "d");
  v = generator();
  assert(v == data_variable("d01:D"));
  v = generator();
  assert(v == data_variable("d03:D"));

  // find all identifiers appearing in e
  std::set<identifier_string> ids = identifiers(e);
  assert(ids.size() == 8);
  assert(ids.find(identifier_string("d"))    != ids.end());
  assert(ids.find(identifier_string("d00"))   != ids.end());
  assert(ids.find(identifier_string("d02"))  != ids.end());
  assert(ids.find(identifier_string("D"))    != ids.end());
  assert(ids.find(identifier_string("Bool")) != ids.end());
  assert(ids.find(identifier_string("=="))   != ids.end());
  assert(ids.find(identifier_string("!="))   != ids.end());
  assert(ids.find(identifier_string("&&"))   != ids.end());

  return 0;
}
//]
