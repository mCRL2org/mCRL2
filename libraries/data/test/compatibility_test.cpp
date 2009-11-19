// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compatibility_test.cpp
/// \brief Regression test for the transformations between new and old data
///       format.

#include <iostream>
#include <sstream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/data_specification.h"

void compatibility_test()
{
  std::stringstream data_stream(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  atermpp::aterm_appl data_spec_aterm = mcrl2::core::parse_data_spec(data_stream);
  if (data_spec_aterm == 0) {
    throw mcrl2::runtime_error("Error while parsing data specification");
  }
  data_spec_aterm = mcrl2::core::type_check_data_spec(data_spec_aterm);
  if (data_spec_aterm == 0)
    throw mcrl2::runtime_error("Error while type checking data specification");
  
  atermpp::aterm_appl data_spec_aterm_new =
    mcrl2::data::detail::data_specification_to_aterm_data_spec(
      // mcrl2::data::remove_all_system_defined(
        mcrl2::data::data_specification(data_spec_aterm)
      // )
    );

  BOOST_CHECK(data_spec_aterm == data_spec_aterm_new);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  compatibility_test();

  return EXIT_SUCCESS;
}
