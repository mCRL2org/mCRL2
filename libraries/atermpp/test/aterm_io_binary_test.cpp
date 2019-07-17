// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_io.h"

#include <boost/test/included/unit_test_framework.hpp>

#include <vector>

using namespace std;
using namespace atermpp;

BOOST_AUTO_TEST_CASE(simple_term_test)
{
  std::stringstream stream;

  // The sequence of terms to send.
  std::vector<aterm_appl> sequence;

  aterm f = aterm_appl(function_symbol("f", 0));
  aterm g = aterm_appl(function_symbol("g", 0));

  sequence.emplace_back(function_symbol("test", 2), f, g);
  sequence.push_back(static_cast<const aterm_appl&>(f));
  sequence.emplace_back(function_symbol("nested", 2), sequence.front(), g);
  sequence.push_back(static_cast<const aterm_appl&>(g));
  sequence.emplace_back(function_symbol("nested", 2), sequence.front(), sequence.front());

  {
    binary_aterm_output input(stream);

    for (const auto& term : sequence)
    {
      input.write_term(term);
    }

    // The buffer is flushed here.
  }

  binary_aterm_input output(stream);

  for (std::size_t index = 0; index < sequence.size(); ++index)
  {
    BOOST_CHECK(output.read_term() == sequence[index]);
  }
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
