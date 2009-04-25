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
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_specification_compatibility.h"

using namespace mcrl2;
using namespace atermpp;

void compatibility_test()
{
  const std::string text(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  // Create fake lps stream
  // std::stringstream data_stream;
  // data_stream << text;
  // data_stream << "init delta;\n";

  // aterm_appl spec_old_format(data::parse_data_specification(text));
  // lps_spec = data::detail::type_check_specification(lps_spec);
  // lps_spec = data::detail::alpha_reduce(lps_spec);

  // aterm_appl spec_old_format = atermpp::arg1(lps_spec);
  // data::data_specification spec_new_format(spec_old_format);
  // aterm_appl spec_old_format1 = data::detail::data_specification_to_aterm_data_spec(remove_all_system_defined(spec_new_format));

  // BOOST_CHECK(spec_old_format == spec_old_format1);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  compatibility_test();

  return EXIT_SUCCESS;
}
