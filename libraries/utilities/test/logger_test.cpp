// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file logger_test.cpp

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/utilities/logger.h"

using namespace mcrl2::log;

void print_all_log_levels()
{
  mCRL2log(error) << "An error message" << std::endl;
  mCRL2log(warning) << "A warning" << std::endl;
  mCRL2log(info) << "Some information" << std::endl;
  mCRL2log(verbose) << "Detailed information" << std::endl;
  mCRL2log(debug) << "Debugging info" << std::endl;
  mCRL2log(debug1) << "Detailed debugging info" << std::endl;
  mCRL2log(debug2) << "Detailed debugging info (2)" << std::endl;
  mCRL2log(debug3) << "Detailed debugging info (3)" << std::endl;
  mCRL2log(debug4) << "Detailed debugging info (4)" << std::endl;
  mCRL2log(debug5) << "Detailed debugging info (5)" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_logging_basic)
{
  // Messages will only be printed up to debug, due to standard MCRL2_MAX_LOG_LEVEL
  mcrl2_logger::set_reporting_level(debug2);
  print_all_log_levels();
}

BOOST_AUTO_TEST_CASE(test_logging_use_case)
{
  mCRL2log(info) << "Entering logging use case" << std::endl;
  const int count = 3;
  mCRL2log(debug) << "A loop with " << count << " iterations" << std::endl;
  for (int i = 0; i < count; ++i)
  {
    mCRL2log(debug1) << "the counter i = " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(debug2) << "the counter is greater then 2" << std::endl;
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

BOOST_AUTO_TEST_CASE(test_logging_hint)
{
  mcrl2_logger::set_reporting_level(info);
  mCRL2log(debug, "test_hint") << "Testing hint, should not be printed" << std::endl;
  mcrl2_logger::set_reporting_level(debug, "test_hint");
  mCRL2log(debug, "test_hint") << "Testing hint, should be printed" << std::endl;
  mCRL2log(debug) << "Testing hint, should not be printed" << std::endl;
  mcrl2_logger::set_reporting_level(verbose, "test_hint");
  mCRL2log(info) << "Testing hint, should still be printed" << std::endl;
  mcrl2_logger::clear_reporting_level("test_hint");
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

// Show that arguments to logging are not executed if the log level is larger
// than MCRL2_MAX_LOG_LEVEL (i.e. the BOOST_CHECK(false) in test_assert() should
// never be triggered.
BOOST_AUTO_TEST_CASE(test_non_execution_of_arguments_static)
{
  BOOST_CHECK(MCRL2_MAX_LOG_LEVEL < debug5);
  mCRL2log(debug5) << "This line should not end with BOOM! ............. " << test_assert() << std::endl;
}

// Show that arguments to logging are not executed if the log level is larger
// than mcrl2_logger::reporting_level() (i.e. the BOOST_CHECK(false) in test_assert() should
// never be triggered.
BOOST_AUTO_TEST_CASE(test_non_execution_of_arguments_dynamic)
{
  BOOST_CHECK(MCRL2_MAX_LOG_LEVEL >= debug);
  mcrl2_logger::set_reporting_level(verbose);
  mCRL2log(debug) << "This line should not end with BOOM! ............. " << test_assert() << std::endl;
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

