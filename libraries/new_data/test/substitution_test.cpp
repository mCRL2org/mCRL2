// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <utility>

#include <boost/test/minimal.hpp>

#include "mcrl2/new_data/lambda.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/detail/concepts.h"

using namespace mcrl2;
using namespace mcrl2::new_data;

data_expression operator+(data_expression const& l, data_expression const& r) {
  return sort_nat::plus(l, r);
}

data_expression operator*(data_expression const& l, data_expression const& r) {
  return sort_nat::times(l, r);
}

int test_main(int a, char**aa) {
  MCRL2_ATERMPP_INIT(a, aa);
  using namespace mcrl2::new_data::sort_nat;

  variable        x("x", sort_nat::nat());
  variable        y("y", sort_nat::nat());
  data_expression e(variable("z", sort_nat::nat()) + (x + y));

  using mcrl2::new_data::concepts::MutableSubstitution;

  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< variable, data_expression > >));
  BOOST_CONCEPT_ASSERT((MutableSubstitution< mutable_map_substitution< variable, variable > >));

  mutable_map_substitution< variable, data_expression > s;

  BOOST_CHECK(static_cast< variable >(s(x)) == x);
  BOOST_CHECK(static_cast< variable >(s(y)) != x);

  function_symbol c("c", sort_nat::nat());

  BOOST_CHECK(c + x == c + x);
  BOOST_CHECK(s(data_expression(c + x)) == c + x);
  BOOST_CHECK(s(data_expression(c + x * y )) == c + x * y);

  s[y] = c;

  BOOST_CHECK(static_cast< data_expression >(s(x)) == x);
  BOOST_CHECK(static_cast< data_expression >(s(y)) == c);
  BOOST_CHECK(s(y) == c);
  BOOST_CHECK(s(c + x * y) == c + x * c);
  BOOST_CHECK(s(lambda(x,x)) == lambda(x,x));
  BOOST_CHECK(s(lambda(x,y)) == lambda(x,c));

  // non-capture avoiding substitution
  BOOST_CHECK(s(lambda(y,y)) != lambda(y,y));
  BOOST_CHECK(s(application(lambda(y,y),x) + y) != application(lambda(y,y), x) + c);

  return EXIT_SUCCESS;
}
