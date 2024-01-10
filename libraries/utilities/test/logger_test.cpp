// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file logger_test.cpp

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/logger.h"

#include <thread>

using namespace mcrl2::log;

void print_all_log_levels()
{
  mCRL2log(error) << "An error message" << std::endl;
  mCRL2log(warning) << "A warning" << std::endl;
  mCRL2log(info) << "Some information" << std::endl;
  mCRL2log(verbose) << "Detailed information" << std::endl;
  mCRL2log(debug) << "Debugging info" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_logging_use_case)
{
  mCRL2log(info) << "Entering logging use case" << std::endl;
  const int count = 3;
  mCRL2log(debug) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(debug) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(debug) << "the counter is greater then 2" << std::endl;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_indentation)
{
  mCRL2log(info) << "Entering indentation test" << std::endl;
  const int count = 3;
  mCRL2log(info) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(debug) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(debug) << "the counter is greater then 2" << std::endl;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_logging_multiline)
{
  mCRL2log(info) << "Testing multiline logging (line 1)" << std::endl
                 << "line 2" << std::endl
                 << "the last last line" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_file_logging)
{
  FILE * pFile;
  pFile = fopen ("logger_test_file.txt" , "w");
  BOOST_REQUIRE(pFile != nullptr);

  file_output::set_stream(pFile);
  mCRL2log(info) << "This line is written to logger_test_file.txt" << std::endl;
  file_output::set_stream(stderr);
  fclose(pFile);
  mCRL2log(info) << "This line is written to stderr" << std::endl;
}

std::string test_assert()
{
  BOOST_CHECK(false);
  return "BOOM!";
}

BOOST_AUTO_TEST_CASE(test_fflush)
{
  for(int i = 0; i < 10; ++i)
  {
    fprintf(stderr, "%d", i);
    fflush(stderr);
  }
  fprintf(stderr, "\n");
  fflush(stderr);
}

BOOST_AUTO_TEST_CASE(test_multiline_nonewline)
{
  mCRL2log(info) << "There is just one newline";
  mCRL2log(info) << "in this message" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_parallel_logging)
{
  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([]() {
      mCRL2log(info) << "A message";
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }
}