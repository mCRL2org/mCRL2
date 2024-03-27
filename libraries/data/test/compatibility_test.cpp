// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compatibility_test.cpp
/// \brief Regression test for the transformations between new and old data
///       format.

#define BOOST_TEST_MODULE compatibility_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/core/parse.h"
#include "mcrl2/data/data_io.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2;

void compatibility_test()
{
  std::stringstream data_stream(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  std::string text = utilities::read_text(data_stream);
  atermpp::aterm data_spec_aterm = data::detail::data_specification_to_aterm(data::detail::parse_data_specification_new(text));
  if (data_spec_aterm == atermpp::aterm())
  {
    throw mcrl2::runtime_error("Error while parsing data specification");
  }
  data::data_specification data_spec(data_spec_aterm);
  data::typecheck_data_specification(data_spec);

  atermpp::aterm data_spec_aterm_new =
    mcrl2::data::detail::data_specification_to_aterm(
      data_spec
      // mcrl2::data::remove_all_system_defined(
      // mcrl2::data::data_specification(data_spec_aterm)
      // )
    );

  BOOST_CHECK(data_spec_aterm == data_spec_aterm_new);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  compatibility_test();
}
