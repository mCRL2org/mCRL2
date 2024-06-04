// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE ldd_stream_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/test_utility.h"
#include "mcrl2/symbolic/ldd_stream.h"

#include <sylvan_ldd.hpp>

#include <vector>
#include <random>

using sylvan::ldds::ldd;
using namespace mcrl2::symbolic;

BOOST_AUTO_TEST_CASE(random_test_ldd_stream)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    ldd input = random_set(5000, 5, 10);

    std::stringstream stream;
    {
      binary_ldd_ostream output(stream);
      output << input;

      // The buffer is flushed here.
    }

    binary_ldd_istream read(stream);
    ldd result;
    read >> result;

    BOOST_CHECK_EQUAL(input, result);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_ldd_stream_sequence)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    std::vector<ldd> input = std::vector<ldd>(10);
    for (ldd& set : input)
    {
      set = random_set(500, 5, 10);
    }

    std::stringstream stream;
    {
      binary_ldd_ostream output(stream);
      for (ldd& set : input)
      {        
        output << set;
      }

      // The buffer is flushed here.
    }

    binary_ldd_istream read(stream);
    std::vector<ldd> result = std::vector<ldd>(10);
    for (ldd& set : result)
    {
      read >> set;
    }

    for (std::size_t i = 0; i < input.size(); ++i)
    {
      BOOST_CHECK_EQUAL(input[i], result[i]);
    }
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_simple_cases)
{
  initialise_sylvan();

  std::stringstream stream;
  {
    binary_ldd_ostream output(stream);
    output << sylvan::ldds::false_();
    output << sylvan::ldds::true_();

    // The buffer is flushed here.
  }

  binary_ldd_istream read(stream);

  ldd result1;
  ldd result2;
  
  read >> result1;
  read >> result2;

  BOOST_CHECK_EQUAL(result1, sylvan::ldds::false_());
  BOOST_CHECK_EQUAL(result2, sylvan::ldds::true_());

  quit_sylvan();
}

#endif // MCRL2_ENABLE_SYLVAN