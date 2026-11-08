// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_identifier_generator_test.cpp
/// \brief Tests that identifier generators created in different translation units
///        do not hand out the same names.

#define BOOST_TEST_MODULE enumerator_identifier_generator_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/rewriter.h"

#include <set>

using namespace mcrl2;
using namespace mcrl2::data;

// The generator of the rewriter is created inside the mcrl2_data library, and the generator below is created in this
// translation unit. Both use the default prefix "x_", so their names must still be different.
BOOST_AUTO_TEST_CASE(test_rewriter_and_user_generator_are_distinct)
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter R(data_spec);
  enumerator_identifier_generator id_generator;

  std::set<core::identifier_string> rewriter_names;
  auto& rewriter_generator = const_cast<enumerator_identifier_generator&>(R.identifier_generator());
  for (int i = 0; i < 10; ++i)
  {
    rewriter_names.insert(rewriter_generator());
  }

  for (int i = 0; i < 10; ++i)
  {
    const core::identifier_string name = id_generator();
    BOOST_CHECK_MESSAGE(rewriter_names.count(name) == 0, "the name " << name << " is also used by the rewriter");
  }
}

// The rewriter binds a fresh variable of its own generator for y in the substitution when it rewrites a where clause.
// If that variable has the same name as the free variable x, which comes from a generator in this translation unit,
// then x is captured and x + y whr y = 3 end rewrites to 3 + 3.
BOOST_AUTO_TEST_CASE(test_where_clause_does_not_capture_fresh_variable)
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter R(data_spec);
  enumerator_identifier_generator id_generator;

  const variable x(id_generator(), sort_nat::nat());
  const variable y("y", sort_nat::nat());
  const data_expression three = sort_nat::nat(3);

  const data_expression t = where_clause(sort_nat::plus(x, y), assignment_expression_list({ assignment(y, three) }));
  BOOST_CHECK_EQUAL(R(t), R(sort_nat::plus(x, three)));
}
