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

#include <iostream>
#include <thread>

using namespace mcrl2::log;

void print_all_log_levels()
{
  mCRL2log(log_level_t::error) << "An error message" << std::endl;
  mCRL2log(log_level_t::warning) << "A warning" << std::endl;
  mCRL2log(log_level_t::info) << "Some information" << std::endl;
  mCRL2log(log_level_t::verbose) << "Detailed information" << std::endl;
  mCRL2log(log_level_t::debug) << "Debugging info" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_logging_use_case)
{
  mCRL2log(log_level_t::info) << "Entering logging use case" << std::endl;
  const int count = 3;
  mCRL2log(log_level_t::debug) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(log_level_t::debug) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(log_level_t::debug) << "the counter is greater then 2" << std::endl;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_indentation)
{
  mCRL2log(log_level_t::info) << "Entering indentation test" << std::endl;
  const int count = 3;
  mCRL2log(log_level_t::info) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(log_level_t::debug) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(log_level_t::debug) << "the counter is greater then 2" << std::endl;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_logging_multiline)
{
  mCRL2log(log_level_t::info) << "Testing multiline logging (line 1)" << std::endl
                 << "line 2" << std::endl
                 << "the last last line" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_file_logging)
{
  FILE * pFile;
  pFile = fopen ("logger_test_file.txt" , "w");
  BOOST_REQUIRE(pFile != nullptr);

  file_output::set_stream(pFile);
  mCRL2log(log_level_t::info) << "This line is written to logger_test_file.txt" << std::endl;
  file_output::set_stream(stderr);
  fclose(pFile);
  mCRL2log(log_level_t::info) << "This line is written to stderr" << std::endl;
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
    std::cerr << i << std::flush;
  }
  std::cerr << '\n' << std::flush;
}

BOOST_AUTO_TEST_CASE(test_multiline_nonewline)
{
  mCRL2log(log_level_t::info) << "There is just one newline";
  mCRL2log(log_level_t::info) << "in this message" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_parallel_logging)
{
  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i) {
    threads.emplace_back([]() {
      mCRL2log(log_level_t::info) << "A message";
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

BOOST_AUTO_TEST_CASE(test_scoped_reporting_level)
{
  const log_level_t global_level = logger::get_reporting_level();
  BOOST_CHECK(logger::get_reporting_level() == log_level_t::info);

  {
    // The scope overrides the reporting level on the current thread only.
    scoped_reporting_level scope(log_level_t::quiet);
    BOOST_CHECK(logger::get_reporting_level() == log_level_t::quiet);
    BOOST_CHECK(!mCRL2logEnabled(global_level));

    // Other threads keep using the global reporting level.
    std::atomic<bool> other_thread_sees_global{false};
    std::thread other([&]() {
      other_thread_sees_global = (logger::get_reporting_level() == global_level);
    });
    other.join();
    BOOST_CHECK(other_thread_sees_global.load());
  }

  // After the scope the global level is in effect again.
  BOOST_CHECK(logger::get_reporting_level() == global_level);
}
