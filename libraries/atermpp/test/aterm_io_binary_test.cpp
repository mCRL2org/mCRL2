// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_io_binary.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace atermpp;

BOOST_AUTO_TEST_CASE(simple_int_test)
{
  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);
    output << aterm_int(50);
  }

  binary_aterm_istream input(stream);
  aterm t;
  input.get(t);
  BOOST_CHECK_EQUAL(t, aterm_int(50));
}

BOOST_AUTO_TEST_CASE(simple_term_test)
{

  // The sequence of terms to send.
  std::vector<aterm_appl> sequence;

  aterm f = aterm_appl(function_symbol("f", 0));
  aterm g = aterm_appl(function_symbol("g", 0));

  sequence.emplace_back(function_symbol("test", 2), f, g);
  sequence.push_back(static_cast<const aterm_appl&>(f));
  sequence.emplace_back(function_symbol("nested", 2), sequence.front(), g);
  sequence.push_back(static_cast<const aterm_appl&>(g));
  sequence.emplace_back(function_symbol("deeply_nested", 3), sequence[3], sequence.front(), sequence[3]);

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);

    for (const auto& term : sequence)
    {
      output << term;
    }

    // The buffer is flushed here.
  }

  binary_aterm_istream input(stream);

  for (std::size_t index = 0; index < sequence.size(); ++index)
  {
    aterm t;
    input.get(t);
    BOOST_CHECK_EQUAL(t, sequence[index]);
  }
}

BOOST_AUTO_TEST_CASE(transitions_test)
{
  std::vector<aterm_appl> sequence;

  function_symbol transition("transition", 2);
  aterm label = aterm_appl(function_symbol("state", 1), aterm_int(0));
  aterm time = aterm_appl(function_symbol("time", 1), aterm_int(50));

  aterm_list states;

  for (std::size_t index = 0; index < 2; ++index)
  {
    sequence.emplace_back(transition, states, time);

    // Increase the state labels size.
    states.push_front(label);
  }

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);

    for (const auto& term : sequence)
    {
      output << term;
    }

    // The buffer is flushed here.
  }

  binary_aterm_istream input(stream);

  for (std::size_t index = 0; index < sequence.size(); ++index)
  {
    aterm t;
    input.get(t);
    BOOST_CHECK_EQUAL(t, sequence[index]);
  }
}
