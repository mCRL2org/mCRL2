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


// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, random_test_ldd_stream_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define random_test_ldd_stream_task(a) RUN(random_test_ldd_stream_task, a)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, random_test_ldd_stream_task)
{ 

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

  return true;
}

BOOST_AUTO_TEST_CASE(random_test_ldd_stream)
{
  initialise_sylvan();

  random_test_ldd_stream_task();

  quit_sylvan();
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, random_test_ldd_stream_sequence_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define random_test_ldd_stream_sequence_task(a) RUN(random_test_ldd_stream_sequence_task, a)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, random_test_ldd_stream_sequence_task)
{ 
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

  return true;
}

BOOST_AUTO_TEST_CASE(random_test_ldd_stream_sequence)
{
  initialise_sylvan();

  random_test_ldd_stream_sequence_task();

  quit_sylvan();
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, random_test_ldd_stream_simple_cases_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define random_test_ldd_stream_simple_cases_task(a) RUN(random_test_ldd_stream_simple_cases_task, a)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, random_test_ldd_stream_simple_cases_task)
{ 
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

  return true;
}

BOOST_AUTO_TEST_CASE(random_test_simple_cases)
{
  initialise_sylvan();

  random_test_ldd_stream_simple_cases_task();

  quit_sylvan();
}

/// \brief Writes U to a stream and reads it back.
static ldd write_and_read(const ldd& U)
{
  std::stringstream stream;
  {
    binary_ldd_ostream output(stream);
    output << U;

    // The buffer is flushed here.
  }

  binary_ldd_istream read(stream);
  ldd result;
  read >> result;
  return result;
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, test_ldd_stream_copy_nodes_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define test_ldd_stream_copy_nodes_task(a) RUN(test_ldd_stream_copy_nodes_task, a)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, test_ldd_stream_copy_nodes_task)
{
  using namespace sylvan::ldds;

  // Copy nodes, as union_cube_copy makes them in the transition relations of summand groups. In the first level a copy
  // node is followed by an ordinary node with value 0 (Sylvan stores 0 as the value of a copy node).
  ldd input = cube_copy({0, 5, 0}, {1, 0, 1});
  input = union_cube_copy(input, {0, 5, 2}, {0, 0, 0});
  input = union_cube_copy(input, {3, 0, 4}, {0, 1, 0});
  BOOST_CHECK(sylvan::lddmc_iscopy(input.get()));
  BOOST_CHECK(!sylvan::lddmc_iscopy(input.right().get()));
  BOOST_CHECK_EQUAL(input.right().value(), 0u);

  ldd result = write_and_read(input);
  BOOST_CHECK(result == input);
  BOOST_CHECK(sylvan::lddmc_iscopy(result.get()));

  // Random relations with copy nodes.
  std::mt19937 generator(42);
  std::uniform_int_distribution<std::uint32_t> value(0, 3);
  std::bernoulli_distribution copy(0.3);
  std::size_t different = 0;
  for (std::size_t i = 0; i < 100; ++i)
  {
    input = empty_set();
    for (std::size_t j = 0; j < 200; ++j)
    {
      std::vector<std::uint32_t> values(6);
      std::vector<int> copies(6);
      for (std::size_t k = 0; k < values.size(); ++k)
      {
        values[k] = value(generator);
        copies[k] = copy(generator) ? 1 : 0;
      }
      input = union_cube_copy(input, values, copies);
    }

    if (write_and_read(input) != input)
    {
      ++different;
    }
  }
  BOOST_CHECK_EQUAL(different, 0u);

  return true;
}

BOOST_AUTO_TEST_CASE(test_ldd_stream_copy_nodes)
{
  initialise_sylvan();

  test_ldd_stream_copy_nodes_task();

  quit_sylvan();
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, test_ldd_stream_version_without_copy_nodes_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define test_ldd_stream_version_without_copy_nodes_task(a) RUN(test_ldd_stream_version_without_copy_nodes_task, a)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, test_ldd_stream_version_without_copy_nodes_task)
{
  // The LDD {[3]} in version 0x8306 of the format, which does not store whether a node is a copy node.
  std::stringstream stream;
  {
    mcrl2::utilities::obitstream output(stream);
    output.write_bits(0x8baf, 16); // BLF_MAGIC
    output.write_bits(0x8306, 16); // BLF_VERSION
    output.write_bits(0, 1);       // a new node with value 3, down true (index 1) and right false (index 0)
    output.write_integer(3);
    output.write_bits(1, 2);
    output.write_bits(0, 2);
    output.write_bits(1, 1);       // output the node with index 2
    output.write_bits(2, 2);
  }

  binary_ldd_istream read(stream);
  ldd result;
  read >> result;
  BOOST_CHECK(result == sylvan::ldds::cube({3}));

  return true;
}

BOOST_AUTO_TEST_CASE(test_ldd_stream_version_without_copy_nodes)
{
  initialise_sylvan();

  test_ldd_stream_version_without_copy_nodes_task();

  quit_sylvan();
}

#endif // MCRL2_ENABLE_SYLVAN