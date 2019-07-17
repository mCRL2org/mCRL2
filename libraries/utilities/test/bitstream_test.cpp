// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/bitstream.h"

#include <boost/test/included/unit_test_framework.hpp>

using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(fixed_sequence_test)
{
  std::stringstream stream;

  {
    obitstream input(stream);
    input.write_integer(50);
    input.write_bits(7, 3);

    input.write_string("test");
    input.write_integer(1337);

    input.write_string("function_symbol");
    input.write_integer(5);

    // The buffer is flushed here.
  }

  // Check the resulting stream.
  ibitstream output(stream);
  BOOST_CHECK(output.read_integer() == 50);
  BOOST_CHECK(output.read_bits(3) == 7);

  BOOST_CHECK(strcmp(output.read_string(), "test") == 0);
  BOOST_CHECK(output.read_integer() == 1337);

  BOOST_CHECK(strcmp(output.read_string(), "function_symbol") == 0);
  BOOST_CHECK(output.read_integer() == 5);
}

boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return nullptr;
}
